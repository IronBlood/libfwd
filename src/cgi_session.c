#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <fwd/cgi.h>
#include <fwd/cgi_session.h>
#include <fwd/cgi_sess_file.h>

#define SESSID_LENGTH 32

void session_load_var(struct s_session *session, struct s_session_var *var);
void session_load_vars(struct s_session *session);
void session_save_var(struct s_session *session, struct s_session_var *var);
void session_save_vars(struct s_session *session);
char *session_generate_id();


void session_register_var(struct s_cgi_context *ctx, char *name, void *var_ptr,
                          t_serializer serializer, t_deserializer deserializer)
{
	struct s_session *session = ctx->session;
	struct s_session_var *var, **last;

	if(!session) {
		session = ctx->session = session_create();
		session->cgi_ctx = ctx;
	}

	if(!session || !name || !var_ptr)
		return;
	
	last = &session->vars;
	var = session->vars;

	while(var) {
		last = &var->next;
		var = var->next;
	}

	*last = var = malloc(sizeof(struct s_session_var));
	memset(var, 0, sizeof(struct s_session_var));

	var->name = strdup(name);
	var->serialize = serializer;
	var->deserialize = deserializer;

	var->var = var_ptr;

	if(session->sessid)
		session_load_var(session, var);
}

struct s_session *session_create()
{
	struct s_session *session;

	session = malloc(sizeof(struct s_session));
	memset(session, 0, sizeof(struct s_session));

	session->handler = &file_handler;
	session->timeout = SESSION_TIMEOUT;

	return session;
}

void session_load_var(struct s_session *session, struct s_session_var *var)
{
	struct s_serialized *serialized;

	serialized = session->saved_vars;

	while(serialized) {
		if(!strcasecmp(serialized->name, var->name))
			break;
		serialized = serialized->next;
	}
	if(serialized)
		var->deserialize(serialized, var->var);
}

void session_load_vars(struct s_session *session)
{
	struct s_serialized serialized, *last, *next;
	struct s_session_var *var;

	if((last = session->saved_vars)) {   // 如果序列化链表已存在，则清空链表
		while(last) {
			next = last->next;

			if(last->data)
				free(last->data);

			if(last->name)
				free(last->name);

			free(last);

			last = next;
		}
	}

	if(session->handler)
		session->handler->stream_open(session);
	else
		return;

	if(!session->stream)
		return;

	memset(&serialized, 0, sizeof(struct s_serialized));
	last = NULL;

	while(session->handler->load_var(session, NULL, &serialized) > 0) {
		next = malloc(sizeof(struct s_serialized));
		memcpy(next, &serialized, sizeof(struct s_serialized));

		if(last)
			last->next = next;
		else
			session->saved_vars = next;

		last = next;
	}

	session->handler->stream_close(session);

	var = session->vars;

	while(var) {
		session_load_var(session, var);
		var = var->next;
	}
}

void session_save_var(struct s_session *session, struct s_session_var *var)
{
	struct s_serialized *serialized, *last = NULL;

	serialized = session->saved_vars;

	while(serialized) {
		if(!strcmp(serialized->name, var->name)) {
			if(serialized->data)
				free(serialized->data);

			serialized->size = 0;
			var->serialize(serialized, var->var);
			return;
		}
		last = serialized;
		serialized = serialized->next;
	}

	serialized = malloc(sizeof(struct s_serialized));
	memset(serialized, 0, sizeof(struct s_serialized));
	serialized->name = strdup(var->name);
	var->serialize(serialized, var->var);

	if(last)
		last->next = serialized;
	else
		session->saved_vars = serialized;
}

void session_save_vars(struct s_session *session)
{
	struct s_session_var *var;
	struct s_serialized *serialized;

	var = session->vars;

	if(session->handler) {
		session->handler->stream_open(session);
		session->handler->stream_clean(session);
	} else {
		return;
	}

	while(var) {
		session_save_var(session, var);
		var = var->next;
	}

	serialized = session->saved_vars;

	while(serialized) {
		session->handler->save_var(session, serialized->name, serialized);
		serialized = serialized->next;
	}

	session->handler->stream_close(session);
}

inline char *session_generate_id(char *sessid, int length)
{
	register int i;
	unsigned char c;

	sessid[length - 1] = '\0';

	for(i=0; i!=length; ++i) {
		do {
			c=(unsigned char)(random() % 255);
		} while(!isalnum(c));

		sessid[i] = c;
	}

	return sessid;
}

/** Start session handling.
 * Starts session handling. Loads variables from the storage.
 * 如果 ctx 中未包含 session id，则生成新的 session id。
 * 如果 ctx 中包含了 session id，则检查是否存在对应的 session 记录，若无，该 session 当做新的 session 对待。
 * \warning 如果 ctx 包含的 session 已经超时被删除，但是对应存在 session 记录（新生成的），如何处理？
 */
struct s_session *session_start(struct s_cgi_context *ctx)
{
	struct s_session *session = ctx->session;
	char *sessid = NULL;
	//int created = 0;
	//int new_sessid = 0;

	if(!session) {
		session = session_create();
		ctx->session = session;
		session->cgi_ctx = ctx;
		//created = 1;
	}

	// TODO: get sessid from ctx
	if(ctx) {
		struct s_http_param * param_session = param_get(ctx, "sessionid");
		if(param_session)
			sessid = param_session->value;
	}

	srandom(time(NULL));

	if(!sessid) {
		//new_sessid = 1;
		sessid = (char*)malloc(sizeof(char) * (SESSID_LENGTH+1));

		do {
			sessid = session_generate_id(sessid, SESSID_LENGTH+1);
			session->sessid = sessid;

			if(!session->handler)
				break;
		} while(session->handler->stream_exists(session));

		session->sessid = strdup(sessid);
		session->is_new = 1;
		free(sessid);  // TODO: check if is needed. 20121004 by ironblood
	} else {
		session->sessid = strdup(sessid);
		session->is_new = !session->handler->stream_exists(session);
	}

	session_load_vars(session);

	if(session->handler)
		if((long)(random() % 1000) <= (long)(1000 * GC_PROB)) // TODO: 这个触发条件是不是有点不靠谱？
			session->handler->collect_garbage(session);

	return session;
}

void session_timeout(struct s_cgi_context *ctx, long timeout)
{
	if(!ctx->session)
		ctx->session = session_create();

	ctx->session->timeout = timeout;
}

void session_destroy(struct s_cgi_context *ctx)
{
	struct s_session *session = ctx->session;

	if(session->handler)
		session->handler->stream_destroy(session);
}

void session_save(struct s_session *session)
{
	session_save_vars(session);
}

void session_free(struct s_session *session)
{
	struct s_session_var *var, *next;
	struct s_serialized *serialized, *s_next;

	var = session->vars;
	serialized = session->saved_vars;

	while(var) {
		next = var->next;

		if(var->name)
			free(var->name);

		free(var);

		var = next;
	}

	while(serialized) {
		if(serialized->name)
			free(serialized->name);

		if(serialized->data)
			free(serialized->data);

		s_next = serialized->next;
		free(serialized);
		serialized = s_next;
	}

	if(session->stream)
		if(session->handler)
			session->handler->stream_close(session);

	if(session->sessid)
		free(session->sessid);

	free(session);
}
