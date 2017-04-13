#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>


#define MAXLINE 1024

int main(int argc, char *argv[], char** envp)
{
	char buffer[MAXLINE];
	char buf[MAXLINE];
	char template1[] = "/tmp/tmpdir.XXXXXX";
	char template2[] = "/tmp/tmp.XXXXXX";
	unsigned int seed = time(NULL);
	char *command[] = {"ls", "-al", "./", NULL};
	int pipefd[2];
	struct stat statbuf;
	struct dirent *ep = (struct dirent*) malloc(1000);
  	struct dirent *result = NULL;
  	FILE *file;
  	DIR *d;

  	mkdir("./tempdir", 0777);
  	d = opendir("./tempdir");
  	fdopendir(dirfd(d));
  	readdir(d);
  	readdir_r(d, ep, &result);
  	rewinddir(d);
 	seekdir(d, 0);
  	telldir(d);
  	closedir(d);

	creat("temp", 0755);
	open("temp", O_RDONLY);
	rename("./temp", "./temp2");
	file = fopen("./temp2", "rw");
	setbuf(file, NULL);
	setvbuf(file, NULL, _IOFBF, 1024);
	close(fileno(file));
	fclose(file);

	tempnam("/tmp", "tempXXXXXX");
	tmpfile();
	tmpnam(buffer);
	mkdtemp(template1);
 	mkstemp(template2);

	getenv("HOME");
	putenv("TEST=test");
	getenv("TEST");
	setenv("TEST", "test2", 1);
	getenv("TEST");

	srand(time(NULL));
	rand();
	rand_r(&seed);

	chdir("../123");
	getcwd(buf, sizeof(buf));
	chown("./temp2", 1000, 1000);

	pipe(pipefd);
 	dup(pipefd[0]);
	dup2(1, pipefd[1]);

	fchdir(10);
	fchown(fileno(file), 1000, 1000);
	fsync(fileno(file));
	ftruncate(fileno(file), 0);

	getegid();
	geteuid();
	getgid();
	getuid();
	setegid(1000);
 	seteuid(1000);
 	setgid(1000);
 	setuid(1000);

	link("./temp2", "./temp2_link");
	symlink("./temp2", "./temp2_slink");
	readlink("./temp2_slink", buf, 0);
	unlink("./temp2_link");

 	pread(fileno(file), buf, 0, 0);
 	pwrite(fileno(file), buf, 0, 0);
 	read(fileno(file), buf, 0);
 	write(fileno(file), buf, 0);

 	mkfifo("./myfifo", 0777);
 	remove("./myfifo");

 	chmod("./temp2", 0777);
 	fchmod(fileno(file), 0777);
 	umask(0777);

 	fstat(fileno(file), &statbuf);
 	lstat("./temp2_slink", &statbuf);
 	stat("./temp2", &statbuf);
 	unlink("./temp2_slink");
 	remove("./temp2");
 	rmdir("./tempdir");

 	// execl("/bin/ls", "ls", "-al", "./", (char *)0);
 	// execle("/bin/ls", "ls", "-al", "./", (char *)0, envp);
 	// execlp("ls", "ls", "-al", "./", (char *)0);
 	// execv("/bin/ls", command);
 	// execvp("ls", command);
 	// execve("/bin/ls", command, envp);

 	// fflush(stdout);
 	// char *s = realloc(NULL, 100);

	// exit(0);
	// _exit(0);
	// sleep(1);
	// fork();
	// system("date");

	return 0;
}
