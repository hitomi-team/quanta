#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "snprintf.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ftw.h>
#include <unistd.h>
#endif

#include "serialize.h"
#include "pakar.h"

static const char *help_msg =
	"==== kokonu / kuze.genkei PAKAR utility ====\n"
	"Version 0.1\n"
	"\n"
	"usage: pakar [dir]\n"
	"       pakar [index.pak]\n"
	"       pakar add [index.pak] [file1] [file2] ...\n"
	"       pakar list [index.pak]\n"
	"       pakar extract [index.pak] [file1] [file2] ...\n"
	"       pakar rehash [index.pak]\n"
	"       pakar validate [index.pak]\n"
	"\n"
	"=== create / add / extract files ===\n"
	"pakar [dir]                                   - create a pak out of a dir\n"
	"pakar add [index.pak] [file1] [file2] ...     - add files\n"
	"pakar [index.pak]                             - extract all files\n"
	"pakar extract [index.pak] [file1] [file2] ... - extract individual files\n"
	"\n"
	"=== info ===\n"
	"pakar list [index.pak]                        - prints a list of details about the pak\n"
	"\n"
	"=== integrity ===\n"
	"pakar rehash [index.pak]                      - rehash the pak if it is modified\n"
	"pakar validate [index.pak]                    - validate the pak and it's signature if any\n"
;

void oom()
{
	fprintf(stderr, "!!! OUT OF MEMORY / MEMORY ERROR !!!\n");
}

#ifdef _WIN32
int conv_utf8_to_wchr(WCHAR **dst, const char *src)
{
	WCHAR *wstr;
	int len;

	if ((len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0)) <= 0)
		return -1;

	if ((wstr = malloc((len + 1)*sizeof(WCHAR))) == NULL) {
		oom();
		return -2;
	}

	wstr[len] = L'\0';

	MultiByteToWideChar(CP_UTF8, 0, src, -1, wstr, len);
	(*dst) = wstr;

	return 0;
}

int conv_wchr_to_utf8(char **dst, const WCHAR *src)
{
	char *str;
	int len;

	if ((len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL)) <= 0)
		return -1;

	if ((str = malloc(len + 1)) == NULL) {
		oom();
		return -2;
	}

	str[len] = '\0';

	WideCharToMultiByte(CP_UTF8, 0, src, -1, str, len, NULL, NULL);
	(*dst) = str;

	return 0;
}

static inline void conv_filetime_to_u64_ms(const FILETIME *ft, uint64_t *out)
{
	uint64_t tmp;

	tmp = (int64_t)ft->dwHighDateTime << 32;
	tmp |= ft->dwLowDateTime;
	tmp -= 116444736000000000LL;
	tmp /= 10000000LL;

	(*out) = tmp;
}

int change_dir(const char *str)
{
	WCHAR *wstr;
	BOOL b;

	if (conv_utf8_to_wchr(&wstr, str) != 0)
		return -2;

	b = SetCurrentDirectoryW(wstr);
	free(wstr);

	return b == TRUE ? 0 : -1;
}
#else

static inline void conv_timespec_to_u64_ms(const struct timespec *ts, uint64_t *out)
{
	(*out) = (uint64_t)ts->tv_sec + (uint64_t)ts->tv_nsec/1000000000ULL;
}

#define change_dir chdir

#endif

static inline int is_directory(const char *path)
{
#ifdef _WIN32
	WCHAR *wpath;
	DWORD attribs;

	if (conv_utf8_to_wchr(&wpath, path) != 0)
		return -1;

	attribs = GetFileAttributesW(wpath);
	free(wpath);

	return attribs & FILE_ATTRIBUTE_DIRECTORY;
#else
	struct stat s;
	stat(path, &s);
	return S_ISDIR(s.st_mode);
#endif
}

static inline int is_file(const char *path)
{
#ifdef _WIN32
	WCHAR *wpath;
	DWORD attribs;

	if (conv_utf8_to_wchr(&wpath, path) != 0)
		return -1;

	attribs = GetFileAttributesW(wpath);
	free(wpath);

	return (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
	struct stat s;
	stat(path, &s);
	return S_ISREG(s.st_mode);
#endif
}

/* no better way to do this lol */
enum {
	FILE_INFO_DIR,
	FILE_INFO_REG
};

struct file_info {
	char *path;
	uint64_t modified_time;
	uint64_t size;
	int type;

	struct file_info *next;
};

struct file_entry {
	struct pakar_index_entry entry;
	char *path;
	struct file_entry *next;
};

static struct file_info *fi_head = NULL;
static uint64_t fi_total_size = 0;
static uint32_t num_fi = 0;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static const char *blacklisted_exts[] = {
	".zip",
	".rar",
	".exe",
	".dll",
	".bat",
	".cmd",
	".tar",
	".pak",
	".com",
	".7z",
};

static const size_t num_blacklisted_exts = ARRAY_SIZE(blacklisted_exts);

const char *get_filename_ext(const char *filename)
{
	const char *dot = strrchr(filename, '.');

	if (dot == NULL || dot == filename)
		return NULL;

	return dot + 1;
}

void file_info_add(const char *path, uint64_t modified_time, uint64_t size, int type)
{
	struct file_info *fi;
	const char *ext;
	size_t i;

	if (type == FILE_INFO_DIR)
		goto create_entry;

	if ((ext = get_filename_ext(path)) == NULL)
		goto create_entry;

	for (i = 0; i < num_blacklisted_exts; i++) {
		if (strcmp(ext, blacklisted_exts[i]) == 0)
			return;
	}

create_entry:
	fi = malloc(sizeof(struct file_info));

	fi->type = type;
	fi->modified_time = modified_time;
	fi->size = size;
	fi->path = malloc(strlen(path) + 1);
	strcpy(fi->path, path);

	fi->next = fi_head;
	fi_head = fi;

	fi_total_size += size;

	num_fi++;
}

void file_info_cleanup()
{
	struct file_info *fi;

	while (fi_head != NULL) {
		fi = fi_head->next;
		free(fi_head->path);
		free(fi_head);
		fi_head = fi;
	}
}

#ifndef _WIN32
int nftw_fn(const char *path, const struct stat *info, const int type_flag, struct FTW *path_info)
{
	uint64_t time;
	int type;

	(void)path_info;

	if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0)
		return 0;

	if (type_flag != FTW_F && type_flag != FTW_DP)
		return 0;

	type = type_flag == FTW_F ? FILE_INFO_REG : FILE_INFO_DIR;
	conv_timespec_to_u64_ms(&info->st_mtim, &time);

	file_info_add(path, time, info->st_size, type);

	return 0;
}
#endif

int dir_tree_walk_for_files(const char *dir_path)
{
#ifdef _WIN32
	/* why... */
	WIN32_FIND_DATAW find_data;
	HANDLE handle;
	WCHAR *wdir_path;
	char *utf8_path, *tmp_path;
	uint64_t size, time;
	int type;

	if ((utf8_path = malloc(strlen(dir_path) + 4)) == NULL) {
		oom();
		return -2;
	}

	strcpy(utf8_path, dir_path);
	strcat(utf8_path, "\\*.*");

	if (conv_utf8_to_wchr(&wdir_path, utf8_path) != 0)
		return -1;

	free(utf8_path);

	if ((handle = FindFirstFileW(wdir_path, &find_data)) == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "failed to init dir walk: %s\n", dir_path);
		free(wdir_path);
		return -1;
	}

	do {
		if (conv_wchr_to_utf8(&utf8_path, find_data.cFileName) != 0) {
			FindClose(handle);
			return -2;
		}

		if (strcmp(utf8_path, ".") == 0 || strcmp(utf8_path, "..") == 0)
			continue;

		type = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? FILE_INFO_DIR : FILE_INFO_REG;

		conv_filetime_to_u64_ms(&find_data.ftLastWriteTime, &time);
		size = (uint64_t)find_data.nFileSizeHigh << 32;
		size |= find_data.nFileSizeLow;

		if ((tmp_path = malloc(strlen(dir_path) + strlen(utf8_path) + 2)) == NULL) {
			oom();
			free(utf8_path);
			FindClose(handle);
			return -2;
		}

		strcpy(tmp_path, dir_path);
		strcat(tmp_path, "/");
		strcat(tmp_path, utf8_path);
		file_info_add(tmp_path, time, size, type);

		if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			dir_tree_walk_for_files(tmp_path);

		free(tmp_path);
		free(utf8_path);
	} while (FindNextFileW(handle, &find_data) != 0);

	FindClose(handle);
	free(wdir_path);

	return 0;
#else
	return nftw(dir_path, nftw_fn, 15, FTW_DEPTH);
#endif
}

/* make delta patching a lot easier */
void swap_data(struct file_info *a, struct file_info *b)
{
	char *path = a->path;
	uint64_t modified_time = a->modified_time;
	uint64_t size = a->size;
	int type = a->type;

	a->path = b->path;
	a->modified_time = b->modified_time;
	a->size = b->size;
	a->type = b->type;

	b->path = path;
	b->modified_time = modified_time;
	b->size = size;
	b->type = type;
}

#if 0

#define m_max(X, Y) (((X) > (Y)) ? (X) : (Y))

int sorter(const char *a, const char *b)
{
	size_t a_len = strlen(a);
	size_t b_len = strlen(b);
	size_t l = m_max(a_len, b_len);
	size_t i;

	for (i = 0; i < l; i++) {
		if (a[i] > b[i]) return 1;
		if (a[i] < b[i]) return -1;
		if (a_len > b_len) return 1;
		if (a_len < b_len) return -1;
	}

	return 0;
}

#endif

void sort_dir_tree()
{
	struct file_info *fi_0, *fi_1;
	int swapped;

	fi_1 = NULL;

	do {
		swapped = 0;
		fi_0 = fi_head;

		while (fi_0->next != fi_1) {
			int wh = strcmp(fi_0->path, fi_0->next->path);
			if (wh > 0) {
				swap_data(fi_0, fi_0->next);
				swapped = 1;
			} else if (wh < 0) {
				swap_data(fi_0->next, fi_0);
				swapped = 1;
			}
			fi_0 = fi_0->next;
		}

		fi_1 = fi_0;
	} while (swapped);
}

void pakar_validate_index(FILE *fp)
{
	(void)fp;
}

void pakar_validate_chunk(FILE *fp)
{
	(void)fp;
}

void pakar_validate_all(FILE *index_fp)
{
	(void)index_fp;
}

// why? because windows.
char *work_strdup(const char *src)
{
	char *s = malloc(strlen(src) + 1);
	if (s == NULL) {
		oom();
		return NULL;
	}

	strcpy(s, src);
	return s;
}

char *work_basename(const char *path)
{
	char *s = strrchr(path, '/');
	if (s == NULL)
		return work_strdup(path);
	else
		return work_strdup(s + 1);
}

int work_in_console()
{
#ifndef _WIN32
	return isatty(fileno(stdout));
#else
	DWORD dummy;
	return GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dummy);
#endif
}

FILE *fopen_chunk(const char *name, const char *open_mode, uint32_t num)
{
	FILE *fp;
	const char *tmp;
	char *tmp_basename;
	char *path;
	int len;

	tmp_basename = NULL;

	if (work_in_console() != 0) {
		tmp_basename = work_basename(name);
		if (tmp_basename == NULL)
			return NULL;

		tmp = tmp_basename;
	} else {
		tmp = name;
	}

	len = snprintf(NULL, 0, "%s_%03" PRIu32 ".pak", tmp, num);
	if ((path = malloc(len + 2)) == NULL) {
		if (tmp_basename != NULL)
			free(tmp_basename);

		oom();
		return NULL;
	}

	path[len + 1] = '\0';
	snprintf(path, len + 1, "%s_%03" PRIu32 ".pak", tmp, num);

	if ((fp = fopen(path, open_mode)) == NULL)
		fprintf(stderr, "failed to open chunk pak: %s with openmode %s\n", path, open_mode);

	free(path);

	if (tmp_basename != NULL)
		free(tmp_basename);

	return fp;
}

FILE *fopen_index(const char *name, const char *open_mode)
{
	FILE *fp;
	char *tmp_s;
	char *path;

	if (work_in_console() != 0) {
		tmp_s = work_basename(name);
		if (tmp_s == NULL)
			return NULL;

		path = malloc(strlen(tmp_s) + 11);
		if (path == NULL) {
			oom();
			free(tmp_s);
			return NULL;
		}

		strcpy(path, tmp_s);
		free(tmp_s);
	} else {
		path = malloc(strlen(name) + 11);
		if (path == NULL) {
			oom();
			return NULL;
		}

		strcpy(path, name);
	}

	strcat(path, "_index.pak");

	if ((fp = fopen(path, open_mode)) == NULL)
		fprintf(stderr, "failed to open index pak: %s with openmode %s\n", path, open_mode);

	free(path);
	return fp;
}

int pakar_op_create(const char *path, int argc, char **argv)
{
	struct pakar_chunk_header chunk_header;
	struct pakar_index_header index_header;
	struct serialize_stream stream;
	char stream_buf[512], holder[4096];
	FILE *index_fp, *chunk_fp, *entry_fp;
	struct file_info *fi;
	uint64_t offset;
	int retcode;

	struct file_entry *fe_head, *fe_tmp;

	(void)argc;
	(void)argv;
	(void)stream_buf;
	(void)stream;

	fe_head = NULL;

	if (dir_tree_walk_for_files(path) != 0) {
		fprintf(stderr, "error while doing directory walk in: %s!\n", path);
		file_info_cleanup();
		return 2;
	}

	sort_dir_tree();

	index_header.begin = 0x3F212A7C;
	index_header.version = PAKAR_VERSION;
	index_header.num_files = num_fi;
	index_header.num_chunks = 1;
	index_header.size = fi_total_size;
	memset(index_header.sha256, 0, sizeof(index_header.sha256));

	/* regular processing */
	for (fi = fi_head; fi != NULL; fi = fi->next) {
		if (fi->type != FILE_INFO_REG)
			continue;

		fe_tmp = malloc(sizeof(struct file_entry));
		fe_tmp->entry.crc32 = 0xFFFFFFFF;
		fe_tmp->entry.modified_time = fi->modified_time;
		fe_tmp->entry.size = fi->size;
		fe_tmp->entry.offset = 0;
		fe_tmp->entry.type = fi->type;
		fe_tmp->path = fi->path;
		memcpy((char *)fe_tmp->entry.path, fi->path + strlen(path), sizeof(fe_tmp->entry.path));

		fe_tmp->next = fe_head;
		fe_head = fe_tmp;
	}

	/* directory processing */
	for (fi = fi_head; fi != NULL; fi = fi->next) {
		if (fi->type != FILE_INFO_DIR)
			continue;

		fe_tmp = malloc(sizeof(struct file_entry));
		fe_tmp->entry.crc32 = 0xDEADBEEF;
		fe_tmp->entry.modified_time = fi->modified_time;
		fe_tmp->entry.size = 0;
		fe_tmp->entry.offset = 0xFFFFFFFFFFFFFFFF;
		fe_tmp->entry.type = fi->type;

		// hardcode root path for the pak
		if (strcmp(fi->path, path) == 0) {
			fe_tmp->path = NULL;
			strcpy(fe_tmp->entry.path, "/");
		} else {
			fe_tmp->path = fi->path;
			memcpy((char *)fe_tmp->entry.path, fi->path + strlen(path), sizeof(fe_tmp->entry.path));
		}


		fe_tmp->next = fe_head;
		fe_head = fe_tmp;
	}

	if ((index_fp = fopen_index(path, "wb")) == NULL) {
		retcode = 2;
		goto cleanup;
	}

	serialize_stream_init(&stream, stream_buf, sizeof(stream_buf));

	/* chunking */
	for (retcode = 0, chunk_fp = NULL, fe_tmp = fe_head; fe_tmp != NULL; fe_tmp = fe_tmp->next) {
		if (fe_tmp->entry.type == FILE_INFO_DIR)
			continue;

		if (chunk_fp == NULL) {
			if ((chunk_fp = fopen_chunk(path, "wb", index_header.num_chunks++ - 1)) == NULL) {
				retcode = 3;
				goto cleanup;
			}

			chunk_header.begin = 0x3F212A2A;
			chunk_header.version = PAKAR_VERSION;

			serialize_stream_write_u32(&stream, &chunk_header.begin);
			serialize_stream_write_u32(&stream, &chunk_header.version);
			fwrite(stream.data, 1, stream.offset, chunk_fp);
			serialize_stream_seek(&stream, 0, SERIALIZE_STREAM_SEEK_BEGIN);
			fflush(chunk_fp);

			offset = 0;
		}

		if (fe_tmp->path == NULL)
			continue;

		if ((entry_fp = fopen(fe_tmp->path, "rb")) == NULL)
			continue;

		fe_tmp->entry.offset = offset;

		do {
			memset(holder, 0, sizeof(holder));
			fread(holder, 1, sizeof(holder), entry_fp);
			fwrite(holder, 1, sizeof(holder), chunk_fp);
			offset += sizeof(holder);
		} while (feof(entry_fp) == 0 && ferror(entry_fp) == 0);

		fclose(entry_fp);
	}

	if (chunk_fp != NULL) {
		fflush(chunk_fp);
		fclose(chunk_fp);
	}

	serialize_stream_write_pakar_index_header(&stream, &index_header);
	fwrite(stream.data, 1, stream.offset, index_fp);
	serialize_stream_seek(&stream, 0, SERIALIZE_STREAM_SEEK_BEGIN);

	for (fe_tmp = fe_head; fe_tmp != NULL; fe_tmp = fe_tmp->next) {
		serialize_stream_write_pakar_index_entry(&stream, &fe_tmp->entry);
		fwrite(stream.data, 1, stream.offset, index_fp);
		serialize_stream_seek(&stream, 0, SERIALIZE_STREAM_SEEK_BEGIN);
	}

	fclose(index_fp);
cleanup:
	file_info_cleanup();

	while (fe_head != NULL) {
		fe_tmp = fe_head->next;
		free(fe_head);
		fe_head = fe_tmp;
	}

	return retcode;
}

int pakar_op_add(const char *path, int argc, char **argv)
{
	(void)path;
	(void)argc;
	(void)argv;
	return 4;
}

int pakar_op_extract_all(const char *path, int argc, char **argv)
{
	(void)path;
	(void)argc;
	(void)argv;
	return 4;
}

int pakar_op_extract_singular(const char *path, int argc, char **argv)
{
	(void)path;
	(void)argc;
	(void)argv;
	return 4;
}

int pakar_op_list(const char *path, int argc, char **argv)
{
	(void)path;
	(void)argc;
	(void)argv;
	return 4;
}

int pakar_op_rehash(const char *path, int argc, char **argv)
{
	(void)path;
	(void)argc;
	(void)argv;
	return 4;
}

int pakar_op_validate(const char *path, int argc, char **argv)
{
	(void)path;
	(void)argc;
	(void)argv;
	return 4;
}

typedef int (*pakar_op)(const char *path, int argc, char **argv);

static const pakar_op pakar_ops[] = {
	pakar_op_create,
	pakar_op_add,
	pakar_op_extract_all,
	pakar_op_extract_singular,
	pakar_op_list,
	pakar_op_rehash,
	pakar_op_validate
};

enum {
	PAKAR_MODE_CREATE,
	PAKAR_MODE_ADD,
	PAKAR_MODE_EXTRACT_ALL,
	PAKAR_MODE_EXTRACT_SINGULAR,
	PAKAR_MODE_LIST,
	PAKAR_MODE_REHASH,
	PAKAR_MODE_VALIDATE,
};

#ifdef _WIN32
int _main(int argc, char **argv);

int wmain(int argc, WCHAR **argv)
{
	char **utf8_argv;
	int i, len, retcode;

	if ((utf8_argv = malloc(argc * sizeof(char *))) == NULL) {
		oom();

		/* picked a random number for this particular error */
		return 52;
	}

	for (i = 0; i < argc; i++) {
		if ((len = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL)) <= 0) {
			fprintf(stderr, "fatal error\n");
			return 52;
		}

		if ((utf8_argv[i] = malloc(len + 1)) == NULL) {
			oom();
			return 52;
		}
		utf8_argv[len] = '\0';

		WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, utf8_argv[i], len, NULL, NULL);
	}

	retcode = _main(argc, utf8_argv);

	for (i = 0; i < argc; i++)
		free(utf8_argv[i]);

	free(utf8_argv);

	return retcode;
}
#endif

int
#ifdef _WIN32
_main
#else
main
#endif
(int argc, char **argv)
{
	char *args, *tmp, *path;
	int i, mode, retcode;

	if (argc < 2) {
		fprintf(stderr, "no commands? use -h for help\n");
		return 72;
	}

	tmp = NULL;

	/* set mode, and grab a path. */
	for (i = 1, mode = -1; i < argc; i++) {
		args = argv[i];

		if (strcmp(args, "-h") == 0) {
			printf("%s\n", help_msg);
			return 0;
		}

		if (mode == -1) {
			if (strcmp(args, "add") == 0) {
				mode = PAKAR_MODE_ADD;
				continue;
			} else if (strcmp(args, "extract") == 0) {
				mode = PAKAR_MODE_EXTRACT_SINGULAR;
				continue;
			} else if (strcmp(args, "list") == 0) {
				mode = PAKAR_MODE_LIST;
				continue;
			} else if (strcmp(args, "rehash") == 0) {
				mode = PAKAR_MODE_REHASH;
				continue;
			} else if (strcmp(args, "validate") == 0) {
				mode = PAKAR_MODE_VALIDATE;
				continue;
			}

			if (is_directory(args)) {
				mode = PAKAR_MODE_CREATE;
			} else if (is_file(args)) {
				mode = PAKAR_MODE_EXTRACT_ALL;
			}
		}

		if (mode == -1) {
			printf("no commands, dir or (valid) index.pak passed?\n");
			return 72;
		}

		tmp = args;
		break;
	}

	if (tmp == NULL) {
		fprintf(stderr, "no path to a dir or index.pak?\n");
		return 72;
	}

	if ((path = malloc(strlen(tmp) + 1)) == NULL) {
		oom();
		return 71;
	}

	/* filter out any trailing slashes */
	strcpy(path, tmp);
	tmp = strrchr(path, '\0');

	for (;; tmp--) {
		if (*tmp == '\0')
			continue;

		if (*tmp == '/' || *tmp == '\\') {
			*tmp = '\0';
			continue;
		}

		break;
	}

	retcode = (*(pakar_ops[mode]))(path, argc, argv);

	free(path);
	return retcode;
}
