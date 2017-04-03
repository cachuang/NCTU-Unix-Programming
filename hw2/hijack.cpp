#include "hijack.h"

void *handle;
FILE *output;

#define OUTPUT_PREFIX "[monitor]"
#define MAXLINE 1024

using namespace std;

static void init(void)
{
    handle = dlopen("libc.so.6", RTLD_LAZY);

    output = fopen("monitor.out", "w");;
}

string getFileNameByFd(int fd)
{
    char fd_path[MAXLINE];
    char filename[MAXLINE];

    snprintf(fd_path, sizeof(fd_path), "/proc/self/fd/%d", fd);

    // call original readlink
    readlink_t original_readlink = (readlink_t) dlsym(handle, "readlink");
    int n = original_readlink(fd_path, filename, MAXLINE);

    if (n < 0)
        perror("readlink() error");
    else
        filename[n] = '\0';

    return string(filename);
}

extern "C" 
{

int closedir(DIR *dirp)
{

}
DIR* fdopendir(int fd)
{

}
DIR* opendir(const char *name)
{

}
struct dirent* readdir(DIR *dirp)
{

}
int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{

}
void rewinddir(DIR *dirp)
{

}
void seekdir(DIR *dirp, long offset)
{

}
long telldir(DIR *dirp)
{

}
int creat(const char *pathname, mode_t mode)
{
    // call original function
    creat_t original_creat = (creat_t) dlsym(handle, __func__);
    int ret = original_creat(pathname, mode);

    fprintf(output, "%s %s('%s', %o) = %d\n", OUTPUT_PREFIX, __func__, pathname, mode, ret);

    return ret;
}
int open(const char *pathname, int flags)
{
	// call original function
    open_t original_open = (open_t) dlsym(handle, __func__);
    int ret = original_open(pathname, flags);

    fprintf(output, "%s %s('%s', %d) = %d\n", OUTPUT_PREFIX, __func__, pathname, flags, ret);

    return ret;
}

int remove(const char *pathname)
{
    // call original function
    remove_t original_remove = (remove_t) dlsym(handle, __func__);
    int ret = original_remove(pathname);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, pathname, ret);

    return ret;
}
int rename(const char *oldpath, const char *newpath)
{
    // call original function
    rename_t original_rename = (rename_t) dlsym(handle, __func__);
    int ret = original_rename(oldpath, newpath);

    fprintf(output, "%s %s('%s', '%s') = %d\n", OUTPUT_PREFIX, __func__, oldpath, newpath, ret);

    return ret;
}
void setbuf(FILE *stream, char *buf)
{
    // call original function
    setbuf_t original_setbuf = (setbuf_t) dlsym(handle, __func__);
    original_setbuf(stream, buf);

    string filename;

    if (stream) {
        int fd = fileno(stream);
        filename = getFileNameByFd(fd);
    }

    fprintf(output, "%s %s('%s', '%s')\n", OUTPUT_PREFIX, __func__, filename.c_str(), buf);
}
int setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
    // call original function
    setvbuf_t original_setvbuf = (setvbuf_t) dlsym(handle, __func__);
    int ret = original_setvbuf(stream, buf, mode, size);

    string filename;

    if (stream) {
        int fd = fileno(stream);
        filename = getFileNameByFd(fd);
    }

    fprintf(output, "%s %s('%s', '%s', %d, %zu) = %d\n", OUTPUT_PREFIX, __func__, filename.c_str(), buf, mode, size, ret);
}

char* tempnam(const char *dir, const char *pfx)
{
    // call original function
    tempnam_t original_tempnam = (tempnam_t) dlsym(handle, __func__);
    char *ret = original_tempnam(dir, pfx);

    fprintf(output, "%s %s('%s', '%s') = %s\n", OUTPUT_PREFIX, __func__, dir, pfx, ret);

    return ret;
}

FILE* tmpfile(void)
{
    // call original function
    tmpfile_t original_tmpfile = (tmpfile_t) dlsym(handle, __func__);
    FILE *ret = original_tmpfile();

    int fd = fileno(ret);
    string filename = getFileNameByFd(fd);

    fprintf(output, "%s %s() = %s\n", OUTPUT_PREFIX, __func__, filename.c_str());

    return ret;
}
char* tmpnam(char *s)
{
    // call original function
    tmpnam_t original_tmpnam = (tmpnam_t) dlsym(handle, __func__);
    char *ret = original_tmpnam(s);

    fprintf(output, "%s %s('%s') = %s\n", OUTPUT_PREFIX, __func__, s, ret);

    return ret;
}
void exit(int status)
{
    fprintf(output, "%s %s(%d)\n", OUTPUT_PREFIX, __func__, status);

    // call original function
    __attribute__((__noreturn__)) exit_t original_exit = (exit_t) dlsym(handle, "exit");
    original_exit(status);
}
char* getenv(const char *name)
{
    // call original function
    getenv_t original_getenv = (getenv_t) dlsym(handle, __func__);
    char *ret = original_getenv(name);

    fprintf(output, "%s %s('%s') = %s\n", OUTPUT_PREFIX, __func__, name, ret);

    return ret;
}
char* mkdtemp(char *_template)
{
    // call original function
    mkdtemp_t original_mkdtemp = (mkdtemp_t) dlsym(handle, __func__);
    char *ret = original_mkdtemp(_template);

    fprintf(output, "%s %s('%s') = %s\n", OUTPUT_PREFIX, __func__, _template, ret);

    return ret;
}
int mkstemp(char *_template)
{
    // call original function
    mkstemp_t original_mkstemp = (mkstemp_t) dlsym(handle, __func__);
    int ret = original_mkstemp(_template);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, _template, ret);

    return ret;
}
int putenv(char *string)
{
    // call original function
    putenv_t original_putenv = (putenv_t) dlsym(handle, __func__);
    int ret = original_putenv(string);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, string, ret);

    return ret;
}
int rand(void)
{
    // call original function
    rand_t original_rand = (rand_t) dlsym(handle, __func__);
    int ret = original_rand();

    fprintf(output, "%s %s() = %d\n", OUTPUT_PREFIX, __func__, ret);

    return ret;
}
int rand_r(unsigned int *seedp)
{
    // call original function
    rand_r_t original_rand_r = (rand_r_t) dlsym(handle, __func__);
    int ret = original_rand_r(seedp);

    fprintf(output, "%s %s(%u) = %d\n", OUTPUT_PREFIX, __func__, *seedp, ret);

    return ret;
}
int setenv(const char *name, const char *value, int overwrite)
{
    // call original function
    setenv_t original_setenv = (setenv_t) dlsym(handle, __func__);
    int ret = original_setenv(name, value, overwrite);

    fprintf(output, "%s %s('%s', '%s', %d) = %d\n", OUTPUT_PREFIX, __func__, name, value, overwrite, ret);

    return ret;
}
void srand(unsigned int seed)
{
    fprintf(output, "%s %s() = %u\n", OUTPUT_PREFIX, __func__, seed);

    // call original function
    srand_t original_srand = (srand_t) dlsym(handle, __func__);
    original_srand(seed);
}
int system(const char *command)
{
    // call original function
    system_t original_system = (system_t) dlsym(handle, __func__);
    int ret = original_system(command);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, command, ret);

    return ret;
}
int chdir(const char *path)
{
    // call original function
    chdir_t original_chdir = (chdir_t) dlsym(handle, __func__);
    int ret = original_chdir(path);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, path, ret);

    return ret;
}
int chown(const char *path, uid_t owner, gid_t group)
{
    // call original function
    chown_t original_chown = (chown_t) dlsym(handle, __func__);
    int ret = original_chown(path, owner, group);

    struct group *gr = getgrgid(group);
    struct passwd *pw = getpwuid(owner);

    fprintf(output, "%s %s('%s', '%s', '%s') = %d\n", OUTPUT_PREFIX, __func__, path, pw->pw_name, gr->gr_name, ret);

    return ret;
}
int close(int fd)
{
    // call original function
    close_t original_close = (close_t) dlsym(handle, __func__);
    int ret = original_close(fd);

    fprintf(output, "%s %s(%d) = %d\n", OUTPUT_PREFIX, __func__, fd, ret);

    return ret;
}
int dup(int oldfd)
{
    // call original function
    dup_t original_dup = (dup_t) dlsym(handle, __func__);
    int ret = original_dup(oldfd);

    fprintf(output, "%s %s(%d) = %d\n", OUTPUT_PREFIX, __func__, oldfd, ret);

    return ret;
}
int dup2(int oldfd, int newfd)
{
    // call original function
    dup2_t original_dup2 = (dup2_t) dlsym(handle, __func__);
    int ret = original_dup2(oldfd, newfd);

    fprintf(output, "%s %s(%d, %d) = %d\n", OUTPUT_PREFIX, __func__, oldfd, newfd, ret);

    return ret;
}
void _exit(int status)
{
    fprintf(output, "%s %s(%d)\n", OUTPUT_PREFIX, __func__, status);
    // _exit won't flush I/O buffer, so explict call fclose here
    fclose(output);

    __attribute__((__noreturn__)) _exit_t original__exit = (_exit_t) dlsym(handle, "_exit");
    original__exit(status);
}
#define MAXARGS 31
extern char **environ;
int execl(const char *path, const char *arg, ...)
{
    va_list ap;
    char *array[MAXARGS +1];
    int argno = 0;

    va_start(ap, arg);
    while (arg != 0 && argno < MAXARGS)
    {
        array[argno++] = (char *) arg;
        arg = va_arg(ap, const char *);
    }
    array[argno] = NULL;
    va_end(ap);

    fclose(output);
    unsetenv("LD_PRELOAD");

    execv_t original_execv = (execv_t) dlsym(handle, "execv");
    int ret = original_execv(path, array);

    //fprintf(output, "%s %s('%s', '%s', %d) = %d\n", OUTPUT_PREFIX, __func__, name, value, overwrite, ret);

    return ret;
}
int execle(const char *path, const char *arg, ...)
{

}
int execlp(const char *file, const char *arg, ...)
{

}
int execv(const char *path, char *const argv[])
{
    // call original function
    // execv_t original_execv = (execv_t) dlsym(handle, __func__);
    // int ret = original_execv(path, argv);

    // fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, path, argv, ret);
}
int execve(const char *filename, char *const argv[], char *const envp[])
{

}
int execvp(const char *file, char *const argv[])
{

}
int fchdir(int fd)
{

}
int fchown(int fd, uid_t owner, gid_t group)
{

}
pid_t fork(void)
{

}
int fsync(int fd)
{

}
int ftruncate(int fd, off_t length)
{

}
char* getcwd(char *buf, size_t size)
{
    // call original function
    getcwd_t original_getcwd = (getcwd_t) dlsym(handle, __func__);
    char *ret = original_getcwd(buf, size);

    fprintf(output, "%s %s('%s', %zu) = %p\n", OUTPUT_PREFIX, __func__, buf, size, ret);

    return ret;
}
gid_t getegid(void)
{

}
uid_t geteuid(void)
{

}
gid_t getgid(void)
{

}
uid_t getuid(void)
{

}
int link(const char *oldpath, const char *newpath)
{

}
int pipe(int pipefd[2])
{

}
ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{

}
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)
{

}
ssize_t read(int fd, void *buf, size_t count)
{

}
ssize_t readlink(const char *path, char *buf, size_t bufsiz)
{

}
int rmdir(const char *pathname)
{

}
int setegid(gid_t egid)
{

}
int seteuid(uid_t euid)
{

}
int setgid(gid_t gid)
{

}
int setuid(uid_t uid)
{

}
unsigned int sleep(unsigned int seconds)
{
    sleep_t original_sleep = (sleep_t) dlsym(handle, __func__);
    original_sleep(seconds);

    return 0;
}
int symlink(const char *oldpath, const char *newpath)
{

}
int unlink(const char *path)
{

}
ssize_t write(int fildes, const void *buf, size_t nbyte)
{

}
int chmod(const char *path, mode_t mode)
{

}
int fchmod(int fd, mode_t mode)
{

}
int fstat(int fildes, struct stat *buf)
{

}
int lstat(const char *path, struct stat *buf)
{

}
int mkdir(const char *pathname, mode_t mode)
{

}
int mkfifo(const char *pathname, mode_t mode)
{

}
int stat(const char *path, struct stat *buf)
{

}
mode_t umask(mode_t mask)
{

}



}