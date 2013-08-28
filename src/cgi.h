#ifndef CGI_H_
#define CGI_H_

#include <stdio.h>
#include "cgi_headers.h"
#include "cgi_params.h"
#include "cgi_session.h"
#include "cgi_template.h"
#include "cgi_fcgx_wrapper.h"

#include <fcgiapp.h>

/** CGI context structure
 * Hold all context-dependent stuff, such as: output stream, session.
 */
struct s_cgi_context {
	struct s_session *session;			/**< HTTP session */
	struct s_headers *headers;			/**< HTTP headers */
	struct s_param_list *params;		/**< HTTP parameters */
	char **envp; 						/**< FastCGI environ */
	FCGX_Stream *in;    				/**< FastCGI In Stream */
	struct s_ob_stream *out;   			/**< Output stream. Use it as FILE* handle to output data to client. */
	FCGX_Stream *__system_out; 			/**< FastCGI out Stream */
	FCGX_Stream *__error;      			/**< FastCGI error Stream */
	int __ob_enabled;
	void **ob_buffer; 					/**< Pointer to variable containing output buffer. */
	unsigned long *ob_content_length; 	/**< Pointer to variable containing output buffer length. */
};


struct s_cgi_context *cgi_context_create(FCGX_Request *);
void cgi_context_write(struct s_cgi_context *, const char *, size_t);
void cgi_context_enable_buffer(struct s_cgi_context *);
void cgi_context_free(struct s_cgi_context *);
char *cgi_getenv(struct s_cgi_context *, char *);
char *cgi_getparamvalue(struct s_cgi_context *, char *);

#endif
