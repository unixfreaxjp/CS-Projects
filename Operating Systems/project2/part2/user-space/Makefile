all: testcall getprinfo.o

testcall: testcall.c getprinfo.o
	cc testcall.c getprinfo.o -o testcall
	
getprinfo.o:
	cc -c getprinfo.c
	
clean:
	rm testcall getprinfo.o
