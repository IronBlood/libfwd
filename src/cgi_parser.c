#include <stdio.h>
#include <string.h>
#include <fwd/cgi_parser.h>

void skip_chars(char **data, char *chars)
{
	int flag = 1, i;
	int length = strlen(chars);

	while(flag && (**data != 0)) {
		flag = 0;
		for(i=0; i<length; ++i) {
			flag += (**data == chars[i]);
			if(flag) {
				(*data)++;
				break;
			}
		}
	}
}

void skip_to(char **data, char *chars)
{
	int flag = 1, i;
	int length = strlen(chars);

	while(flag && (**data != 0)) {
		flag = 0;
		for(i=0; i<length; ++i) {
			flag |= (**data != chars[i]);
			if(flag) {
				(*data)++;
				break;
			}
		}
	}
}

void skip_blanks(char **data)
{
	skip_chars(data, " \t");
}

void skip_to_blank(char **data)
{
	skip_to(data, " \t");
}

int skip_word(char **data, char *word)
{
	int minsize = MIN(strlen(*data), strlen(word));

	if(!strncasecmp(*data, word, minsize) && (minsize == strlen(word))) {
		*data += minsize;
		return 1;
	}

	return 0;
}

int get_word(char **dest, char **string)
{
	int quoted = 0, word_quoted = 0;
	char *first, tmp;
	char quote = '"';

	first = *string;

	while((**string != 0) && !(!quoted && strchr(" \t\r\n;", **string))) {
		word_quoted |= quoted;

		if(!quoted && (quote == **string))
			quoted = 1;
		else if (quoted && (quote == **string))
			quoted = 0;

		(*string)++;
	}

	if(word_quoted) {
		first++;
		if(quote == *(*string -1)) {
			tmp = *(*string -1);
			*(*string - 1) = 0;

			if(asprintf(dest, "%s", first) < 0) {
				*(*string - 1) = tmp;
				return 0;
			}

			*(*string - 1) = tmp;
			return 1;
		}
	}

	tmp = **string;
	**string = 0;

	if(asprintf(dest, "%s", first) < 0) {
		**string = tmp;
		return 0;
	}

	**string = tmp;

	return 1;
}