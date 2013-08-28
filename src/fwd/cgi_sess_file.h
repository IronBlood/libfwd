#ifndef CGI_SESS_FILE_H
#define CGI_SESS_FILE_H

#include "cgi_session.h"

/* session file handlers */
int load_var_file(struct s_session *session, char *name, struct s_serialized *var);
int save_var_file(struct s_session *session, char *name, struct s_serialized *var);
int open_file(struct s_session *session);
int close_file(struct s_session *session);
int check_file(struct s_session *session);
int clean_file(struct s_session *session);
int destroy_file(struct s_session *session);
int clean_old_files(struct s_session *session);

/** Filled in file session handler. */
struct s_session_handler file_handler;

#endif