#define _GNU_SOURCE
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "cgi_headers.h"
#include "cgi_ob_stream.h"

ssize_t ob_stream_write(struct s_ob_stream *stream, const char *buffer, size_t size)
{
	struct s_ob_stream *s = stream;
	long written, start = 0;
	int left = s->buffersize - s->datasize;
	char *tmpbuf;

	if(size > left) {
		int added_size = BUFFER_GROW_THRESHOLD * ((size + 1 - left) / BUFFER_GROW_THRESHOLD + (((size + 1 - left) % BUFFER_GROW_THRESHOLD) ? 1 : 0));
		tmpbuf = (char *)realloc(s->buffer, (s->buffersize += added_size));

		if(!tmpbuf)
			return 0;

		s->buffer = tmpbuf;
	}

	if(size) {
		memcpy(s->buffer + s->datasize, buffer, size);
		*(s->buffer + s->datasize + size) = '\0';
	}

	s->datasize += size;

	/*if(s->cgi_ctx) {
		if(s->datasize && !s->cgi_ctx->__ob_enabled) {
			if(!s->cgi_ctx->headers->are_sent)
				headers_out(s->cgi_ctx);

			while(s->datasize) {
				written = FCGI_fwrite(s->buffer + start, 1, s->datasize, s->cgi_ctx->__system_out);
				s->datasize -= written;
				start += written;
			}
		}
	}*/

	return size;
}

int ob_stream_free(struct s_ob_stream *stream)
{
	struct s_ob_stream *s = stream;
	//long written, start = 0;

	if(s->cgi_ctx) {
		if(!s->cgi_ctx->headers->are_sent)
			headers_out(s->cgi_ctx);

		/*while(s->datasize) {
			written = FCGI_fwrite(s->buffer + start, 1, s->datasize, s->cgi_ctx->__system_out);
			s->datasize -= written;
			start += written;
		}*/
		FCGI_fprintf(s->cgi_ctx->__system_out, "%s", s->buffer);

		ob_end_discard(s->cgi_ctx);
	}

	if(s->buffer) {
		free(s->buffer);
		s->datasize = s->buffersize = 0;
	}

	free(stream);
	return 0;
}

// use fopencookie(3)
// see more: http://www.kernel.org/doc/man-pages/online/pages/man3/fopencookie.3.html
// also see: http://www.kernel.org/doc/man-pages/online/pages/man3/fmemopen.3.html
struct s_ob_stream *ob_create()
{
	struct s_ob_stream *s = malloc(sizeof(struct s_ob_stream));

	/*cookie_io_functions_t stream_func = {  // TODO: check fn pointer usage
		.read	= NULL,
		.write 	= ob_stream_write,
		.seek	= NULL,
		.close	= ob_stream_free
	};*/

	struct s_ob_stream init = {
		//.file 		= fopencookie(s, "rw", stream_func),
		.buffer 	= NULL,
		.buffersize = 0,
		.datasize	= 0,
		.cgi_ctx	= NULL
	};

	if(s)
		memcpy(s, &init, sizeof(struct s_ob_stream));

	return s;
}

struct s_ob_stream *ob_open(struct s_cgi_context *ctx)
{
	struct s_ob_stream *s = ob_create();

	s->cgi_ctx = ctx;
	ctx->ob_buffer = (void**)&s->buffer;
	ctx->ob_content_length = &s->datasize;
	//setvbuf(s->file, NULL, _IONBF, 0);

	return s;
}

void ob_end_flush(struct s_cgi_context *ctx)
{
	ctx->__ob_enabled = 0;
	//fwrite(*ctx->ob_buffer, 0, 0, ctx->out);
}

void ob_end_discard(struct s_cgi_context *ctx)
{
	*ctx->ob_content_length = 0;
	ctx->__ob_enabled = 0;
}
