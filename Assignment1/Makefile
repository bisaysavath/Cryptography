all:	cipher

cipher:	cipher.o Playfair.o Railfence.o Vigenere.o
	g++ cipher.o Playfair.o Railfence.o Vigenere.o -o cipher

cipher.o: cipher.cpp
	g++ -c cipher.cpp 

Playfair.o: Playfair.cpp Playfair.h CipherInterface.h
	g++ -c Playfair.cpp

Railfence.o: Railfence.h Railfence.cpp CipherInterface.h
	g++ -c Railfence.cpp
	
Vigenere.o: Vigenere.h Vigenere.cpp CipherInterface.h
	g++ -c Vigenere.cpp
	
# Uncomment this code once you add the appropriate files
#RowTransposition.o:	RowTransposition.cpp RowTransposition.h
#	g++ -g -c RowTransposition.cpp


clean:
	rm -rf *.o *~ cipher
