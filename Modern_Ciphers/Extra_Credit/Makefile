all:	cipher

cipher:	cipher.o DES.o 
	g++ cipher.o DES.o -o cipher -lcrypto

cipher.o:	cipher.cpp
	g++ -g -c cipher.cpp 

DES.o:	DES.cpp DES.h
	g++ -g -c DES.cpp


clean:
	rm -rf *.o cipher
