#ifndef CGI_UTILS_H
#define CGI_UTILS_H

char *url_encode(const char *, int, int *);
char *url_decode(const char *, int *);
char *fast_get_env(char **, char *);

#endif