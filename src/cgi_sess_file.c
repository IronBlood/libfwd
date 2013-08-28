#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/file.h>

#include "cgi_sess_file.h"

// TODO: change folder
#define SESSION_FILE "/tmp/sess_"

struct s_session_handler file_handler = {
	.load_var			= &load_var_file,
	.save_var			= &save_var_file,
	.stream_open		= &open_file,
	.stream_close		= &close_file,
	.stream_clean		= &clean_file,
	.stream_destroy 	= &destroy_file,
	.stream_exists		= &check_file,
	.collect_garbage 	= &clean_old_files
};

inline char *get_file_name(char *session_id)
{
	char *filename;
	long len;

	len = sizeof(SESSION_FILE) + strlen(session_id) - 1;

	if(!(filename = malloc(len + 1)))
		return NULL;

	*filename = '\0';

	strcat(filename, SESSION_FILE);
	strcat(filename, session_id);

	return filename;
}

int load_var_file(struct s_session *session, char *name, struct s_serialized *var)
{
	long len = 0, i = 0, match = 1;
	char sym, *read_name = NULL;

	if(!session)
		return -1;

	if(!session->stream)
		return -1;

	if(!var)
		return -1;

	var->data = NULL;
	var->size = 0;

	if(name)
		len = strlen(name) + 1;

	flock(fileno((FILE*)session->stream), LOCK_SH);

	if(name)
		fseek((FILE*)session->stream, 0, SEEK_SET);

READ_NAME:
	match = 1;
	i = 0;
	sym = 1;

	while(sym) {
		if(fread(&sym, 1, 1, (FILE*)session->stream) != 1) {
			goto BAIL_OUT;
		}

		if(name) {
			if(i<len && match)
				if(sym != name[i])
					match = 0;

			if(i>len && match)
				match = 0;
		}

		++i;  // get length of serialized session name
	}

	if(match) {
		read_name = malloc(i);
		fseek(session->stream, -i, SEEK_CUR);
		fread(read_name, 1, i, session->stream);
		var->name = read_name;
	}

READ_SIZE:
	sym = 1;
	i = 0;

	while(sym) {
		if(fread(&sym, 1, 1, (FILE*)session->stream) != 1) {
			goto BAIL_OUT;
		}

		++i; //get length of data size
	}

	if(i) {
		char *var_size_string = malloc(i);
		fseek(session->stream, -i, SEEK_CUR);
		fread(var_size_string, 1, i, session->stream);
		var->size = atoi(var_size_string); // int
		free(var_size_string);
	}

	//if(fread(&var->size, sizeof(var->size), 1, (FILE*)session->stream) != 1) {
	//	goto BAIL_OUT;
	//}

	if(!match) {
		fseek(session->stream, var->size, SEEK_CUR);
		goto READ_NAME;
	}

	len += sizeof(var->size);

	var->data = malloc(var->size);

	if((i=fread(var->data, var->size, 1, (FILE*)session->stream)) != 1) {
		goto BAIL_OUT;
	}

	flock(fileno((FILE*)session->stream), LOCK_UN);

	len += var->size;

	return 1;

BAIL_OUT:
	if(var->data)
		free(var->data);

	if(read_name)
		free(read_name);

	var->data = NULL;

	flock(fileno((FILE*)session->stream), LOCK_UN);

	return 0;
}

int save_var_file(struct s_session *session, char *name, struct s_serialized *var)
{
	long len, fpos;

	if(!session)
		return -1;

	if(!session->stream)
		return -1;

	if(var->size && !var->data) /* Corrupted */
		return -1;

	flock(fileno((FILE*)session->stream), LOCK_EX);

	fseek((FILE*)session->stream, 0, SEEK_END);

	fpos = ftell((FILE*)session->stream);

	len = strlen(name) + 1;

	if(fwrite(name, 1, len, (FILE*)session->stream) != len) {
		goto BAIL_OUT;
	}

	// 向 session 文件中写入数据长度
	len += sizeof(var->size) + 1;
	char * var_size_string = malloc(sizeof(var->size) + 1);
	sprintf(var_size_string, "%zd", var->size);
	if(fwrite(var_size_string, 1, strlen(var_size_string) + 1, (FILE*)session->stream) != (strlen(var_size_string)+1) ) {
		free(var_size_string);
		goto BAIL_OUT;
	}
	free(var_size_string);

	len += var->size;

	if(var->data) {
		if(fwrite(var->data, var->size, 1, (FILE*)session->stream) != 1) {
			goto BAIL_OUT;
		}
	}

	flock(fileno((FILE*)session->stream), LOCK_UN);

	return len;

BAIL_OUT:
	fseek((FILE*)session->stream, fpos, SEEK_SET);
	ftruncate(fileno((FILE*)session->stream), fpos);
	flock(fileno((FILE*)session->stream), LOCK_UN);
	return -1;
}

int open_file(struct s_session *session)
{
	char *filename;

	if(!session)
		return -1;

	if(!session->sessid)
		return -1;

	if(!(filename = get_file_name(session->sessid)))
		return -1;

	session->error = 0;

	if(!session->stream)
		session->stream = fopen(filename, "a+b");

	free(filename);

	if(!session->stream) {
		session->error = errno;
		return -1;
	}

	fseek((FILE*)session->stream, 0, SEEK_SET);

	return 0;
}

int close_file(struct s_session *session)
{
	if(!session->stream)
		return -1;

	fclose((FILE*)session->stream);

	session->stream = NULL;

	return 0;
}

int check_file(struct s_session *session)
{
	char *filename;
	struct stat sb;
	time_t maxtime;

	maxtime = time(NULL) - session->timeout;

	if(!session)
		return -1;

	if(!session->sessid)
		return -1;

	if(!(filename = get_file_name(session->sessid)))
		return -1;

	if(stat(filename, &sb) == -1) {
		free(filename);

		if(errno == ENOENT) 
			return 0;

		return -1;
	}

	if(sb.st_atime < maxtime && sb.st_mtime < maxtime) { // 判断文件的访问时间和修改时间
		unlink(filename);
		free(filename);
		return 0;
	}

	free(filename);

	return 1; // session 文件依旧有效
}

int destroy_file(struct s_session *session)
{
	char *filename;

	if(!session)
		return -1;

	if(!session->sessid)
		return -1;

	if(!(filename = get_file_name(session->sessid)))
		return -1;

	unlink(filename);

	free(filename);

	return 0;
}

int clean_file(struct s_session *session)
{
	char *filename;

	if(!session)
		return -1;

	if(!session->sessid)
		return -1;

	if(session->stream) {
		if(ftruncate(fileno((FILE*)session->stream), 0) == -1)
			return -1;
		return 0;
	}

	if(!(filename = get_file_name(session->sessid)))
		return -1;

	if(truncate(filename, 0) == -1) {
		free(filename);
		return -1;
	}

	free(filename);
	return 0;
}

int clean_old_files(struct s_session *session)
{
	char *filename = NULL;
	DIR  *dir = NULL;
	struct dirent de, *res;
	struct stat sb;
	time_t maxtime;
	char *dirname = NULL;
	char *filepart = NULL;
	long len;

	dirname = malloc(sizeof(SESSION_FILE));

	if(!dirname)
		goto BAIL_OUT;

	memcpy(dirname, SESSION_FILE, sizeof(SESSION_FILE));

	filepart = dirname + sizeof(SESSION_FILE) - 1;

	while(*filename != '/')
		filepart --;
	*filename = '\0';

	filepart++;

	len = sizeof(SESSION_FILE) - (filepart - dirname);

	maxtime = time(NULL) - session->timeout;

	if(!(dir = opendir(dirname))) {
		free(dirname);
		return -1;
	}

	while(readdir_r(dir, &de, &res) == 0) {
		if(!res)
			break;

		if(strlen(res->d_name) > len-1) {
			if(!strncmp(filepart, res->d_name, len-1)) {
				if(asprintf(&filename, "/tmp/%s", res->d_name) < 0) {
					goto BAIL_OUT;
				}

				stat(filename, &sb);

				if(sb.st_atime < maxtime && sb.st_mtime < maxtime)
					unlink(filename);

				free(filename);
			}
		}
	}

	closedir(dir);
	free(dirname);

	return 0;

BAIL_OUT:
	if(dirname)
		free(dirname);
	if(filename)
		free(filename);
	if(dir)
		closedir(dir);
	return -1;
}
