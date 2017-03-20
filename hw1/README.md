# Homework #1

### Implement a 'netstat -nap'-like program

In this homework, you have to implement a 'netstat -nap' tool by yourself. You have to list all the existing TCP and UDP connections. For each identified connection (socket descriptor), find the corresponding process name and its command lines that creates the connection (socket descriptor). **You have to implement all the features by yourself and cannot make calls to the system built-in netstat program nor parse output from 'netstat -nap'**. Your codes must be implemented in C and/or C++.

To provide more flexibilities, your program have to accept several predefined options, including

- -t or --tcp: list only TCP connections.
- -u or --udp: list only UDP connections.
- An optional string to list only command lines that containing the string.

You have to handle the additional options using getopt_long function. In short, the synopsis of homework #1 would be:

``` sh
$ ./hw1 [-t|--tcp] [-u|--udp] [filter-string]
```

When no argument is passed, your program should output all identified connections. You may test your program with a root account so that your program would be able to access /proc files owned by other users.

### Hints

We have some hints for you to implement this homework.

- Look at the two files /proc/network/tcp and /proc/network/udp, and google for relevant information. Well, the file content is not difficult to understand.
- Read files in /proc/[pid]/fd
- Traverse all /proc/[pid]/fd directories and identify socket descriptors. The socket descriptors are actually symbolic links point to 'socket:[inode]' or '[0000]:inode', where inode is the corresponding inode number used in /proc/network/tcp and /proc/network/udp.
- You may need to work with opendir(3), readdir(3), stat(2), and readlink(2)
- For address conversion, you may consider working with inet_ntop(3) function. You may also want to include <netinet/in.h> for in_addr and in6_addr data structure. For how addresses are shown to the users, you may have a look at tcp and tcp6 liunx kernel codes. You can then fill in_addr and in6_addr data structure.
- For the ease of debugging, some sample addresses are shown for you (works on little-endian machines, e.g., i386 and x86_64).
- IPv4 address: 017AA8C0 <===> 192.168.122.1
- IPv6 address: BACD0120000000000000000052965732 <===> 2001:cdba::3257:9652
- IPv6 address: 0000000000000000FFFF0000BF00A8C0 <===> ::ffff:192.168.0.191

### Samples

#### Run the command without any argument

``` sh
$ ./hw1
List of TCP connections:
Proto Local Address           Foreign Address         PID/Program name and arguments
tcp   0.0.0.0:993             0.0.0.0:*               2472/dovecot    
tcp   0.0.0.0:60546           0.0.0.0:*               764/rpc.statd   
tcp   0.0.0.0:995             0.0.0.0:*               2472/dovecot    
tcp   127.0.0.1:3306          0.0.0.0:*               2684/mysqld     
tcp   0.0.0.0:110             0.0.0.0:*               2472/dovecot    
tcp   0.0.0.0:143             0.0.0.0:*               2472/dovecot    
tcp   0.0.0.0:111             0.0.0.0:*               742/rpcbind     
tcp   0.0.0.0:465             0.0.0.0:*               2827/master     
tcp   192.168.1.254:53        0.0.0.0:*               30495/named     
tcp   127.0.0.1:53            0.0.0.0:*               30495/named     
tcp   10.0.3.1:53             0.0.0.0:*               2580/dnsmasq    
tcp   127.0.0.1:631           0.0.0.0:*               27868/cupsd     
tcp   127.0.0.1:953           0.0.0.0:*               30495/named     
tcp   0.0.0.0:25              0.0.0.0:*               2827/master     
tcp   0.0.0.0:1723            0.0.0.0:*               2845/pptpd      
tcp   140.113.1.1:50275       140.112.172.4:23        16993/telnet ptt.cc
tcp6  :::993                  :::*                    2472/dovecot    
tcp6  :::995                  :::*                    2472/dovecot    
tcp6  :::2122                 :::*                    2475/sshd       
tcp6  :::110                  :::*                    2472/dovecot    
tcp6  :::143                  :::*                    2472/dovecot    
tcp6  :::111                  :::*                    742/rpcbind     
tcp6  :::80                   :::*                    4358/apache2    
tcp6  :::465                  :::*                    2827/master     
tcp6  :::53                   :::*                    30495/named     
tcp6  :::33173                :::*                    764/rpc.statd   
tcp6  ::1:631                 :::*                    27868/cupsd     
tcp6  ::1:953                 :::*                    30495/named     
tcp6  :::25                   :::*                    2827/master     
tcp6  :::443                  :::*                    4358/apache2    
tcp6  ::1:53859               ::1:631                 2608/cups-browsed

List of UDP connections:
Proto Local Address           Foreign Address         PID/Program name and arguments
udp   0.0.0.0:5353            0.0.0.0:*               1071/avahi-daemon: 
udp   0.0.0.0:56307           0.0.0.0:*               764/rpc.statd   
udp   0.0.0.0:41678           0.0.0.0:*               1071/avahi-daemon: 
udp   0.0.0.0:12491           0.0.0.0:*               2559/dhcpd      
udp   10.0.3.1:53             0.0.0.0:*               30495/named     
udp   192.168.1.254:53        0.0.0.0:*               30495/named     
udp   127.0.0.1:53            0.0.0.0:*               30495/named     
udp   10.0.3.1:53             0.0.0.0:*               2580/dnsmasq    
udp   0.0.0.0:67              0.0.0.0:*               2559/dhcpd      
udp   0.0.0.0:67              0.0.0.0:*               2580/dnsmasq    
udp   0.0.0.0:111             0.0.0.0:*               742/rpcbind     
udp   0.0.0.0:631             0.0.0.0:*               2608/cups-browsed
udp   0.0.0.0:913             0.0.0.0:*               742/rpcbind     
udp   127.0.0.1:940           0.0.0.0:*               764/rpc.statd   
udp6  :::5353                 :::*                    1071/avahi-daemon: 
udp6  :::59492                :::*                    764/rpc.statd   
udp6  :::53                   :::*                    30495/named     
udp6  :::111                  :::*                    742/rpcbind     
udp6  :::913                  :::*                    742/rpcbind     
udp6  :::34193                :::*                    1071/avahi-daemon: 
udp6  :::18161                :::*                    2559/dhcpd    
```
  
#### Run the command with --tcp

``` sh
$ ./hw1 --tcp
(only show TCP connections)
Run the command with --tcp and a filter string

$ ./hw1 --tcp telnet
List of TCP connections:
Proto Local Address           Foreign Address         PID/Program name and arguments
tcp   140.113.1.1:50275       140.112.172.4:23        16993/telnet ptt.cc
```