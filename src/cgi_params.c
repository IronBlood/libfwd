/*
 * cgi_params.c
 *
 *  Created on: 2012-10-19
 *      Author: IronBlood
 */

#include "cgi_params.h"
#include <string.h>

static struct s_http_param *param_create(char *name, char *value);
static void param_free(struct s_http_param *);

/** Initial ctx HTTP parameter list.
 * Initial ctx HTTP parameter list, only get parameter name and value from QUERY_STRING.
 * @param ctx s_cgi_context instance
 * @return pointer to s_param_list instance
 * @see void http_parm_init(void) in nju09/BBSLIB.c
 */
struct s_param_list *param_list_init(struct s_cgi_context *ctx)
{
	if(!ctx)
		return NULL;

	if(cgi_getenv(ctx, "QUERY_STRING")==NULL)
		return NULL;

	struct s_param_list *pl;
	char buf[1024], *t1, *t2;
	struct s_http_param *new_param;

	strncpy(buf, cgi_getenv(ctx, "QUERY_STRING"), 1024);

	pl = malloc(sizeof(struct s_param_list));
	if(pl == NULL)
		return NULL;

	memset(pl, 0, sizeof(struct s_param_list));
	pl->cgi_ctx = ctx;

	t1 = strtok(buf, "&");
	while(t1 != NULL) {
		t2 = strchr(t1, '=');
		if(t2 != 0) {
			t2[0] = '\0';
			t2++;
			new_param = param_create(t1, t2);
			if(new_param == NULL)
				continue;            // create fail
			if(pl->params == NULL)
				pl->params = new_param;
			else
				pl->params->next = new_param;
		}
		t1 = strtok(NULL, "&");
	}

	return pl;
}

static struct s_http_param *param_create(char *name, char *value)
{
	struct s_http_param *param;
	if(!name)
		return NULL;

	param = malloc(sizeof(struct s_http_param));
	if(!param)
		return NULL;

	memset(param, 0, sizeof(struct s_http_param));

	if(!(param->name = strdup(name))) {
		free(param);
		return NULL;
	}

	if(value)
		param->value = strdup(value);
	else
		param->value = strdup("");

	if(!(param->value)) {
		free(param->name);
		free(param);
		return NULL;
	}

	return param;
}

struct s_http_param *param_get(struct s_cgi_context *ctx, char *name)
{
	if(!ctx || !name)
		return NULL;

	struct s_http_param * param = ctx->params->params;
	while(param) {
		if(!strcasecmp(param->name, name))
			return param;

		param = param->next;
	}

	return NULL;
}

void param_list_free(struct s_param_list *pl)
{
	struct s_http_param *param, *next;
	param = pl->params;

	while(param) {
		next = param->next;
		param_free(param);
		param = next;
	}

	free(pl);
}

void param_free(struct s_http_param *param)
{
	if(!param)
		return;

	if(param->name)
		free(param->name);

	if(param->value)
		free(param->value);

	free(param);
}
