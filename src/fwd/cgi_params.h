#ifndef CGI_PARAMS_H
#define CGI_PARAMS_H

#include "cgi.h"

/** HTTP parameter structure
 * This structure contains HTTP parameter items.
 * It holds parameter name, parameter value, and a pointer to the next item.
 */
struct s_http_param
{
	char *name; /**< parameter name */
	char *value; /**< parameter value */
	struct s_http_param *next; /**< pointer to the next parameter item */
};

struct s_param_list
{
	struct s_http_param *params;
	struct s_cgi_context *cgi_ctx;
};

struct s_param_list *param_list_init(struct s_cgi_context *);
struct s_http_param *param_get(struct s_cgi_context *, char *);
void param_list_free(struct s_param_list *);

#endif
