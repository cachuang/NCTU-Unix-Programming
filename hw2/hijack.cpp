#include "hijack.h"

#define OUTPUT_PREFIX "[monitor]"
#define GLIBC "libc.so.6"
#define DEFAULT_MONITOR_OUTPUT "monitor.out"
#define MAXLINE 1024
#define MAX_ENVIRON_LINE 5

using namespace std;

void *handle;
FILE *output;

static void init(void)
{
    handle = dlopen(GLIBC, RTLD_LAZY);

    getenv_t original_getenv = (getenv_t) dlsym(handle, "getenv");
    char *output_file = original_getenv("MONITOR_OUTPUT");

    if(output_file)
    {
        if(!strcmp(output_file, "stderr"))
            output = stderr;
        else
            output = fopen(output_file, "w");
    }
    else
        output = fopen(DEFAULT_MONITOR_OUTPUT, "w");
}

static void end(void)
{
    fclose(output);
}

string getFileNameByFd(int fd)
{
    char fd_path[MAXLINE];
    char filename[MAXLINE];

    snprintf(fd_path, sizeof(fd_path), "/proc/self/fd/%d", fd);

    // call original readlink
    readlink_t original_readlink = (readlink_t) dlsym(handle, "readlink");
    int n = original_readlink(fd_path, filename, MAXLINE);

    if (n < 0) {
        return "";
    }
    else
        filename[n] = '\0';

    return string(filename);
}

extern "C" 
{

int closedir(DIR *dirp)
{
    string dirname = getFileNameByFd(dirfd(dirp));

    // call original function
    closedir_t original_closedir = (closedir_t) dlsym(handle, __func__);
    int ret = original_closedir(dirp);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, dirname.c_str(), ret);

    return ret;
}
DIR* fdopendir(int fd)
{
    // call original function
    fdopendir_t original_fdopendir = (fdopendir_t) dlsym(handle, __func__);
    DIR *ret = original_fdopendir(fd);

    string dirname = getFileNameByFd(fd);

    fprintf(output, "%s %s(%d) = %s\n", OUTPUT_PREFIX, __func__, fd, dirname.c_str());

    return ret;
}
DIR* opendir(const char *name)
{
    // call original function
    opendir_t original_opendir = (opendir_t) dlsym(handle, __func__);
    DIR *ret = original_opendir(name);

    string dirname = getFileNameByFd(dirfd(ret));

    fprintf(output, "%s %s('%s') = %s\n", OUTPUT_PREFIX, __func__, name, dirname.c_str());

    return ret;
}
struct dirent* readdir(DIR *dirp)
{
    // call original function
    readdir_t original_readdir = (readdir_t) dlsym(handle, __func__);
    struct dirent *ret = original_readdir(dirp);

    string dirname = getFileNameByFd(dirfd(dirp));

    if(ret) 
    {
        ino_t ino = ret->d_ino;
        off_t off = ret->d_off;
        unsigned short int reclen = ret->d_reclen;
        unsigned char type = ret->d_type;
        char *name = ret->d_name;

        fprintf(output, "%s %s('%s') = (inode=%zu, offset=%jd, reclen=%u, type=%u, name=%s)\n", OUTPUT_PREFIX, __func__, dirname.c_str(), ino, off, reclen, type, name);
    }
    else 
        fprintf(output, "%s %s('%s') = (null)\n", OUTPUT_PREFIX, __func__, dirname.c_str());

    return ret;
}
int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
    // call original function
    readdir_r_t original_readdir_r = (readdir_r_t) dlsym(handle, __func__);
    int ret = original_readdir_r(dirp, entry, result);

    string dirname = getFileNameByFd(dirfd(dirp));

    ino_t ino = entry->d_ino;
    off_t off = entry->d_off;
    unsigned short int reclen = entry->d_reclen;
    unsigned char type = entry->d_type;
    char *name = entry->d_name;

    fprintf(output, "%s %s('%s', (inode=%zu, offset=%jd, reclen=%u, type=%u, name=%s), %p) = %d\n", OUTPUT_PREFIX, __func__, dirname.c_str(), ino, off, reclen, type, name, result, ret);

    return ret;
}
void rewinddir(DIR *dirp)
{
    // call original function
    rewinddir_t original_rewinddir = (rewinddir_t) dlsym(handle, __func__);
    original_rewinddir(dirp);

    string dirname = getFileNameByFd(dirfd(dirp));

    fprintf(output, "%s %s('%s')\n", OUTPUT_PREFIX, __func__, dirname.c_str());

    return;
}
void seekdir(DIR *dirp, long offset)
{
    // call original function
    seekdir_t original_seekdir = (seekdir_t) dlsym(handle, __func__);
    original_seekdir(dirp, offset);

    string dirname = getFileNameByFd(dirfd(dirp));

    fprintf(output, "%s %s('%s', %ld)\n", OUTPUT_PREFIX, __func__, dirname.c_str(), offset);

    return;
}
long telldir(DIR *dirp)
{
    // call original function
    telldir_t original_telldir = (telldir_t) dlsym(handle, __func__);
    long ret = original_telldir(dirp);

    string dirname = getFileNameByFd(dirfd(dirp));

    fprintf(output, "%s %s('%s') = %ld\n", OUTPUT_PREFIX, __func__, dirname.c_str(), ret);

    return ret;
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
// int system(const char *command)
// {
//     // call original function
//     system_t original_system = (system_t) dlsym(handle, __func__);
//     int ret = original_system(command);

//     fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, command, ret);

//     return ret;
// }
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
    // _exit won't flush I/O buffer, so explict call fflush here
    fflush(output);

    __attribute__((__noreturn__)) _exit_t original__exit = (_exit_t) dlsym(handle, "_exit");
    original__exit(status);
}
#define MAXARGS 31

int execl(const char *path, const char *arg, ...)
{
    ptrdiff_t argc;
    va_list ap;
    va_start(ap, arg);
    for (argc = 1; va_arg(ap, const char *); argc++)
    {
      if (argc == INT_MAX)
        {
            va_end (ap);
            errno = E2BIG;
            return -1;
        }
    }
    va_end (ap);

    fprintf(output, "%s %s('%s', ", OUTPUT_PREFIX, __func__, path);

    ptrdiff_t i;
    char *argv[argc + 1];
    va_start (ap, arg);
    argv[0] = (char *) arg;
    for (i = 1; i <= argc; i++) {
        argv[i] = va_arg (ap, char *);
    }
    va_end (ap);

    for(i = 0; i <= argc; i++) {
        fprintf(output, "'%s'", argv[i]);
        if(i != argc)
            fprintf(output, ", ");
    }
    fprintf(output, ")\n");

    // The exec() functions will replace original process, so explicit call fflush here to flush the output
    fflush(output);
    // The exec() functions will inherit environment variable, result in generate another monitor.out and override the original monitor.out
    // unset the LD_PRELOAD to avoid
    unsetenv("LD_PRELOAD");

    execve_t original_execve = (execve_t) dlsym(handle, "execve");
    int ret = original_execve(path, argv, __environ);

    // The exec() functions return only if an error has occurred
    // append the return value to the output
    // use fseek() to go back to the position of '\n'
    fseek(output, -1, SEEK_CUR);    
    fprintf(output, " = %d\n", ret);

    return ret;
}
int execle(const char *path, const char *arg, ...)
{
    ptrdiff_t argc;
    va_list ap;
    va_start(ap, arg);
    for (argc = 1; va_arg(ap, const char *); argc++)
    {
      if (argc == INT_MAX)
        {
            va_end (ap);
            errno = E2BIG;
            return -1;
        }
    }
    va_end (ap);

    fprintf(output, "%s %s('%s', ", OUTPUT_PREFIX, __func__, path);

    ptrdiff_t i;
    char *argv[argc + 1];
    va_start (ap, arg);
    argv[0] = (char *) arg;
    for (i = 1; i <= argc; i++) {
        argv[i] = va_arg (ap, char *);
    }
    // In execle(), the last argument is environmnet variable 
    char **envp = va_arg (ap, char **);
    va_end (ap);

    for(i = 0; i <= argc; i++) {
        fprintf(output, "'%s'", argv[i]);
        if(i != argc)
            fprintf(output, ", ");
        else
        {
            for(int i = 0; envp[i] != NULL && i < MAX_ENVIRON_LINE; i++) 
                fprintf(output, ", '%s'", envp[i]);
        }
    }
    fprintf(output, ")\n");

    // The exec() functions will replace original process, so explicit call fflush here to flush the output
    fflush(output);
    // The exec() functions will inherit environment variable, result in generate another monitor.out and override the original monitor.out
    // unset the LD_PRELOAD to avoid
    unsetenv("LD_PRELOAD");

    execve_t original_execve = (execve_t) dlsym(handle, "execve");
    int ret = original_execve(path, argv, __environ);

    // The exec() functions return only if an error has occurred
    // append the return value to the output
    // use fseek() to go back to the position of '\n'
    fseek(output, -1, SEEK_CUR);    
    fprintf(output, " = %d\n", ret);

    return ret;
}
int execlp(const char *file, const char *arg, ...)
{
    ptrdiff_t argc;
    va_list ap;
    va_start(ap, arg);
    for (argc = 1; va_arg(ap, const char *); argc++)
    {
      if (argc == INT_MAX)
        {
            va_end (ap);
            errno = E2BIG;
            return -1;
        }
    }
    va_end (ap);

    fprintf(output, "%s %s('%s', ", OUTPUT_PREFIX, __func__, file);

    ptrdiff_t i;
    char *argv[argc + 1];
    va_start (ap, arg);
    argv[0] = (char *) arg;
    for (i = 1; i <= argc; i++) {
        argv[i] = va_arg (ap, char *);
    }
    va_end (ap);

    for(i = 0; i <= argc; i++) {
        fprintf(output, "'%s'", argv[i]);
        if(i != argc)
            fprintf(output, ", ");
    }
    fprintf(output, ")\n");

    // The exec() functions will replace original process, so explicit call fflush here to flush the output
    fflush(output);
    // The exec() functions will inherit environment variable, result in generate another monitor.out and override the original monitor.out
    // unset the LD_PRELOAD to avoid
    unsetenv("LD_PRELOAD");

    execvp_t original_execvp = (execvp_t) dlsym(handle, "execvp");
    int ret = original_execvp(file, argv);

    // The exec() functions return only if an error has occurred
    // append the return value to the output
    // use fseek() to go back to the position of '\n'
    fseek(output, -1, SEEK_CUR);    
    fprintf(output, " = %d\n", ret);

    return ret;
}
int execv(const char *path, char *const argv[])
{
    ptrdiff_t i;

    fprintf(output, "%s %s('%s', ", OUTPUT_PREFIX, __func__, path);
    for(i = 0; argv[i] != NULL; i++) {
        fprintf(output, "'%s', ", argv[i]);
    }
    fprintf(output, "'%s')\n", argv[i]);

    // The exec() functions will replace original process, so explicit call fflush here to flush the output
    fflush(output);
    // The exec() functions will inherit environment variable, result in generate another monitor.out and override the original monitor.out
    // unset the LD_PRELOAD to avoid
    unsetenv("LD_PRELOAD");

    execv_t original_execv = (execv_t) dlsym(handle, "execv");
    int ret = original_execv(path, argv);

    // The exec() functions return only if an error has occurred
    // append the return value to the output
    // use fseek() to go back to the position of '\n'
    fseek(output, -1, SEEK_CUR);    
    fprintf(output, " = %d\n", ret);

    return ret;
}
int execve(const char *filename, char *const argv[], char *const envp[])
{
    ptrdiff_t i;

    fprintf(output, "%s %s('%s', ", OUTPUT_PREFIX, __func__, filename);
    // print command argument
    for(i = 0; argv[i] != NULL; i++) {
        fprintf(output, "'%s'", argv[i]);
        if(argv[i+1] != NULL)
            fprintf(output, ", ");
    }
    // print environment variable
    for(i = 0; envp[i] != NULL && i < MAX_ENVIRON_LINE; i++) 
        fprintf(output, ", '%s'", envp[i]);
    fprintf(output, ")\n");

    // The exec() functions will replace original process, so explicit call fflush here to flush the output
    fflush(output);
    // The exec() functions will inherit environment variable, result in generate another monitor.out and override the original monitor.out
    // unset the LD_PRELOAD to avoid
    unsetenv("LD_PRELOAD");

    execve_t original_execve = (execve_t) dlsym(handle, "execve");
    int ret = original_execve(filename, argv, __environ);

    // The exec() functions return only if an error has occurred
    // append the return value to the output
    // use fseek() to go back to the position of '\n'
    fseek(output, -1, SEEK_CUR);    
    fprintf(output, " = %d\n", ret);

    return ret;
}
int execvp(const char *file, char *const argv[])
{
    ptrdiff_t i;

    fprintf(output, "%s %s('%s', ", OUTPUT_PREFIX, __func__, file);

    for(i = 0; argv[i] != NULL; i++) {
        fprintf(output, "'%s', ", argv[i]);
    }
    fprintf(output, "'%s')\n", argv[i]);

    // The exec() functions will replace original process, so explicit call fflush here to flush the output
    fflush(output);
    // The exec() functions will inherit environment variable, result in generate another monitor.out and override the original monitor.out
    // unset the LD_PRELOAD to avoid
    unsetenv("LD_PRELOAD");

    execvp_t original_execvp = (execvp_t) dlsym(handle, "execvp");
    int ret = original_execvp(file, argv);

    // The exec() functions return only if an error has occurred
    // append the return value to the output
    // use fseek() to go back to the position of '\n'
    fseek(output, -1, SEEK_CUR);    
    fprintf(output, " = %d\n", ret);

    return ret;
}
int fchdir(int fd)
{
    // call original function
    fchdir_t original_fchdir = (fchdir_t) dlsym(handle, __func__);
    int ret = original_fchdir(fd);

    string filename = getFileNameByFd(fd);

    if(filename != "")
        fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, filename.c_str(), ret);
    else
        fprintf(output, "%s %s(%d) = %d\n", OUTPUT_PREFIX, __func__, fd, ret);

    return ret;
}
int fchown(int fd, uid_t owner, gid_t group)
{
    // call original function
    fchown_t original_fchown = (fchown_t) dlsym(handle, __func__);
    int ret = original_fchown(fd, owner, group);

    string filename = getFileNameByFd(fd);
    struct group *gr = getgrgid(group);
    struct passwd *pw = getpwuid(owner);

    if(filename != "")
        fprintf(output, "%s %s('%s', '%s', '%s') = %d\n", OUTPUT_PREFIX, __func__, filename.c_str(), pw->pw_name, gr->gr_name, ret);
    else
        fprintf(output, "%s %s(%d, '%s', '%s') = %d\n", OUTPUT_PREFIX, __func__, fd, pw->pw_name, gr->gr_name, ret);

    return ret;
}
pid_t fork(void)
{
    // flush the output first, or the child process will have the copy of output
    // result in duplicate line in output
    fflush(output);

    // call original function
    fork_t original_fork = (fork_t) dlsym(handle, __func__);
    int ret = original_fork();

    // close file if process is child (pid = 0)
    if(ret == 0)
        fclose(output);

    fprintf(output, "%s %s() = %d\n", OUTPUT_PREFIX, __func__, ret);
}
int fsync(int fd)
{
    // call original function
    fsync_t original_fsync = (fsync_t) dlsym(handle, __func__);
    int ret = original_fsync(fd);

    string filename = getFileNameByFd(fd);

    if(filename != "")
        fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, filename.c_str(), ret);
    else
        fprintf(output, "%s %s(%d) = %d\n", OUTPUT_PREFIX, __func__, fd, ret);

    return ret;
}
int ftruncate(int fd, off_t length)
{
    // call original function
    ftruncate_t original_ftruncate = (ftruncate_t) dlsym(handle, __func__);
    int ret = original_ftruncate(fd, length);

    string filename = getFileNameByFd(fd);

    if(filename != "")
        fprintf(output, "%s %s('%s', %jd) = %d\n", OUTPUT_PREFIX, __func__, filename.c_str(), length, ret);
    else
        fprintf(output, "%s %s(%d, %jd) = %d\n", OUTPUT_PREFIX, __func__, fd, length, ret);

    return ret;
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
    // call original function
    getegid_t original_getegid = (getegid_t) dlsym(handle, __func__);
    gid_t ret = original_getegid();

    struct group *gr = getgrgid(ret);

    fprintf(output, "%s %s() = %s\n", OUTPUT_PREFIX, __func__, gr->gr_name);

    return ret;
}
uid_t geteuid(void)
{
    // call original function
    geteuid_t original_geteuid = (geteuid_t) dlsym(handle, __func__);
    uid_t ret = original_geteuid();

    struct passwd *pw = getpwuid(ret);

    fprintf(output, "%s %s() = %s\n", OUTPUT_PREFIX, __func__, pw->pw_name);

    return ret;
}
gid_t getgid(void)
{
    // call original function
    getgid_t original_getgid = (getgid_t) dlsym(handle, __func__);
    gid_t ret = original_getgid();

    struct group *gr = getgrgid(ret);

    fprintf(output, "%s %s() = %s\n", OUTPUT_PREFIX, __func__, gr->gr_name);

    return ret;
}
uid_t getuid(void)
{
    // call original function
    getuid_t original_getuid = (getuid_t) dlsym(handle, __func__);
    uid_t ret = original_getuid();

    struct passwd *pw = getpwuid(ret);

    fprintf(output, "%s %s() = %s\n", OUTPUT_PREFIX, __func__, pw->pw_name);

    return ret;
}
int link(const char *oldpath, const char *newpath)
{
    // call original function
    link_t original_link = (link_t) dlsym(handle, __func__);
    int ret = original_link(oldpath, newpath);

    fprintf(output, "%s %s('%s', '%s') = %d\n", OUTPUT_PREFIX, __func__, oldpath, newpath, ret);

    return ret;
}
int pipe(int pipefd[2])
{
    // call original function
    pipe_t original_pipe = (pipe_t) dlsym(handle, __func__);
    int ret = original_pipe(pipefd);

    fprintf(output, "%s %s([%d, %d]) = %d\n", OUTPUT_PREFIX, __func__, pipefd[0], pipefd[1], ret);

    return ret;
}
ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    // call original function
    pread_t original_pread = (pread_t) dlsym(handle, __func__);
    ssize_t ret = original_pread(fd, buf, count, offset);

    string filename = getFileNameByFd(fd);

    if(filename != "")
        fprintf(output, "%s %s('%s', %p, %zu, %jd) = %zd\n", OUTPUT_PREFIX, __func__, filename.c_str(), buf, count, offset, ret);
    else
        fprintf(output, "%s %s(%d, %p, %zu, %jd) = %zd\n", OUTPUT_PREFIX, __func__, fd, buf, count, offset, ret);

    return ret;
}
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    // call original function
    pwrite_t original_pwrite = (pwrite_t) dlsym(handle, __func__);
    ssize_t ret = original_pwrite(fd, buf, count, offset);

    string filename = getFileNameByFd(fd);

    if(filename != "")
        fprintf(output, "%s %s('%s', %p, %zu, %jd) = %zd\n", OUTPUT_PREFIX, __func__, filename.c_str(), buf, count, offset, ret);
    else
        fprintf(output, "%s %s(%d, %p, %zu, %jd) = %zd\n", OUTPUT_PREFIX, __func__, fd, buf, count, offset, ret);

    return ret;
}
ssize_t read(int fd, void *buf, size_t count)
{
    // call original function
    read_t original_read = (read_t) dlsym(handle, __func__);
    ssize_t ret = original_read(fd, buf, count);

    string filename = getFileNameByFd(fd);

    if(filename != "")
        fprintf(output, "%s %s('%s', %p, %zu) = %zd\n", OUTPUT_PREFIX, __func__, filename.c_str(), buf, count, ret);
    else
        fprintf(output, "%s %s(%d, %p, %zu) = %zd\n", OUTPUT_PREFIX, __func__, fd, buf, count, ret);

    return ret;
}
ssize_t readlink(const char *path, char *buf, size_t bufsiz)
{
    // call original function
    readlink_t original_readlink = (readlink_t) dlsym(handle, __func__);
    ssize_t ret = original_readlink(path, buf, bufsiz);

    fprintf(output, "%s %s('%s', %p, %zu) = %zd\n", OUTPUT_PREFIX, __func__, path, buf, bufsiz, ret);

    return ret;
}
int rmdir(const char *pathname)
{
    // call original function
    rmdir_t original_rmdir = (rmdir_t) dlsym(handle, __func__);
    int ret = original_rmdir(pathname);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, pathname, ret);

    return ret;
}
int setegid(gid_t egid)
{
    // call original function
    setegid_t original_setegid = (setegid_t) dlsym(handle, __func__);
    int ret = original_setegid(egid);

    struct group *gr = getgrgid(egid);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, gr->gr_name, ret);

    return ret;
}
int seteuid(uid_t euid)
{
    // call original function
    seteuid_t original_seteuid = (seteuid_t) dlsym(handle, __func__);
    int ret = original_seteuid(euid);

    struct passwd *pw = getpwuid(euid);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, pw->pw_name, ret);

    return ret;
}
int setgid(gid_t gid)
{
    // call original function
    setgid_t original_setgid = (setgid_t) dlsym(handle, __func__);
    int ret = original_setgid(gid);

    struct group *gr = getgrgid(gid);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, gr->gr_name, ret);

    return ret;
}
int setuid(uid_t uid)
{
    // call original function
    setuid_t original_setuid = (setuid_t) dlsym(handle, __func__);
    int ret = original_setuid(uid);

    struct passwd *pw = getpwuid(uid);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, pw->pw_name, ret);

    return ret;
}
unsigned int sleep(unsigned int seconds)
{
    sleep_t original_sleep = (sleep_t) dlsym(handle, __func__);
    unsigned int ret = original_sleep(seconds);

    fprintf(output, "%s %s(%u) = %u\n", OUTPUT_PREFIX, __func__, seconds, ret);

    return 0;
}
int symlink(const char *oldpath, const char *newpath)
{
    // call original function
    symlink_t original_symlink = (symlink_t) dlsym(handle, __func__);
    int ret = original_symlink(oldpath, newpath);

    fprintf(output, "%s %s('%s', '%s') = %d\n", OUTPUT_PREFIX, __func__, oldpath, newpath, ret);

    return ret;
}
int unlink(const char *path)
{
    // call original function
    unlink_t original_unlink = (unlink_t) dlsym(handle, __func__);
    int ret = original_unlink(path);

    fprintf(output, "%s %s('%s') = %d\n", OUTPUT_PREFIX, __func__, path, ret);

    return ret;
}
ssize_t write(int fd, const void *buf, size_t nbyte)
{
    // call original function
    write_t original_write = (write_t) dlsym(handle, __func__);
    ssize_t ret = original_write(fd, buf, nbyte);

    string filename = getFileNameByFd(fd);

    if(filename != "")
        fprintf(output, "%s %s('%s', %p, %zu) = %zd\n", OUTPUT_PREFIX, __func__, filename.c_str(), buf, nbyte, ret);
    else
        fprintf(output, "%s %s(%d, %p, %zu) = %zd\n", OUTPUT_PREFIX, __func__, fd, buf, nbyte, ret);

    return ret;
}
int chmod(const char *path, mode_t mode)
{
    // call original function
    chmod_t original_chmod = (chmod_t) dlsym(handle, __func__);
    int ret = original_chmod(path, mode);

    fprintf(output, "%s %s('%s', %o) = %d\n", OUTPUT_PREFIX, __func__, path, mode, ret);

    return ret;
}
int fchmod(int fd, mode_t mode)
{
    // call original function
    fchmod_t original_fchmod = (fchmod_t) dlsym(handle, __func__);
    int ret = original_fchmod(fd, mode);

    string filename = getFileNameByFd(fd);

    if(filename != "")
        fprintf(output, "%s %s('%s', %o) = %d\n", OUTPUT_PREFIX, __func__, filename.c_str(), mode, ret);
    else
        fprintf(output, "%s %s(%d, %o) = %d\n", OUTPUT_PREFIX, __func__, fd, mode, ret);

    return ret;
}
int __fxstat(int ver, int fd, struct stat *buf)
{
    // call original function
    __fxstat_t original___fxstat = (__fxstat_t) dlsym(handle, __func__);
    int ret = original___fxstat(ver, fd, buf);

    string filename = getFileNameByFd(fd);
    dev_t dev = buf->st_dev;     
    ino_t ino = buf->st_ino;     
    mode_t mode = buf->st_mode;    
    nlink_t nlink = buf->st_nlink;   
    uid_t uid = buf->st_uid;   
    gid_t gid = buf->st_gid;    
    dev_t rdev = buf->st_rdev;    
    off_t size = buf->st_size;    
    blksize_t blksize = buf->st_blksize; 
    blkcnt_t blocks = buf->st_blocks; 
    time_t mtime = buf->st_mtime;  
    time_t ctime = buf->st_ctime;  

    if(filename != "")
        fprintf(output, "%s %s('%s', (dev=%zu, inode=%zu, mode=%o, nlink=%zu, uid=%d, gid=%d, rdev=%zu, size=%jd, blksize=%zu, blocks=%zu, mtime=%zu, stime=%zu)) = %d\n", 
                        OUTPUT_PREFIX, "fstat", filename.c_str(), dev, ino, mode, nlink, uid, gid, rdev, size, blksize, blocks, mtime, ctime, ret);
    else
        fprintf(output, "%s %s(%d, (dev=%zu, inode=%zu, mode=%o, nlink=%zu, uid=%d, gid=%d, rdev=%zu, size=%jd, blksize=%zu, blocks=%zu, mtime=%zu, stime=%zu)) = %d\n", 
                        OUTPUT_PREFIX, "fstat", fd, dev, ino, mode, nlink, uid, gid, rdev, size, blksize, blocks, mtime, ctime, ret);

    return ret;
}
int __lxstat(int ver, const char *path, struct stat *buf)
{
    // call original function
    __lxstat_t original___lxstat = (__lxstat_t) dlsym(handle, __func__);
    int ret = original___lxstat(ver, path, buf);

    dev_t dev = buf->st_dev;     
    ino_t ino = buf->st_ino;     
    mode_t mode = buf->st_mode;    
    nlink_t nlink = buf->st_nlink;   
    uid_t uid = buf->st_uid;   
    gid_t gid = buf->st_gid;    
    dev_t rdev = buf->st_rdev;    
    off_t size = buf->st_size;    
    blksize_t blksize = buf->st_blksize; 
    blkcnt_t blocks = buf->st_blocks; 
    time_t mtime = buf->st_mtime;  
    time_t ctime = buf->st_ctime;  

    fprintf(output, "%s %s('%s', (dev=%zu, inode=%zu, mode=%o, nlink=%zu, uid=%d, gid=%d, rdev=%zu, size=%jd, blksize=%zu, blocks=%zu, mtime=%zu, stime=%zu)) = %d\n", 
                    OUTPUT_PREFIX, "lstat", path, dev, ino, mode, nlink, uid, gid, rdev, size, blksize, blocks, mtime, ctime, ret);

    return ret;
}
int mkdir(const char *pathname, mode_t mode)
{
    // call original function
    mkdir_t original_mkdir = (mkdir_t) dlsym(handle, __func__);
    int ret = original_mkdir(pathname, mode);

    fprintf(output, "%s %s('%s', %o) = %d\n", OUTPUT_PREFIX, __func__, pathname, mode, ret);

    return ret;
}
int mkfifo(const char *pathname, mode_t mode)
{
    // call original function
    mkfifo_t original_mkfifo = (mkfifo_t) dlsym(handle, __func__);
    int ret = original_mkfifo(pathname, mode);

    fprintf(output, "%s %s('%s', %o) = %d\n", OUTPUT_PREFIX, __func__, pathname, mode, ret);

    return ret;
}
int __xstat(int ver, const char *path, struct stat *buf)
{
    // call original function
    __xstat_t original__xstat = (__xstat_t) dlsym(handle, __func__);
    int ret = original__xstat(ver, path, buf);

    dev_t dev = buf->st_dev;     
    ino_t ino = buf->st_ino;     
    mode_t mode = buf->st_mode;    
    nlink_t nlink = buf->st_nlink;   
    uid_t uid = buf->st_uid;   
    gid_t gid = buf->st_gid;    
    dev_t rdev = buf->st_rdev;    
    off_t size = buf->st_size;    
    blksize_t blksize = buf->st_blksize; 
    blkcnt_t blocks = buf->st_blocks; 
    time_t mtime = buf->st_mtime;  
    time_t ctime = buf->st_ctime;  

    fprintf(output, "%s %s('%s', (dev=%zu, inode=%zu, mode=%o, nlink=%zu, uid=%d, gid=%d, rdev=%zu, size=%jd, blksize=%zu, blocks=%zu, mtime=%zu, stime=%zu)) = %d\n", 
                    OUTPUT_PREFIX, "stat", path, dev, ino, mode, nlink, uid, gid, rdev, size, blksize, blocks, mtime, ctime, ret);

    return ret;
}
mode_t umask(mode_t mask)
{
    // call original function
    umask_t original_umask = (umask_t) dlsym(handle, __func__);
    mode_t ret = original_umask(mask);

    fprintf(output, "%s %s(%o) = %o\n", OUTPUT_PREFIX, __func__, mask, ret);

    return ret;
}

} // extern "C"