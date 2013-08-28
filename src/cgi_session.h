/* port from <cgi/cgi_session.h> in libcgic
 */

#ifndef CGI_SESSION_H_
#define CGI_SESSION_H_

#ifndef SESSION_TIMEOUT
#define SESSION_TIMEOUT 900
#endif

#ifndef GC_PROB
#define GC_PROB (1/50)
#endif

struct s_cgi_context;
struct s_session;
struct s_serialized;

/** Type serializer function
 * Serializer's responsibility is to convert var to s_serialized structure.
 * s_serialized structure is then written to the session storage using handler functions.
 */
typedef void (*t_serializer)(struct s_serialized *, void *var);

/** Type deserializer function
 * Deserializer's responsibility is to convert s_serialized structure to var (which could
 * be a pointer to any variable type). Your serializer will be responsible for converting
 * only one specified type.
 */
typedef void (*t_deserializer)(struct s_serialized *, void *var);

/** Handler for loading session data. */
typedef int (*session_media_load)	(struct s_session*, char *, struct s_serialized*);
/** Handler for saving session data. */
typedef int (*session_media_save)	(struct s_session*, char *, struct s_serialized*);
/** Handler for opening session storage. */
typedef int	(*session_media_open)	(struct s_session*);
/** Handler for closing session storage. */
typedef int	(*session_media_close)	(struct s_session*);
/** Handler for clearing (truncating) session storage. */
typedef int	(*session_media_clean)	(struct s_session*);
/** Handler for removing session storage. */
typedef int	(*session_media_destroy)(struct s_session*);
/** Handler for checking whether storage exists. */
typedef int	(*session_media_check)	(struct s_session*);
/** Handler for garbage collection (checking for outdated sessions and destroying them). */
typedef int	(*session_media_gc)		(struct s_session*);

/** Serialized variable for storing into session storage. */
struct s_serialized
{
	char *name; /**< Variable name. */
	size_t size; /**< Data length. */
	void *data; /**< Actual data. */
	struct s_serialized *next; /**< Pointer to the next element. */
};

struct s_session_var
{
	char *name;
	void *var;
	t_serializer serialize;
	t_deserializer deserialize;
	struct s_session_var *next;
};

/** Session handler structure.
 * sess_file.h contains handler one for storing sessions in files, but you can provide another
 * one for storing sessions i.e. in mysql database, and so on.
 * @see sess_file.h
 */
struct s_session_handler
{
	session_media_load		load_var; /**< Loads data from media. */
	session_media_save		save_var; /**< Saves data to media. */
	session_media_open		stream_open; /**< Opens/initializes media. */
	session_media_close		stream_close; /**< Closes/frees media. */
	session_media_clean		stream_clean; /**< Clears media. */
	session_media_destroy	stream_destroy; /**< Removes/destroys media. */
	session_media_check		stream_exists; /**< Checks whether given session exists. */
	session_media_gc		collect_garbage; /**< Clears outdated sessions. */
};

struct s_session
{
	void *stream;

	struct s_session_handler *handler;
	
	char *sessid;
	int is_new;
	struct s_serialized *saved_vars;
	struct s_session_var *vars;
	int error;
	long timeout;
	struct s_cgi_context *cgi_ctx;
};

void session_register_var(struct s_cgi_context *ctx, char *name, void *var_ptr,
                          t_serializer serializer, t_deserializer deserializer);
/** Shortcut for session_register_var function.
 * @param ctx pointer to existing CGI context (t_cgi_context *).
 * @param var variable.
 * @param serializer type serializer (function).
 * @param deserializer type deserializer (function).
 * @see session_register_var(struct s_cgi_context *, char *, void *, t_serializer, t_deserializer)
 */
#define SESSION_REGISTER(ctx, var, serializer, deserializer) \
        session_register_var(ctx, #var, &var, serializer, deserializer)

struct s_session *session_create(void);
struct s_session *session_start(struct s_cgi_context *ctx);
void session_destroy(struct s_cgi_context *ctx);
void session_timeout(struct s_cgi_context *ctx, long timeout);
void session_save(struct s_session *);
void session_free(struct s_session *);

#include "cgi_sess_stdtype.h"
#endif
