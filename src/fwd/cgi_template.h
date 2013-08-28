/** 
 * \file cgi_template.h
 * \brief Template handling functions.
 * \author IronBlood
 */

#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include <stdio.h>

typedef char * t_cgi_tpl;

t_cgi_tpl template_create(char *filename);
void template_set(t_cgi_tpl *tpl, const char *key, char *fmt, ...);
void template_out(t_cgi_tpl tpl, struct s_cgi_context *ctx);
void template_free(t_cgi_tpl tpl);

#endif
