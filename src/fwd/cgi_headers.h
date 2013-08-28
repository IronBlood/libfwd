/**
 * \file cgi_headers.h
 * \brief HTTP headers handling.
 * \author Vladimir Pavluk, 2006-2007
 *
 * HTTP headers handling functions. Ported from <cgi/cgi_headers.h> in libcgic by IronBlood.
 */


#ifndef CGI_HEADERS_H_
#define CGI_HEADERS_H_

struct s_cgi_context;

/** HTTP header structure
 * This structure contains HTTP headers list element.
 * It holds header name, its value, and a pointer to the next element.
 */
struct s_http_header
{
	char *name; /**< Header name. */
	char *value; /**< Header value. */
	struct s_http_header *next; /**< Pointer to the next HTTP header item. */
};

struct s_headers
{
	struct s_http_header *headers;
	struct s_cgi_context *cgi_ctx;

	int are_sent;
};

struct s_http_header *header_add(struct s_cgi_context *ctx, char *name, char *value);
struct s_http_header *header_set(struct s_cgi_context *ctx, char *name, char *value);
struct s_http_header *header_get(struct s_cgi_context *ctx, char *name);
void headers_out(struct s_cgi_context *ctx);

struct s_headers *headers_create(void);
void headers_free(struct s_headers *headers);

#endif
