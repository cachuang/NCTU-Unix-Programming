all: 
	g++ -fPIC -shared -Os -s -o hw2.so hijack.cpp -ldl
	gcc test.c -o test
clean:
	rm -f test hw2.so monitor.out
run:
	LD_PRELOAD=./hw2.so ./test
