#ifndef CGI_FCGX_WRAPPER_H
#define CGI_FCGX_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcgiapp.h>

size_t FCGI_fwrite(const char *str, size_t size, size_t nmemb, FCGX_Stream *stream);

int FCGI_fprintf(FCGX_Stream *stream, const char *format, ...);

#endif
