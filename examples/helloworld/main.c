#include <string.h>
#include <fwd/cgi.h>
#include <ght_hash_table.h>

int helloworld_main(FCGX_Request *request);
int error_501_main(FCGX_Request *request);

/** CGI applet structure
 *  Hold applet name and function address.
 */
typedef struct cgi_applet_s {
	int (*main) (FCGX_Request *request);
	char *name;
} cgi_applet_t;

cgi_applet_t applets[] = {
	{ helloworld_main, "/helloworld.json" },
	{ NULL, NULL },
};

cgi_applet_t * get_cgi_applet(char *needcgi) {
	static ght_hash_table_t *p_table = NULL;
	cgi_applet_t *a;

	if(p_table == NULL) {
		a = applets;
		p_table = ght_create(250, NULL, 0);
		while (a->main != NULL) {
			ght_insert(p_table, (void *)a, strlen(a->name), (void *)a->name);
			a++;
		}
	}

	if(p_table == NULL)
		return NULL;
	else
		return ght_get(p_table, strlen(needcgi), needcgi);
}

int main(int argc, char *argv[]) {
	FCGX_Init();
	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);

	while(FCGX_Accept_r(&request) >= 0) {
		char * script_name = FCGX_GetParam("SCRIPT_NAME", request.envp);
		cgi_applet_t *cgi = get_cgi_applet(script_name);

		if(cgi!=NULL) {
			(*(cgi->main))(&request);
		} else {
			error_501_main(&request);
		}
		
		FCGX_Finish_r(&request);
	}

	return 0;
}

int helloworld_main(FCGX_Request *request) {
	struct s_cgi_context *ctx = cgi_context_create(request);
	cgi_context_enable_buffer(ctx);

	header_add(ctx, "Content-type", "application/json; charset=utf-8");

	t_cgi_tpl tpl = "{ \"message\" : \"hello world\" }";

	template_out(tpl, ctx);

	cgi_context_free(ctx);
	return 0;
}

int error_501_main(FCGX_Request *request) {
	FCGX_FPrintF(request->out, "HTTP/1.1 501 Not Implemented\r\nContent-type: text/plain\r\n\r\nerror!");
	return -1;
}
