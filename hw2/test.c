#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define MAXLINE 1024

int main()
{
	char buffer[L_tmpnam];
	char buf[MAXLINE];
	char template1[] = "/tmp/tmpdir.XXXXXX";
	char template2[] = "/tmp/tmp.XXXXXX";
	int seed = time(NULL);

	creat("temp", 0755);
	open("temp", O_RDONLY);
	rename("./temp", "./temp2");
	FILE *file = fopen("./temp2", "rw");
	setbuf(file, NULL);
	setvbuf(file, NULL, _IOFBF, 1024);
	fclose(file);
	remove("./temp2");
	tempnam("/tmp", "tempnam_test");
	tmpfile();
	tmpnam(buffer);
	getenv("HOME");
 	mkdtemp(template1);
 	mkstemp(template2);
 	putenv("TEST=test");
 	getenv("TEST");
 	rand();
 	rand_r(&seed);
 	setenv("TEST", "test2", 1);
 	getenv("TEST");
 	srand(time(NULL));
 	//system("date");
 	//chdir("../");
 	getcwd(buf, sizeof(buf));
 	chown("./temp2", 1000, 1000);
 	// close(10);
 	// dup(10);
 	// dup2(10, 11);
 	execl("/bin/pwd", "pwd", (char *)0);

	return 0;
}
