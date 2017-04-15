# Homework #2

### Hi! Jack ... Hijack!?

In this homework, we are going to practice library injection and API hijacking. Please implement a "library call monitor" (LCM) program that is able to show the activities of an arbitrary binary running on a Linux operating system. You have to implement your LCM as a shared library and inject the shared library into a process using using LD_PRELOAD. You have to dump the library calls as well as a summary of passed parameters. Please monitor at least the functions listed in the section "Minimum Requirements" below. The result should be stored into a filename, e.g., **"monitor.out"**. You may write the output to a filename specified by an environment variable **"MONITOR_OUTPUT"**. If the value of **MONITOR_OUTPUT** is **stderr**, output the messages to standart error instead of a file.

You have to compile your source codes and generate a shared object. You don't have to implement any monitored program by yourself. Instead, you must work with those binaries already installed in the system.

### Minimum Requirements

The minimum list of monitored library calls is shown below. It covers almost all the functions we have introduced in the class.

|  |  |  |    |  |  |   |  |
|:--------:|:---------:|:-------:|:---------:|:---------:|:---------:|:--------:|:-------:|
| closedir | fdopendir | opendir |  readdir  | readdir_r | rewinddir |  seekdir | telldir |
|   creat  |    open   |  remove |   rename  |   setbuf  |  setvbuf  |  tempnam | tmpfile |
|  tmpnam  |    exit   |  getenv |  mkdtemp  |  mkstemp  |   putenv  |   rand   |  rand_r |
|  setenv  |   srand   |  system |   chdir   |   chown   |   close   |    dup   |   dup2  |
|   _exit  |   execl   |  execle |   execlp  |   execv   |   execve  |  execvp  |  fchdir |
|  fchown  |    fork   |  fsync  | ftruncate |   getcwd  |  getegid  |  geteuid |  getgid |
|  getuid  |    link   |   pipe  |   pread   |   pwrite  |    read   | readlink |  rmdir  |
|  setegid |  seteuid  |  setgid |   setuid  |   sleep   |  symlink  |  unlink  |  write  |
|   chmod  |   fchmod  |  fstat  |   lstat   |   mkdir   |   mkfifo  |   stat   |  umask  |

### Summarize of Function Call Parameters

You will get a basic score if you only print out the raw value of monitored function calls. For example, the primitive data types char, int, short, long, long long, float, and double. For pointers, you can also print out its raw values. If you would like to get higher scores, here are additional requirements.

- For char * data type, you can print it out as a string.
- For file descriptors (passed as an int), FILE*, and DIR* pointers, you can convert them to corresponding file names.
- For char * arrays, print out the first few strings in the array.
- For uid and gid (also passed as an int), convert them to the corresponding user name and group name.
- For struct stat or its pointer, retrieve meaningful information from the structure. For example, file type, file size, and permissions.

### Grading Policy

The tentative grading policy for this homework is listed below:

- [20%] A monitored executable can work as usual. Your program cannot affect the original function of a monitored executable.
- [30%] Monitor functions listed in minimum requirements.
- [10%] Provide basic summaries for function call parameters.
- [20%] Provide comprehensive summaries for function call parameters.
- [10%] Output can be configured using MONITOR_OUTPUT environmental variable.
- [20%] Monitor more library calls than the minimum requirement (up to 5). Please describe why you choose the function and the corresponding testing binaries
- [10%] Compiled size of your codes (stripped). The smaller, the better.
- [10%] Use Makefile to manage the building process of your program. We will not grade your program if we cannot use make command to build your program.

### Hints

Some hints that may simplify your work:

- You may need to define macros to simplify your implementation.
- You may consider working with # and ## operators in macros.
- For variable-length function parameters, consider working with strarg.h.
- You may consider working with __attribute__((constructor)). If you don't know what is that, please google for it!
- The implementation for some library functions may be different to its well-known prototypes. For example, the actual implementation for stat in GNU C library is __xstat. Therefore, you may be not able to find symbol stat in the library. In case that you are not sure about the real symbols used in C library, try to work with readelf or nm to get the symbol names.

### Running Examples

Suppose you have compiled your homework files into hw2.so, two examples from our simple implementation are given below. In the first example, the output is stored into monitor.out file.

``` sh
$ MONITOR_OUTPUT=monitor.out LD_PRELOAD=./hw2.so head -n 1000 /etc/services
... show first 1000 lines in /etc/services ...
$ cat monitor.out
[monitor] realloc((nil),1600) = 0x18622e0
[monitor] realloc((nil),1024) = 0x1862930
[monitor] realloc(0x1862930,2048) = 0x1862930
[monitor] open('/etc/services', 0x0) = 4
[monitor] read(4, 0x7ffd88317a40, 8192) = 8192
[monitor] read(4, 0x7ffd88317a40, 8192) = 8192
[monitor] read(4, 0x7ffd88317a40, 8192) = 3221
[monitor] read(4, 0x7ffd88317a40, 8192) = 0
[monitor] close(4) = 0
[monitor] fflush(0x7f69c738b620) = 0
[monitor] fflush(0x7f69c738b540) = 0
```

In the second example, the output is written to stderr. The blue lines are those printed out by the monitor.

``` sh
$ MONITOR_OUTPUT=stderr LD_PRELOAD=./hw2.so ls -la
[monitor] getenv('QUOTING_STYLE') = '(null)'
[monitor] getenv('COLUMNS') = '(null)'
[monitor] getenv('TABSIZE') = '(null)'
[monitor] getenv('LS_BLOCK_SIZE') = '(null)'
[monitor] getenv('BLOCK_SIZE') = '(null)'
[monitor] getenv('BLOCKSIZE') = '(null)'
[monitor] getenv('POSIXLY_CORRECT') = '(null)'
[monitor] getenv('BLOCK_SIZE') = '(null)'
[monitor] getenv('TIME_STYLE') = '(null)'
[monitor] realloc((nil),1600) = 0x1361390
[monitor] realloc((nil),1024) = 0x13619e0
[monitor] realloc(0x13619e0,2048) = 0x13619e0
[monitor] getenv('LS_COLORS') = 'rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:
do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:mi=00:su=37;41:sg=30;43:ca=30;41:tw
=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:*.arc=01;31:*.arj=01;3
1:*.taz=01;31:*.lha=01;31:*.lz4=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz
=01;31:*.tzo=01;31:*.t7z=01;31:*.zip=01;31:*.z=01;31:*.Z=01;31:*.dz=01;31:*.gz=0
1;31:*.lrz=01;31:*.lz=01;31:*.lzo=01;31:*.xz=01;31:*.bz2=01;31:*.bz=01;31:*.tbz=
01;31:*.tbz2=01;31:*.tz=01;31:*.deb=01;31:*.rpm=01;31:*.jar=01;31:*.war=01;31:*.
ear=01;31:*.sar=01;31:*.rar=01;31:*.alz=01;31:*.ace=01;31:*.zoo=01;31:*.cpio=01;
31:*.7z=01;31:*.rz=01;31:*.cab=01;31:*.jpg=01;35:*.jpeg=01;35:*.gif=01;35:*.bmp=
01;35:*.pbm=01;35:*.pgm=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.
tif=01;35:*.tiff=01;35:*.png=01;35:*.svg=01;35:*.svgz=01;35:*.mng=01;35:*.pcx=01
;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:*.mkv=01;35:*.webm=01;35:*.
ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:*.nuv=01;3
5:*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=
01;35:*.flv=01;35:*.gl=01;35:*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cg
m=01;35:*.emf=01;35:*.ogv=01;35:*.ogx=01;35:*.aac=00;36:*.au=00;36:*.flac=00;36:
*.m4a=00;36:*.mid=00;36:*.midi=00;36:*.mka=00;36:*.mp3=00;36:*.mpc=00;36:*.ogg=0
0;36:*.ra=00;36:*.wav=00;36:*.oga=00;36:*.opus=00;36:*.spx=00;36:*.xspf=00;36:'
[monitor] getenv('TZ') = '(null)'
[monitor] opendir('.') = 0x1367ba0
[monitor] readdir(0x1367ba0) = 0x1367bd0
[monitor] __lxstat(1, '..', 0x1363070) = 0
[monitor] fopen('/etc/passwd','rme') = 0x1372220
[monitor] fopen('/etc/group','rme') = 0x13727c0
[monitor] readdir(0x1367ba0) = 0x1367be8
[monitor] __lxstat(1, '.', 0x1363130) = 0
[monitor] readdir(0x1367ba0) = 0x1367c00
[monitor] __lxstat(1, 'Makefile', 0x13631f0) = 0
[monitor] readdir(0x1367ba0) = 0x1367c20
[monitor] __lxstat(1, 'hw2.o', 0x13632b0) = 0
[monitor] readdir(0x1367ba0) = 0x1367c40
[monitor] __lxstat(1, 'hw2.so', 0x1363370) = 0
[monitor] readdir(0x1367ba0) = 0x1367c60
[monitor] __lxstat(1, 'monitor.out', 0x1363430) = 0
[monitor] readdir(0x1367ba0) = 0x1367c80
[monitor] __lxstat(1, 'hw2.c', 0x13634f0) = 0
[monitor] readdir(0x1367ba0) = (nil)
[monitor] closedir(0x1367ba0) = 0
[monitor] fputs_unlocked(0x4167e2,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x7ffc48749634,0x7f40427b7620) = 1
total 160
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x1371200,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x13727d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
drwxrwxr-x  2 chuang chuang  4096 Mar 30 09:41 .
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x1371200,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x13727d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
drwxrwxr-x 23 chuang chuang  4096 Mar 29 10:08 ..
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x1371200,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x13727d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
-rw-rw-r--  1 chuang chuang 16523 Mar 30 09:41 hw2.c
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x1371200,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x13727d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
-rw-rw-r--  1 chuang chuang 70064 Mar 30 09:41 hw2.o
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x1371200,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x13727d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
-rwxrwxr-x  1 chuang chuang 51152 Mar 30 09:41 hw2.so
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x1371200,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x13727d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
-rw-rw-r--  1 chuang chuang   307 Mar 30 09:28 Makefile
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x1371200,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x13727d0,0x7f40427b7620) = 1
[monitor] fputs_unlocked(0x7ffc487483d0,0x7f40427b7620) = 1
-rw-r--r--  1 chuang chuang   453 Mar 30 09:41 monitor.out
[monitor] fflush(0x7f40427b7620) = 0
```language
```
### Some Other Hints ...

When testing your homework, you may inspect symbols used by an executable. We have mentioned that you are not able to see any symbol if the symbols were stripped using strip command. However, you may consider working with readelf command. For example, we can check the symbols that are unknown to the binary:

``` sh
$ nm /usr/bin/wget
nm: /usr/bin/wget: no symbols
$ readelf --syms /usr/bin/wget | grep open
    72: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND freopen64@GLIBC_2.2.5 (2)
    73: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND iconv_open@GLIBC_2.2.5 (2)
   103: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND gzdopen
   107: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND fdopen@GLIBC_2.2.5 (2)
   119: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND open64@GLIBC_2.2.5 (2)
   201: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND fopen64@GLIBC_2.2.5 (2)
```

Alternatively, you may consider using nm -D to read symbols. Basically we have two different symbol tables. One is the regular symbol table and the other is dynamic symbol table. The one removed by strip is the regular symbol table. So you will need to work with nm -D or readelf --syms to read the dynamic symbol table.
