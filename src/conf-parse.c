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
#include "conf-parse.h"

#include "strutl.h"
#include "memutl.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
/******************************************************************************
*                                    TYPES                                    *
******************************************************************************/
enum section_parse_result {
	SECTION_PARSE_ERR,
	SECTION_PARSE_CHANGED,
	SECTION_PARSE_NO_SECTION
};
/*****************************************************************************/
enum conf_section {
	CONF_INIT = -1,
	CONF_GENERAL,
	CONF_CONTROLLER_GROUPS,
	CONF_GROUP_PARAMS,

	CONF_NUM_SECTIONS
};
/*****************************************************************************/
struct parser_state {
	enum conf_section section;
	struct prog_conf conf;

	size_t controllers_mem_size;
	size_t params_mem_size;
};
/*****************************************************************************/
struct param_line {
	struct lstr param;
	struct lstr val;
};
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static const char *SECTION_NAMES[] = {
	[CONF_GENERAL] = "general",
	[CONF_CONTROLLER_GROUPS] = "groups",
	[CONF_GROUP_PARAMS] = "params"
};
/*****************************************************************************/
static const char DEFAULT_CGROUP_PREFIX[] = "/sys/fs/cgroup/";
/*****************************************************************************/
static const size_t INITIAL_CONTROLLER_MEM_SIZE = 32u;
/*****************************************************************************/
static const size_t INITIAL_PARAM_MEM_SIZE = 64u;
/******************************************************************************
*                              PRIVATE FUNCTIONS                              *
******************************************************************************/
static int process_group_params(
	const struct lstr *param,
	const struct lstr *val,
	struct parser_state *state
) {
	struct cgroup_param *p;
	if(state->params_mem_size == state->conf.num_params) {
		void *mem = realloc(
			state->conf.params,
			state->params_mem_size * 2
			* sizeof(*state->conf.params)
		);
		if(mem == NULL) {
			perror("");
			return 1;
		}
		state->conf.params = mem;
		state->params_mem_size *= 2;
	}
	p = &state->conf.params[state->conf.num_params];
	state->conf.num_params += 1;

	p->name = lstrcpy2(*param);
	p->value = lstrcpy2(*val);

	if(anynull(p->name, p->value)) {
		perror(NULL);
		return 1;
	}

	return 0;
}
/*****************************************************************************/
static int process_controller_groups(
	const struct lstr *param,
	const struct lstr *val,
	struct parser_state *state
) {
	struct cgroup_controller *ctrlr;
	if(state->controllers_mem_size == state->conf.num_controllers) {
		void *mem = realloc(
			state->conf.controllers,
			state->controllers_mem_size * 2
			* sizeof(*state->conf.controllers)
		);
		if(mem == NULL) {
			perror("");
			return 1;
		}
		state->conf.controllers = mem;
		state->controllers_mem_size *= 2;
	}
	ctrlr = &state->conf.controllers[state->conf.num_controllers];
	state->conf.num_controllers += 1;

	ctrlr->group = lstrcpy2(*val);
	ctrlr->controller = lstrcpy2(*param);

	if(anynull(ctrlr->group, ctrlr->controller)) {
		perror(NULL);
		return 1;
	}

	return 0;
}
/*****************************************************************************/
static int process_cgroup_prefix(
	const struct lstr *val, struct parser_state * state
) {
	char *mem = realloc(state->conf.cgroup_prefix, val->len + 1);

	if(mem == NULL) {
		perror(NULL);
		return 1;
	}
	lstrcpy(mem, *val);
	state->conf.cgroup_prefix = mem;

	return 0;
}
/*****************************************************************************/
static int process_general(
	const struct lstr *param,
	const struct lstr *val,
	struct parser_state *state
) {
	if(lstrcmp2(*param, "prefix") == 0) {
		return process_cgroup_prefix(val, state);
	}

	char *param_cstr = lstrcpy2(*param);
	if(param_cstr) {
		fprintf(
			stderr,
			"Paramater name '%s' is not valid in the 'general' "
			"section.\n",
			param_cstr
		);
	} else {
		perror("Parameter is not valid in 'general' section.");
	}
	free(param_cstr);
	return 1;
}
/*****************************************************************************/
static enum section_parse_result parse_section(
	const char *line, size_t len, struct parser_state *state
) {
	struct lstr section;
	struct lstr stripped = strip(line, len);

	if(stripped.len == 0 || stripped.str[0] != '[') {
		return SECTION_PARSE_NO_SECTION;
	} else if(stripped.len == 1 || stripped.str[stripped.len - 1] != ']') {
		return SECTION_PARSE_ERR;
	}
	section = strip(stripped.str + 1, stripped.len - 2);

	for(int i = 0; i < CONF_NUM_SECTIONS; i++) {
		if(lstrcmp2(section, SECTION_NAMES[i]) == 0) {
			state->section = (enum conf_section)i;
			return SECTION_PARSE_CHANGED;
		}
	}

	char *tmp_section = lstrcpy2(section);
	if(tmp_section) {
		fprintf(stderr, "No section named '%s'.\n", tmp_section);
	}
	free(tmp_section);

	return SECTION_PARSE_ERR;
}
/*****************************************************************************/
static int parse_param_line(
	struct param_line *ret, struct lstr uncomment
) {
	struct lstr param;
	struct lstr val;

	param = chopl(uncomment, '=');

	if(param.len >= uncomment.len) {
		fprintf(stderr, "Error: config parameter without value\n");
		return 1;
	}
	val.str = param.str + param.len + 1;
	val.len = uncomment.len - param.len - 1;

	param = stripl(param);
	val = stripl(val);

	if(val.len == 0) {
		fprintf(stderr, "Error: empty value\n");
		return 1;
	}
	if(param.len == 0) {
		fprintf(stderr, "Error: empty param\n");
		return 1;
	}

	ret->param = param;
	ret->val = val;

	return 0;
}
/*****************************************************************************/
static int parse_line(const char *line, size_t len, struct parser_state *state)
{
	struct lstr uncomment = stripl(chop(line, len, '#'));
	enum section_parse_result sec_res;
	struct param_line param_line;

	if(uncomment.len == 0) {
		return 0;
	}

	sec_res = parse_section(
		uncomment.str, uncomment.len, state
	);

	if(sec_res == SECTION_PARSE_ERR) {
		return 1;
	} else if(sec_res == SECTION_PARSE_CHANGED) {
		return 0;
	}

	if(parse_param_line(&param_line, uncomment)) {
		return 1;
	}

	switch(state->section) {
	case CONF_GENERAL:
		return process_general(
			&param_line.param, &param_line.val, state
		);
	case CONF_CONTROLLER_GROUPS:
		return process_controller_groups(
			&param_line.param, &param_line.val, state
		);
	case CONF_GROUP_PARAMS:
		return process_group_params(
			&param_line.param, &param_line.val, state
		);
	default:
		fprintf(
			stderr,
			"Error: parameter assignment outside of section.\n"
		);
		return 1;
	}

	return 0;
}
/*****************************************************************************/
static struct prog_conf* complete_parsing(struct parser_state *state)
{
	struct prog_conf *conf = malloc(sizeof(state->conf));

	if(conf == NULL) {
		destroy_conf(&state->conf);
		perror(NULL);
		return NULL;
	}

	memcpy(conf, &state->conf, sizeof(state->conf));

	return conf;
}
/*****************************************************************************/
static int init_parser_state(struct parser_state *state)
{
	struct prog_conf *conf = &state->conf;

	state->section = CONF_INIT;
	state->controllers_mem_size = INITIAL_CONTROLLER_MEM_SIZE;
	state->params_mem_size = INITIAL_PARAM_MEM_SIZE;
	conf->num_controllers = 0;
	conf->num_params = 0;

	conf->cgroup_prefix = malloc(sizeof(DEFAULT_CGROUP_PREFIX));
	conf->controllers = calloc(
		sizeof(*(state->conf.controllers)),
		INITIAL_CONTROLLER_MEM_SIZE
	);
	conf->params = calloc(
		sizeof(*(conf->params)),
		INITIAL_PARAM_MEM_SIZE
	);

	if(anynull(conf->cgroup_prefix, conf->controllers, conf->params)) {
		goto fail;
	}

	memcpy(
		conf->cgroup_prefix,
		DEFAULT_CGROUP_PREFIX,
		sizeof(DEFAULT_CGROUP_PREFIX)
	);

	return 0;

fail:
	free(conf->controllers);
	free(conf->cgroup_prefix);
	free(conf->params);

	conf->controllers = NULL;
	conf->cgroup_prefix = NULL;
	conf->params = NULL;

	return 1;
}
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
struct prog_conf *parse_conf(const char *conf_path)
{
	char *line = NULL;
	size_t line_buffer_size = 0;
	ssize_t len = 0;
	struct parser_state state;
	int line_num = 1;

	FILE *fp = fopen(conf_path, "r");

	if(fp == NULL) {
		perror("Error: unable to read configuration file");
		goto fail;
	}

	if(init_parser_state(&state)) {
		perror("Error initilizing parser");
		goto fail;
	}

	while((len = getline(&line, &line_buffer_size, fp)) != -1) {
		if(parse_line(line, len, &state)) {
			fprintf(
				stderr,
				"Config error on line %d.\n",
				line_num
			);
			goto fail;
		}
		line_num += 1;
	}

	if(errno) {
		perror("Error reading config file");
		goto fail;
	}

	free(line);
	fclose(fp);
	return complete_parsing(&state);

fail:
	free(line);
	fclose(fp);
	return NULL;
}
/*****************************************************************************/
void destroy_conf(struct prog_conf *conf)
{
	if(conf == NULL) {
		return;
	}

	for(size_t i = 0; i < conf->num_controllers; i++) {
		struct cgroup_controller *ctrlr = &conf->controllers[i];

		free(ctrlr->group);
		free(ctrlr->controller);
	}
	for(size_t i = 0; i < conf->num_params; i++) {
		struct cgroup_param *p = &conf->params[i];

		free(p->name);
		free(p->value);
	}

	free(conf->cgroup_prefix);
	free(conf->controllers);
	free(conf->params);

	free(conf);
}
/*****************************************************************************/