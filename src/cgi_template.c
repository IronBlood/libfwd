#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <json/json.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cgi.h"

char *template_string_replace(char *ori, const char *old, const char *new);

/** Create template from file
 * Create a new instance of  template from file.
 * @param filename file to parse.
 * @return parsed template on success, or NULL if failed.
 * @see template_set(t_cgi_tpl *tpl, const char *key, char *fmt, ...)
 * @see template_out(t_cgi_tpl tpl, struct s_cgi_context *ctx)
 * @see template_free(t_cgi_tpl tpl)
 */
t_cgi_tpl template_create(char *filename)
{
	char *p, *s;
	int fd;
	struct stat statbuf;

	fd = open(filename, O_RDONLY);  
	if(fd == -1) {
		return NULL; // cannot open
	}

	if(fstat(fd, &statbuf) == -1) {
		close(fd);
		return NULL; // fstat error
	}

	if(!S_ISREG(statbuf.st_mode)) {
		close(fd);
		return NULL; // not a file
	}

	p = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);

	if(p == MAP_FAILED) {
		return NULL;
	}

	s = strdup(p);

	munmap(p, statbuf.st_size);
	return s;
}

/** Set value to template
 * Set value to value placeholder in template.
 * For example, there is an string { "test":"<% test %>" } in template tpl, you
 * can just use template_set(tpl, "test", "%s %s", "hello", "bmy") to set the value
 * of "test" to "hello bmy".
 * @param tpl template instance
 * @param key the name of value placeholder
 * @param fmt value format
 */
void template_set(t_cgi_tpl *tpl, const char *key,  char *fmt, ...)
{
	if(!tpl)
		return;

	va_list v;
	char *new_string, *old_string;
	
	va_start(v, fmt);
	vasprintf(&new_string, fmt, v);
	va_end(v);

	// old_string = "<% key %>"
	old_string = malloc(strlen(key) + 7);
	sprintf(old_string, "<%% %s %%>", key);

	*tpl = template_string_replace(*tpl, old_string, new_string);

	free(new_string);
	free(old_string);
}

char *template_string_replace(char *ori, const char *old, const char *new)
{
	int tmp_string_length = strlen(ori) + strlen(new) - strlen(old) + 1;
	
	char *ch;
	ch = strstr(ori, old);

	if(!ch)
		return ori;

	char *tmp_string = (char *)malloc(tmp_string_length);
	if(tmp_string == NULL) return ori;

	memset(tmp_string, 0, tmp_string_length);
	strncpy(tmp_string, ori, ch - ori);
	*(tmp_string + (ch - ori)) = 0;
	sprintf(tmp_string + (ch - ori), "%s%s", new, ch+strlen(old));
	*(tmp_string + tmp_string_length) = 0;

	free(ori);
	ori = tmp_string;

	return ori;
}

/** Output template to cgi context
 * Output template to cgi context as string.
 * @param tpl template
 * @param ctx cgi context
 */
void template_out(t_cgi_tpl tpl, struct s_cgi_context *ctx)
{
	if(!tpl)
		return;

	if(!ctx)
		return;

	cgi_context_write(ctx, tpl, strlen(tpl));
}

/** free template
 *
 * @param tpl
 */
void template_free(t_cgi_tpl tpl)
{
	free(tpl);
}
