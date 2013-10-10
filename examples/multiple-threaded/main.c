#include <string.h>
#include <fwd/cgi.h>
#include <ght_hash_table.h>
#include <pthread.h>
#include "threadpool.h"

#define MAX_THREAD_COUNT 100
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
static int count = 0;

void helloworld_main(void *request);
void error_501_main(void *request);

/** CGI applet structure
 *  Hold applet name and function address.
 */
typedef struct cgi_applet_s {
	void (*main) (void *request);
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

	struct threadpool *pool;
	pool = threadpool_init(10);

	while(FCGX_Accept_r(&request) >= 0) {
		char * script_name = FCGX_GetParam("SCRIPT_NAME", request.envp);
		cgi_applet_t *cgi = get_cgi_applet(script_name);

		if(cgi!=NULL) {
			//(*(cgi->main))(&request);
			threadpool_add_task(pool, *(cgi->main), (void *)&request, 0);
		} else {
			//error_501_main(&request);
			threadpool_add_task(pool, error_501_main, (void *)&request, 0);
		}
		
		FCGX_Finish_r(&request);
	}

	return 0;
}

void helloworld_main(void *request) {
	struct s_cgi_context *ctx = cgi_context_create((FCGX_Request *)request);
	cgi_context_enable_buffer(ctx);

	header_add(ctx, "Content-type", "application/json; charset=utf-8");

	pthread_mutex_lock(&count_mutex);
	count++;
	t_cgi_tpl tpl = (t_cgi_tpl)malloc(80*sizeof(char));
	memset(tpl, 0, 80);
	snprintf((char *)tpl, 80, "{ \"message\" : \"hello world\", \"visit count\" : %d}", count);
	pthread_mutex_unlock(&count_mutex);

	template_out(tpl, ctx);

	cgi_context_free(ctx);

	free(tpl);
}

void error_501_main(void *request) {
	FCGX_Request *preq = (FCGX_Request *)request;
	FCGX_FPrintF(preq->out, "HTTP/1.1 501 Not Implemented\r\nContent-type: text/plain\r\n\r\nerror!");
}
