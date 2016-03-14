all:	cipher

cipher:	cipher.o Playfair.o Railfence.o Vigenere.o RowTransposition.o Caesar.o
	g++ cipher.o Playfair.o Railfence.o Vigenere.o RowTransposition.o Caesar.o -o cipher

cipher.o: cipher.cpp
	g++ -c cipher.cpp 

Playfair.o: Playfair.cpp Playfair.h CipherInterface.h
	g++ -c Playfair.cpp

Railfence.o: Railfence.h Railfence.cpp CipherInterface.h
	g++ -c Railfence.cpp

Vigenere.o: Vigenere.h Vigenere.cpp CipherInterface.h
	g++ -c Vigenere.cpp

RowTransposition.o: RowTransposition.cpp RowTransposition.h CipherInterface.h
	g++ -c RowTransposition.cpp

Caesar.o: Caesar.cpp Caesar.h CipherInterface.h
	g++ -std=c++11 -c Caesar.cpp

clean:
	rm -rf *.o *~ cipher
