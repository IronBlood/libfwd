#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fwd/cgi.h>
#include <fwd/cgi_headers.h>
#include <fwd/cgi_session.h>
#include <fwd/cgi_ob_stream.h>
#include <fwd/cgi_utils.h>

/** Create and initialize CGI context instance.
 * Create and initialize CGI context instance, only available in FastCGI mode.
 * Each CGI context has an output buffer, you can call ob_enable() to use it.
 * @param request A pointer to FCGI request.
 * @return Pointer to created CGI context (struct s_cgi_context *).
 * @see cgi_context_free(struct s_cgi_context *ctx)
 */
struct s_cgi_context *cgi_context_create(FCGX_Request *request)
{
	if(!request)
		return NULL;

	struct s_cgi_context *ctx;

	ctx = malloc(sizeof(struct s_cgi_context));

	if(!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct s_cgi_context));

	ctx->envp = request->envp;
	ctx->in   = request->in;
	ctx->__system_out = request->out;
	ctx->__error      = request->err;
	
	ctx->out = ob_open(ctx);
	ctx->params = param_list_init(ctx);

	ctx->headers = headers_create();
	ctx->headers->cgi_ctx = ctx;

	return ctx;
}

/** Write to CGI context output.
 * If output buffer is disabled, buffer passed in will be write to FCGI out stream directly.
 * @param ctx
 * @param buffer
 * @param size buffer length
 */
void cgi_context_write(struct s_cgi_context *ctx, const char *buffer, size_t size)
{
	if(!ctx)
		return;
	int os = 0;
	if(ctx->__ob_enabled)
		os = ob_stream_write(ctx->out, buffer, size);
	else
		os = FCGI_fprintf(ctx->__system_out, "%s", buffer);
	if(os<size) {
		FCGI_fprintf(ctx->__error, "cgi_context_write %s error! \r\n", (ctx->__ob_enabled == 1) ? "ob_stream_write" : "FCGI_fwrite");
	}
}

void cgi_context_free(struct s_cgi_context *ctx)
{
	if(!ctx)
		return;

	if(*(ctx->ob_content_length))
		ob_stream_free(ctx->out);


	if(ctx->headers)
		headers_free(ctx->headers);

	if(ctx->session) {
		if(ctx->session->vars && ctx->session->sessid)
			session_save(ctx->session);

		session_free(ctx->session);
	}

	ctx->in=NULL;
	ctx->__system_out=NULL;

	free(ctx);
}

char *cgi_getenv(struct s_cgi_context *ctx, char *name)
{
	return fast_get_env(ctx->envp, name);
}

char *cgi_getparamvalue(struct s_cgi_context *ctx, char *name)
{
	struct s_http_param *param = param_get(ctx, name);
	if(param)
		return param->value;
	return NULL;

}


void cgi_context_enable_buffer(struct s_cgi_context *ctx)
{
	ctx->__ob_enabled = 1;
}
