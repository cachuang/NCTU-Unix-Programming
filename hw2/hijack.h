#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <pwd.h>
#include <grp.h>
#include <string>

static void init(void) __attribute__((constructor));
static void end(void)  __attribute__((destructor));

typedef int (*closedir_t)(DIR *dirp);
typedef DIR *(*fdopendir_t)(int fd);
typedef DIR *(*opendir_t)(const char *name);
typedef struct dirent *(*readdir_t)(DIR *dirp);
typedef int (*readdir_r_t)(DIR *dirp, struct dirent *entry, struct dirent **result);
typedef void (*rewinddir_t)(DIR *dirp);
typedef void (*seekdir_t)(DIR *dirp, long offset);
typedef long (*telldir_t)(DIR *dirp);
typedef int (*creat_t)(const char *pathname, mode_t mode);
typedef int (*open_t)(const char *pathname, int flags);
typedef int (*remove_t)(const char *pathname);
typedef int (*rename_t)(const char *oldpath, const char *newpath);
typedef void (*setbuf_t)(FILE *stream, char *buf);
typedef int (*setvbuf_t)(FILE *stream, char *buf, int mode, size_t size);
typedef char *(*tempnam_t)(const char *dir, const char *pfx);
typedef FILE *(*tmpfile_t)(void);
typedef char *(*tmpnam_t)(char *s);
typedef void (*exit_t)(int status);
typedef char *(*getenv_t)(const char *name);
typedef char *(*mkdtemp_t)(char *_template);
typedef int (*mkstemp_t)(char *_template);
typedef int (*putenv_t)(char *string);
typedef int (*rand_t)(void);
typedef int (*rand_r_t)(unsigned int *seedp);
typedef int (*setenv_t)(const char *name, const char *value, int overwrite);
typedef void (*srand_t)(unsigned int seed);
typedef int (*system_t)(const char *command);
typedef int (*chdir_t)(const char *path);
typedef int (*chown_t)(const char *path, uid_t owner, gid_t group);
typedef int (*close_t)(int fd);
typedef int (*dup_t)(int oldfd);
typedef int (*dup2_t)(int oldfd, int newfd);
typedef void (*_exit_t)(int status);
typedef int (*execle_t)(const char *path, const char *arg, ...);
typedef int (*execlp_t)(const char *file, const char *arg, ...);
typedef int (*execv_t)(const char *path, char *const argv[]);
typedef int (*execve_t)(const char *filename, char *const argv[], char *const envp[]);
typedef int (*execvp_t)(const char *file, char *const argv[]);
typedef int (*fchdir_t)(int fd);
typedef int (*fchown_t)(int fd, uid_t owner, gid_t group);
typedef pid_t (*fork_t)(void);
typedef int (*fsync_t)(int fd);
typedef int (*ftruncate_t)(int fd, off_t length);
typedef char *(*getcwd_t)(char *buf, size_t size);
typedef gid_t (*getegid_t)(void);
typedef uid_t (*geteuid_t)(void);
typedef gid_t (*getgid_t)(void);
typedef uid_t (*getuid_t)(void);
typedef int (*link_t)(const char *oldpath, const char *newpath);
typedef int (*pipe_t)(int pipefd[2]);
typedef ssize_t (*pread_t)(int fd, void *buf, size_t count, off_t offset);
typedef ssize_t (*pwrite_t)(int fd, const void *buf, size_t count, off_t offset);
typedef ssize_t (*read_t)(int fd, void *buf, size_t count);
typedef ssize_t (*readlink_t)(const char *path, char *buf, size_t bufsiz);
typedef int (*rmdir_t)(const char *pathname);
typedef int (*setegid_t)(gid_t egid);
typedef int (*seteuid_t)(uid_t euid);
typedef int (*setgid_t)(gid_t gid);
typedef int (*setuid_t)(uid_t uid);
typedef unsigned int (*sleep_t)(unsigned int seconds);
typedef int (*symlink_t)(const char *oldpath, const char *newpath);
typedef int (*unlink_t)(const char *path);
typedef ssize_t (*write_t)(int fd, const void *buf, size_t nbyte);
typedef int (*chmod_t)(const char *path, mode_t mode);
typedef int (*fchmod_t)(int fd, mode_t mode);
typedef int (*__fxstat_t)(int ver, int fildes, struct stat *buf);
typedef int (*__lxstat_t)(int ver, const char *path, struct stat *buf);
typedef int (*mkdir_t)(const char *pathname, mode_t mode);
typedef int (*mkfifo_t)(const char *pathname, mode_t mode);
typedef int (*__xstat_t)(int ver, const char *path, struct stat *buf);
typedef mode_t (*umask_t)(mode_t mask);
// Additional monitored function
typedef int (*fflush_t)(FILE *stream);
typedef void *(*realloc_t)(void *ptr, size_t size);
typedef size_t (*fwrite_t)(const void *ptr, size_t size, size_t count, FILE *stream);
typedef char *(*setlocale_t)(int category, const char *locale);
typedef struct tm *(*localtime_t)(const time_t *timep);
