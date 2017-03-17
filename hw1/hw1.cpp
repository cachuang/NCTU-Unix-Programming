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
#include <regex.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024

using namespace std;

bool tcp_flag = false;
bool udp_flag = false;
bool filter_flag = false;
bool invalid_flag = false;
string filter_string;

void printUsage()
{
	printf("Usage: ./hw1 [-t|--tcp] [-u|--udp] [filter-string]\n"); 
}

bool isNumber(const string& str) 
{
	return str.find_first_not_of("0123456789") == string::npos;
}

bool matchRegex(const string& target, const string& pattern)
{
	regex_t regex;
	int status;
	bool result = false;

	if(regcomp(&regex, pattern.c_str(), REG_NOSUB | REG_EXTENDED) != 0) {
		// Compile regex failed
		result = false;
	}
	status = regexec(&regex, target.c_str(), 0, NULL, 0);

	if (status == 0) {
		// Match
		result = true;
	}
	else {
		// Not match or regexec() failed 
		result = false;
	}

	regfree(&regex);

	return result;
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
		int pos;
		// strip path of the process name
		if(p == buffer) {
			if((pos = temp.find_last_of("/")) != string::npos) {
				temp.erase(0, pos+1);
			}
		}
		cmdline = cmdline + temp + " ";
	  	while (*p++); 
	}
	close(fd);

	return cmdline;
}

string getPidByInode(const char* inode)
{
	DIR *proc_dir = opendir("/proc");
  	struct dirent *entry;  

  	if (proc_dir != NULL)
  	{
	    while (entry = readdir(proc_dir))
	    {
		  	if (isNumber(entry->d_name))
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
			  			struct stat file_status;

			  			stat(path.c_str(), &file_status);

			  			// check if socket's inode equals to target inode
			  			if(S_ISSOCK(file_status.st_mode)) {
				  			if (readlink(path.c_str(), line, sizeof(line)) != -1) {
				  				if (strstr(line, inode) != NULL)
				  					return pid;
				  			}
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
	    unsigned int src_ip_hex, src_port_hex, dst_ip_hex, dst_port_hex;
	    char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
	    char src_ip_port[22], dst_ip_port[22];
	    char line[MAXLINE];
	    char inode[MAXLINE];

	    // Ignore first line
	    fgets(line, sizeof(line), fp);

	    while(fscanf(fp, "%*s%x:%x%x:%x%*s%*s%*s%*s%*s%*s%s%*[^\n]\n" \
	   	      , &src_ip_hex, &src_port_hex, &dst_ip_hex, &dst_port_hex, inode) != -1) 
	    {
			inet_ntop(AF_INET, &src_ip_hex, src_ip, sizeof(src_ip));
		    snprintf(src_ip_port, sizeof(src_ip_port), "%s:%d", src_ip, src_port_hex);

			inet_ntop(AF_INET, &dst_ip_hex, dst_ip, sizeof(dst_ip));
			// If destination port = 0, print "*" instead of 0
			if(dst_port_hex == 0)
		    	snprintf(dst_ip_port, sizeof(dst_ip_port), "%s:*", dst_ip);
			else
		    	snprintf(dst_ip_port, sizeof(dst_ip_port), "%s:%d", dst_ip, dst_port_hex);

		    string pid = getPidByInode(inode);
		    string cmdline = getCmdlineByPid(pid);

		    if(!filter_flag || (filter_flag && matchRegex(cmdline, filter_string)))
	        	printf("%-5s %-25s %-25s %s/%s\n", type.c_str(), src_ip_port, dst_ip_port, pid.c_str(), cmdline.c_str()); 
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

		    string pid = getPidByInode(inode);
		    string cmdline = getCmdlineByPid(pid);

		    if(!filter_flag || (filter_flag && matchRegex(cmdline, filter_string)))
	        	printf("%-5s %-25s %-25s %s/%s\n", v6_type.c_str(), src_ip_port, dst_ip_port, pid.c_str(), cmdline.c_str()); 
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

		if(invalid_flag) 
			break; 
	}

	// no argument or only filter string is passed
	if (argc == 1 || (argc == 2 && argv[optind] != NULL)) {
		tcp_flag = udp_flag = true;
	}
	// filter string more than one
	if (optind < argc-1 && !invalid_flag) {
		printf("%s: Too many arguments\n", argv[0]);
		printUsage();
		return 0;
	}

	if (invalid_flag) {
		printUsage();
		return 0;
	}
	if (argv[optind] != NULL) {
		filter_flag = true;
		filter_string = string(argv[optind]);
	}
	if (tcp_flag) {
		printf("List of TCP connections:\n");
		printf("%-5s %-25s %-25s %-10s\n", "Proto", "Local Address", "Foreign Address", "PID/Program name and arguments");
		printIpv4Connection("tcp");
		printIpv6Connection("tcp");
		printf("\n");
	}
	if (udp_flag) {
		printf("List of UDP connections:\n");
		printf("%-5s %-25s %-25s %-10s\n", "Proto", "Local Address", "Foreign Address", "PID/Program name and arguments");
		printIpv4Connection("udp");
		printIpv6Connection("udp");
		printf("\n");
	}

	return 0;  
}  
