#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <getopt.h> 
#include <fcntl.h> 
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024

using namespace std;

void printUsage()
{
	printf("Usage: ./hw1 [-t|--tcp] [-u|--udp] [filter-string]\n"); 
}

bool isNumber(const string& str) 
{
	return str.find_first_not_of("0123456789") == string::npos;
}

string ipv6_hex_to_string(string ip_hex)
{
	struct in6_addr tmp_ip;
	char ip_str[128] = {0};

	if (sscanf(ip_hex.c_str(),
		"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
		&tmp_ip.s6_addr[3], &tmp_ip.s6_addr[2], &tmp_ip.s6_addr[1], &tmp_ip.s6_addr[0],
		&tmp_ip.s6_addr[7], &tmp_ip.s6_addr[6], &tmp_ip.s6_addr[5], &tmp_ip.s6_addr[4],
		&tmp_ip.s6_addr[11], &tmp_ip.s6_addr[10], &tmp_ip.s6_addr[9], &tmp_ip.s6_addr[8],
		&tmp_ip.s6_addr[15], &tmp_ip.s6_addr[14], &tmp_ip.s6_addr[13], &tmp_ip.s6_addr[12]) == 16) 
	{
		inet_ntop(AF_INET6, &tmp_ip, ip_str, sizeof(ip_str));
	}

	return string(ip_str);
}

string getProcessNameByPid(const string& pid)
{
  	string name;
	string path = "/proc/" + pid + "/comm";  
  	fstream file;

  	file.open(path.c_str(), ios::in);

  	if (file) 
  		file >> name;

	file.close();

	return name;
}

string getCmdlineByPid(const string& pid)
{
	string path = "/proc/" + pid + "/cmdline"; 
	string cmdline; 

	char buffer[MAXLINE]; 
	int fd = open(path.c_str(), O_RDONLY);
	int nbytesread = read(fd, buffer, MAXLINE);
    char *end = buffer + nbytesread;

	for (char *p = buffer; p < end;) {
		string temp = string(p);
		cmdline = cmdline + " " + temp;
	  	while (*p++); 
	}
	close(fd);

	string delimiter = getProcessNameByPid(pid);
	int pos;

	// strip process name if argument already contain process name
	if((pos = cmdline.find(delimiter)) != string::npos) {
		cmdline.erase(0, pos + delimiter.length());
	}

	return cmdline;
}

string getPidByInode(const string& inode)
{
	DIR *proc_dir = opendir("/proc");
  	struct dirent *entry;  

  	if (proc_dir != NULL)
  	{
	    while (entry = readdir(proc_dir))
	    {
		  	if(isNumber(entry->d_name))
		  	{
		  		DIR *fd_dir;    
			  	string pid(entry->d_name);
			  	string path = "/proc/" + pid + "/fd";

			  	fd_dir = opendir(path.c_str());

			  	if (fd_dir != NULL) 
			  	{
			  		struct dirent *entry; 

			  		while (entry = readdir(fd_dir)) 
			  		{
			  			char line[MAXLINE] = {0};
			  			string file(entry->d_name);
			  			string path = "/proc/" + pid + "/fd/" + file;

			  			// check if link contain target inode
			  			if (readlink(path.c_str(), line, sizeof(line)) != -1) {
			  				if (strstr(line, inode.c_str()) != NULL)
			  					return pid;
			  			}
			  		}
			  	}
			  	else 
			  	{
			   		char error[MAXLINE];
			   		snprintf(error, sizeof(error), "Failed to open /proc/%s/fd", pid.c_str());
			   		perror(error);
			   		exit(errno);
			  	}

			  	closedir(fd_dir);
	  		}
	  	}

	    closedir(proc_dir);
  	}
  	else 
  	{
   		perror("Failed to open /proc");
   		exit(errno);
  	}
}

void printIpv4Connection(const string& type)
{
	string path = "/proc/net/" + type;
	FILE *fp = fopen(path.c_str(), "r");;

	if(fp) 
	{
	    char line[MAXLINE];
	    char src_ip_hex[9], src_port_hex[5], dst_ip_hex[9], dst_port_hex[5];
	    char src_ip_port[22], dst_ip_port[22];
	    char inode[20];
	    unsigned int ip[4];
	    unsigned int port;

	    // ignore first line
	    fgets(line, sizeof(line), fp);

	    while(fscanf(fp, "%*s %[^:]:%s%[^:]:%s%*s%*s%*s%*s%*s%*s%s%*[^\n]\n" \
	   	, src_ip_hex, src_port_hex, dst_ip_hex, dst_port_hex, inode) != -1) 
	   	{
			sscanf(src_ip_hex, "%2x%2x%2x%2x", &ip[0], &ip[1], &ip[2], &ip[3]);
			sscanf(src_port_hex, "%4x", &port);
		    snprintf(src_ip_port, sizeof(src_ip_port), "%u.%u.%u.%u:%u", ip[3], ip[2], ip[1], ip[0], port);

			sscanf(dst_ip_hex, "%2x%2x%2x%2x", &ip[0], &ip[1], &ip[2], &ip[3]);
			sscanf(dst_port_hex, "%4x", &port);

			// if port = 0, print "*" instead of 0
			if(port == 0)
				snprintf(dst_ip_port, sizeof(dst_ip_port), "%u.%u.%u.%u:*", ip[0], ip[1], ip[2], ip[3]);
			else
				snprintf(dst_ip_port, sizeof(dst_ip_port), "%u.%u.%u.%u:%u", ip[3], ip[2], ip[1], ip[0], port);

		    string inode_str(inode);
		    string pid = getPidByInode(inode_str);
		    string process = getProcessNameByPid(pid);
		    string cmdline = getCmdlineByPid(pid);

	        printf("%-5s %-25s %-25s %s/%s%s\n", type.c_str(), src_ip_port, dst_ip_port, pid.c_str(), process.c_str(), cmdline.c_str()); 
	    }
	}
	else
	{
		char error[MAXLINE];
   		snprintf(error, sizeof(error), "Failed to open /proc/net/%s", type.c_str());
   		perror(error);
   		exit(errno);
	}

	fclose(fp);
}

void printIpv6Connection(const string& type)
{
	string v6_type = type + "6";
	string path = "/proc/net/" + v6_type;
	FILE *fp = fopen(path.c_str(), "r");;

	if(fp) 
	{
	    char line[MAXLINE];
	    char src_ip_hex[33], src_port_hex[5], dst_ip_hex[33], dst_port_hex[5];
	    char src_ip_port[46], dst_ip_port[46];
	    char inode[20];
	    unsigned int ip[16];
	    unsigned int port;

	    // ignore first line
	    fgets(line, sizeof(line), fp);

	    while(fscanf(fp, "%*s %[^:]:%s%[^:]:%s%*s%*s%*s%*s%*s%*s%s%*[^\n]\n" \
	   	, src_ip_hex, src_port_hex, dst_ip_hex, dst_port_hex, inode) != -1) 
	   	{
	   		string src_ip = ipv6_hex_to_string(src_ip_hex);
			sscanf(src_port_hex, "%4x", &port);
		    snprintf(src_ip_port, sizeof(src_ip_port), "%s:%u", src_ip.c_str(), port);

			string dst_ip = ipv6_hex_to_string(dst_ip_hex);
			sscanf(dst_port_hex, "%4x", &port);

			// if port = 0, print "*" instead of 0
			if(port == 0)
				snprintf(dst_ip_port, sizeof(dst_ip_port), "%s:*", dst_ip.c_str());
			else
				snprintf(dst_ip_port, sizeof(dst_ip_port), "%s:%u", dst_ip.c_str(), port);

		    string inode_str(inode);
		    string pid = getPidByInode(inode_str);
		    string process = getProcessNameByPid(pid);
		    string cmdline = getCmdlineByPid(pid);

	        printf("%-5s %-25s %-25s %s/%s%s\n", v6_type.c_str(), src_ip_port, dst_ip_port, pid.c_str(), process.c_str(), cmdline.c_str()); 
	    }
	}
	else
	{
		char error[MAXLINE];
   		snprintf(error, sizeof(error), "Failed to open /proc/net/%s", v6_type.c_str());
   		perror(error);
   		exit(errno);
	}

	fclose(fp);
}

int main(int argc, char *argv[])  
{  
	int option;  
    const char *short_options = "tu";  
	const struct option long_options[] = {  
		{ "tcp", 0, NULL, 't' },  
		{ "udp", 0, NULL, 'u' },  
		{ 0, 0, 0, 0 },  
	}; 
	bool tcp_flag = false;
	bool udp_flag = false;
	bool invalid_flag = false;

	// no argument is passed
	if(argc == 1) {
		tcp_flag = true;
		udp_flag = true;
	}

	while ((option = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) 
	{  
		switch (option) 
		{  
			case 't':  
				tcp_flag = true;
				break;  
			case 'u':  
				udp_flag = true;
				break;  
			case '?':  
				invalid_flag = true;
				break;  
		}  
	}

	if(invalid_flag) {
		printUsage();
		return 0;
	}
	if(tcp_flag) {
		printf("List of TCP connections:\n");
		printf("%-5s %-25s %-25s %-10s\n", "Proto", "Local Address", "Foreign Address", "PID/Program name and arguments");
		printIpv4Connection("tcp");
		printIpv6Connection("tcp");
		printf("\n");
	}
	if(udp_flag) {
		printf("List of UDP connections:\n");
		printf("%-5s %-25s %-25s %-10s\n", "Proto", "Local Address", "Foreign Address", "PID/Program name and arguments");
		printIpv4Connection("udp");
		printIpv6Connection("udp");
		printf("\n");
	}

	return 0;  
}  
