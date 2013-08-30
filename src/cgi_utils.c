#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fwd/cgi_utils.h>

char *url_encode(const char *s, int length, int *new_length)
{
	register unsigned char c;
	unsigned char *to, *start;
	unsigned char const *from, *end;
	unsigned char const hexchars[] = "0123456789ABCDEF";

	from = s;
	end = s + length;
	start = to = (unsigned char *)malloc(3*length + 1);

	while(from < end) {
		c = *from++;

		if( (c<'0' && c!='-' && c!='.') ||
			(c<'A' && c>'9') ||
			(c>'Z' && c<'a' && c!='_') ||
			(c>'z')) {
			to[0] = '%';
			to[1] = hexchars[c>>4];
			to[2] = hexchars[c&15];
			to += 3;
		} else {
			*to++ = c;
		}
	}

	*to = 0;

	if(new_length) 
		*new_length = to - start;

	return (char*)start; // remember to free
}

char *url_decode(const char *string, int *new_length) // TODO: check function
{
	int charcode = 0, cipher_num = 0;
	const char *str = string;
	char *newstr, *newstring;

	newstring = newstr = strdup(string);

	while(*str) {
		switch(*str) {
		case '+':
			*newstr++ = ' ';
			break;
		case '%':
			cipher_num++;
			charcode = 0;
			break;
		default:
			if (cipher_num) {
				charcode <<=4;
				if((*str>='0') && (*str<=9)) {
					charcode += *str - '0';
				} else if((*str>='a') && (*str<='f')) {
					charcode += 10 + *str - 'a';
				} else if((*str>='A') && (*str<='F')) {
					charcode += 10 + *str - 'A';
				} else {
					// Erroneous hexadecimal cipher
					free(newstring);
					return NULL;
				}
				
				cipher_num++;
				if (cipher_num > 2) {
					cipher_num = 0;
					*newstring++ = (char)charcode;
				}
			}
			break;
		}

		*newstr++ = *str;
		str ++;
	}

	*newstr = 0;
	*new_length = newstr - newstring;
	return newstring; // remember to free
}

char *fast_get_env(register char **envp, char *name)
{
	char *m, *n;

	while((m=*envp) != NULL) {
		n = name;

		while(*n && *n == *m) {
			m++;
			n++;
		}

		if(!*n)
			return ++m;
		envp++;
	}
	return NULL;
}
