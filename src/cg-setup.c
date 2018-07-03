/******************************************************************************
* Copyright (C) 2018  Billy Kozak                                             *
*                                                                             *
* This file is part of the cg-bootstrap software.                             *
*                                                                             *
* This program is free software: you can redistribute it and/or modify        *
* it under the terms of the GNU Lesser General Public License as published by *
* the Free Software Foundation, either version 3 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU Lesser General Public License for more details.                         *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.       *
******************************************************************************/
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "cg-setup.h"

#include "conf-parse.h"
#include "memutl.h"
#include "strutl.h"
#include "pathutl.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <linux/magic.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
/******************************************************************************
*                              PRIVATE FUNCTIONS                              *
******************************************************************************/
static char *cgroup_tasks_path(
	const struct prog_conf *conf, struct mem_chunk *chunk, size_t i
) {
	return path_join(
		chunk,
		conf->cgroup_prefix,
		conf->controllers[i].controller,
		conf->controllers[i].group,
		"tasks"
	);
}
/*****************************************************************************/
static char *cgroup_path(
	const struct prog_conf *conf, struct mem_chunk *chunk, size_t i
) {
	return path_join(
		chunk,
		conf->cgroup_prefix,
		conf->controllers[i].controller,
		conf->controllers[i].group
	);
}
/*****************************************************************************/
static char *cgroup_root_path(
	const struct prog_conf *conf, struct mem_chunk *chunk, size_t i
) {
	return path_join(
		chunk,
		conf->cgroup_prefix,
		conf->controllers[i].controller
	);
}
/*****************************************************************************/
static char *param_path(
	const struct prog_conf *conf, struct mem_chunk *chunk, size_t index
) {
	char *group = NULL;
	char *controller = NULL;
	struct cgroup_param param = conf->params[index];
	struct lstr param_controller = chop(
		param.name, strlen(param.name), '.'
	);

	for(size_t i = 0; i < conf->num_controllers; i++) {
		char *this_controller = conf->controllers[i].controller;
		char *this_group = conf->controllers[i].group;

		if(lstrcmp2(param_controller, this_controller) == 0) {
			group = this_group;
			controller = this_controller;
			break;
		}
	}

	if(controller == NULL) {
		return NULL;
	}

	return path_join(
		chunk,
		conf->cgroup_prefix,
		controller,
		group,
		param.name
	);
}
/*****************************************************************************/
static int pre_validate(const struct prog_conf *conf)
{
	struct stat sb;
	struct statfs sfs;
	struct mem_chunk tmp_mem = empty_chunk();

	if(stat(conf->cgroup_prefix, &sb)) {
		perror("Prefix is invalid");
		return 1;
	} else if(!S_ISDIR(sb.st_mode)) {
		fprintf(
			stderr,
			"Prefix '%s' is not a directory.\n",
			conf->cgroup_prefix
		);
		goto fail;
	}

	for(size_t i = 0; i < conf->num_controllers; i++) {
		char *path = cgroup_root_path(conf, &tmp_mem, i);

		if(stat(path, &sb) || !S_ISDIR(sb.st_mode)) {
			fprintf(
				stderr,
				"No cgroup file-system at '%s'.\n",
				path
			);
			goto fail;
		} else if(statfs(path, &sfs)) {
			/* should rarely if ever get here since we just
			checked that the path exists */
			perror(NULL);
			fprintf(
				stderr,
				"No cgroup file-system at '%s'.\n",
				path
			);
			goto fail;
		} else if(sfs.f_type != CGROUP_SUPER_MAGIC) {
			fprintf(
				stderr,
				"File system at '%s' is not a cgroup fs.\n",
				path
			);
			goto fail;
		}
	}
	for(size_t i = 0; i < conf->num_params; i++) {
		char *path = param_path(conf, &tmp_mem, i);
		if(path == NULL && errno) {
			perror(NULL);
			goto fail;
		} else if(path == NULL) {
			fprintf(
				stderr,
				"No cgroup specified for param '%s'.\n",
				conf->params[i].name
			);
			goto fail;
		}
	}

	free_chunk(&tmp_mem);
	return 0;

fail:
	free_chunk(&tmp_mem);
	return 1;
}
/*****************************************************************************/
static int create_missing_cgroups(const struct prog_conf *conf)
{
	struct mem_chunk chunk = empty_chunk();
	struct stat sb;

	for(size_t i = 0; i < conf->num_controllers; i++) {
		char *path = cgroup_path(conf, &chunk, i);
		if(path == NULL) {
			perror(NULL);
			goto fail;
		}
		if(stat(path, &sb)) {
			if(mkpath(path)) {
				perror(NULL);
				fprintf(
					stderr,
					"Unable to create cgroup at '%s'.\n",
					path
				);
				goto fail;
			}
		}
	}

	free_chunk(&chunk);
	return 0;
fail:
	free_chunk(&chunk);
	return 1;
}
/*****************************************************************************/
static int add_self_to_cgroups(const struct prog_conf *conf)
{
	struct mem_chunk chunk = empty_chunk();
	pid_t pid = getpid();

	for(size_t i = 0; i < conf->num_controllers; i++) {
		char *path;
		int fd;
		FILE *fp;

		path = cgroup_tasks_path(conf, &chunk, i);
		if(path == NULL) {
			perror(NULL);
			goto fail;
		}
		fd = open(path,O_TRUNC | O_WRONLY);
		if(fd < 0) {
			perror(NULL);
			fprintf(stderr, "Unable to write to '%s'.\n", path);
			goto fail;
		}
		fp = fdopen(fd, "w");
		if(fp == NULL) {
			perror(NULL);
			close(fd);
			goto fail;
		}

		fprintf(fp, "%u\n", pid);
		fclose(fp);
	}

	free_chunk(&chunk);
	return 0;
fail:
	free_chunk(&chunk);
	return 1;
}
/*****************************************************************************/
static int set_cgroup_params(const struct prog_conf *conf)
{
	struct mem_chunk chunk = empty_chunk();

	for(size_t i = 0; i < conf->num_params; i++) {
		char *path = param_path(conf, &chunk, i);
		char *val = conf->params[i].value;
		size_t val_len = strlen(val);
		int fd = open(path, O_TRUNC | O_WRONLY);
		if(fd < 0) {
			perror(NULL);
			fprintf(stderr, "Unable to write to '%s'\n", path);
			goto fail;
		}
		errno = 0;
		if(write(fd, val, val_len) != val_len) {
			if(errno) {
				perror(NULL);
			}
			close(fd);
			fprintf(
				stderr,
				"Unable to write value '%s' to '%s'.\n",
				val, path
			);
			goto fail;
		}
		close(fd);
	}

	free_chunk(&chunk);
	return 0;

fail:
	free_chunk(&chunk);
	return 1;
}
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
int setup_cgroups(const struct prog_conf *conf)
{
	if(pre_validate(conf)) {
		fprintf(
			stderr,
			"The configuration is invalid and so "
			"will not be applied.\n"
		);
		return 1;
	}
	if(create_missing_cgroups(conf)) {
		fprintf(
			stderr,
			"Unable to create new cgroups.\n"
		);
		return 1;
	}
	if(add_self_to_cgroups(conf)) {
		fprintf(
			stderr,
			"Unable to add self to cgroups.\n"
		);
		return 1;
	}
	if(set_cgroup_params(conf)) {
		fprintf(
			stderr,
			"Unable to set cgroup parameters.\n"
		);
		return 1;
	}

	return 0;
}
/*****************************************************************************/