#ifndef CGI_OB_STREAM_H
#define CGI_OB_STREAM_H

#include <stdio.h>
#include "cgi.h"

#define BUFFER_GROW_THRESHOLD 1024

struct s_ob_stream {
	//FILE *file;
	char *buffer;
	unsigned long buffersize;
	unsigned long datasize;

	struct s_cgi_context *cgi_ctx;
};

struct s_ob_stream *ob_open(struct s_cgi_context *ctx);
ssize_t ob_stream_write(struct s_ob_stream *stream, const char *buffer, size_t size);
int ob_stream_free(struct s_ob_stream *stream);
void ob_end_flush(struct s_cgi_context *ctx);
void ob_end_discard(struct s_cgi_context *ctx);

#endif
