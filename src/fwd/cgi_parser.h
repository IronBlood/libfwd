#ifndef CGI_PARSER_H
#define CGI_PARSER_H

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define E_OK				0
#define E_NOMEM				1
#define E_CORRUPT			2
#define E_NODATA			4
#define E_NOTINITIALIZED	5
#define E_TOOBIG			5

struct PARSER_ERROR_INFO
{
	union {
		char *_corrupted;
		int _is_corrupted;
	} _corrupt_info;
	int _error;
};

void inline skip_chars(char **data, char *chars);
void inline skip_to(char **data, char *chars);
void inline skip_blanks(char **data);
void inline skip_to_blank(char **data);
int skip_word(char **data, char *work);
int get_word(char **dest, char **string);

#endif