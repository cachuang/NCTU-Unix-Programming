#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	open("/etc/services", 0);
	
	return 0;
}
