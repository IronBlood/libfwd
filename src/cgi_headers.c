/*
 * cgi_headers.c
 *
 *  Created on: 2012-9-27
 *      Author: IronBlood
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fwd/cgi.h>

#define HDR_SIZE sizeof(struct s_http_header)

static struct s_http_header *header_create(char *name, char *value);
static void header_free(struct s_http_header *hdr);

static struct s_http_header *header_create(char *name, char *value)
{
	struct s_http_header *hdr;

	if(!name)
		return NULL;

	hdr = malloc(HDR_SIZE);

	if(!hdr)
		return NULL;

	memset(hdr, 0, HDR_SIZE);

	if(!(hdr->name = strdup(name))) {
		free(hdr);
		return NULL;
	}

	if(value)
		hdr->value = strdup(value);
	else
		hdr->value = strdup("");

	if(!(hdr->value)) {
		free(hdr->name);
		free(hdr);
		return NULL;
	}

	hdr->next = 0; // important: initial the next pointer

	return hdr;
}

static void header_free(struct s_http_header *hdr)
{
	if(!hdr)
		return;

	if(hdr->name)
		free(hdr->name);

	if(hdr->value)
		free(hdr->value);

	free(hdr);
}

/** Add new header with given value.
 * Add new header whith given value. Header is added even if header with the
 * same name already exist.
 * @param ctx pointer to existing CGI context (struct s_cgi_context *).
 * @param name header name.
 * @param value heaer value.
 * @return pointer to the newly created struct s_http_header.
 * @see cgi_context_create(FCGX_Request *)
 * @see cgi_context_free(t_cgi_context *)
 * @see header_set(t_cgi_context *, char *, char *)
 * @see header_get(t_cgi_context *, char *)
 */
struct s_http_header *header_add(struct s_cgi_context *ctx, char *name, char *value)
{
	struct s_headers *headers = ctx->headers;
	struct s_http_header *hdr = headers->headers;

	if(!hdr) {
		headers->headers = header_create(name, value);
		return headers->headers;
	}

	while(hdr->next)
		hdr = hdr->next;

	hdr->next = header_create(name, value);
	return hdr->next;
}

/** Set the header value.
 * Sets value of header. If header already exists, overwrites it. Otherwise add new header.
 * @param ctx pointer to existing CGI context (struct s_cgi_context *).
 * @param name header name.
 * @param value heaer value.
 * @return pointer to struct s_http_header which value has been set.
 * @see cgi_context_create(FCGX_Request *)
 * @see cgi_context_free(t_cgi_context *)
 * @see header_add(t_cgi_context *, char *, char *)
 * @see header_get(t_cgi_context *, char *)
 */
struct s_http_header *header_set(struct s_cgi_context *ctx, char *name, char *value)
{
	struct s_headers *headers = ctx->headers;
	struct s_http_header *hdr, *created;
	struct s_http_header **hdr_prev = &headers->headers; // old version of hdr

	hdr = headers->headers;

	// TODO: unuseful?
	//if(!hdr) {
	//	headers->headers = header_create(name, value);
	//	return headers->headers;
	//}

	while(hdr) {
		if(!strcasecmp((*hdr_prev)->name, name)) {
			if(!(created = header_create(name, value))) {
				return NULL;
			}

			created->next = (*hdr_prev)->next;
			header_free(*hdr_prev);
			*hdr_prev = created;
			return created;
		}
		hdr_prev = &((*hdr_prev)->next);
		hdr = hdr->next;
	}

	return *hdr_prev = header_create(name, value);
}

/** Get header.
 * Returns header structure.
 * @param ctx pointer to existing CGI context (struct s_cgi_context *).
 * @param name header name.
 * @return pointer to struct s_http_header.
 * @see cgi_context_create(FCGX_Request *)
 * @see cgi_context_free(t_cgi_context *)
 * @see header_add(t_cgi_context *, char *, char *)
 * @see header_set(t_cgi_context *, char *)
 */
struct s_http_header *header_get(struct s_cgi_context *ctx, char *name)
{
	struct s_headers *headers = ctx->headers;
	struct s_http_header *hdr = headers->headers;

	if(!hdr)
		return NULL;

	while(hdr) { // TODO: check whether hdr or hdr->next
		if(!strcasecmp(hdr->name, name))
			return hdr;

		hdr=hdr->next;
	}

	return NULL;
}

/** Force output of headers.
 * Forces output of headers, even when they are already sent.
 * Normally, they are sent automatically if you use libcgic streams library.
 * @param ctx pointer to existing CGI context (struct s_cgi_context *).
 * @see cgi_context_create(FCGX_Request *)
 * @see cgi_context_free(t_cgi_context *)
 * @see header_set(t_cgi_context *, char *, char *)
 * @see header_add(t_cgi_context *, char *, char *)
 * @see header_get(t_cgi_context *, char *)
 */
void headers_out(struct s_cgi_context *ctx)
{
	struct s_headers *headers = ctx->headers;
	struct s_http_header *hdr = headers->headers;

	while(hdr) { // TODO: if hdr==null?
		FCGI_fprintf(headers->cgi_ctx->__system_out, "%s: %s\r\n", hdr->name, hdr->value);
		hdr=hdr->next;
	}

	FCGI_fprintf(headers->cgi_ctx->__system_out, "\r\n");
	headers->are_sent = 1;
}

struct s_headers *headers_create()
{
	struct s_headers *headers;

	headers = malloc(sizeof(struct s_headers));

	if(headers)
		memset(headers, 0, sizeof(struct s_headers));

	return headers;
}

void headers_free(struct s_headers *headers)
{
	struct s_http_header *hdr, *nxt;

	hdr = headers->headers;

	while(hdr) {
		nxt = hdr->next;
		header_free(hdr);
		hdr = nxt;
	}

	free(headers);
}
