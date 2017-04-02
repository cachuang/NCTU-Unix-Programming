#include "hijack.h"

void *handle;

static void init(void)
{
    handle = dlopen("libc.so.6", RTLD_LAZY);
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

}
int open(const char *pathname, int flags)
{
	printf("====== %s %d =======\n", pathname, flags);

    open_t original_open = (open_t) dlsym(handle, "open");
    original_open(pathname, flags);
}

int remove(const char *pathname)
{

}
int rename(const char *oldpath, const char *newpath)
{

}
void setbuf(FILE *stream, char *buf)
{

}
int setvbuf(FILE *stream, char *buf, int mode, size_t size)
{

}

char* tempnam(const char *dir, const char *pfx)
{

}

FILE* tmpfile(void)
{

}
char* tmpnam(char *s)
{

}
void exit(int status)
{
    exit_t original_exit = (exit_t) dlsym(handle, "exit");
    original_exit(status);
}
char* getenv(const char *name)
{

}
char* mkdtemp(char *emplate)
{

}
int mkstemp(char *emplate)
{

}
int putenv(char *string)
{

}
int rand(void)
{

}
int rand_r(unsigned int *seedp)
{

}
int setenv(const char *name, const char *value, int overwrite)
{

}
void srand(unsigned int seed)
{

}
int system(const char *command)
{

}
int chdir(const char *path)
{

}
int chown(const char *path, uid_t owner, gid_t group)
{

}
int close(int fd)
{

}
int dup(int oldfd)
{

}
int dup2(int oldfd, int newfd)
{

}
void _exit(int status)
{
    _exit_t original__exit = (_exit_t) dlsym(handle, "_exit");
    original__exit(status);
}
int execl(const char *path, const char *arg, ...)
{

}
int execle(const char *path, const char *arg, ...)
{

}
int execlp(const char *file, const char *arg, ...)
{

}
int execv(const char *path, char *const argv[])
{

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