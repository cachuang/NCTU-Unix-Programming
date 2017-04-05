all: 
	g++ -fPIC -shared -Os -s -o hw2.so hijack.cpp -ldl
	gcc test.c -o test
run:
	LD_PRELOAD=./hw2.so ./test
