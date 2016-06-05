#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "CipherInterface.h"
#include "DES.h"

using namespace std;

/* Function to perform CBC mode */
bool CBC(FILE* inFile, FILE* outFile, CipherInterface *cipher, string command);

/* Function to perform CFB  mode */
bool CFB(FILE* inFile, FILE* outFile, CipherInterface *cipher, string command);

int main(int argc, char** argv){
	
	/* Check number of arguments */
	if (argc != 7 ){
		cout << "Not enough arguments" << endl;
		cout << "Usage: " << argv[0] <<" DES <KEY> <ENC/DEC> <CBC/CFB> <INPUTFILE> <OUTPUT FILE>" << endl;
		exit (-1);
	}
    
	string cipherName = argv[1];
	char* key = argv[2];
	string command = argv[3];
	string mode = argv[4];
	char* inputFile = argv[5];
	char* outputFile = argv[6];

	CipherInterface* cipher;
	
	/* Select a cipher */
	if(cipherName == "DES"){
		/* Create an instance of the DES cipher */	
		cipher = new DES();
	}
        else {
		cout << "ERROR: Unknown cipher name" << endl;
		exit (-2);
	}
	
	/* Error checks */
	if(!cipher){
		fprintf(stderr, "ERROR [%s %s %d]: could not allocate memory\n",	
			__FILE__, __FUNCTION__, __LINE__);
		exit(-3);
	}
	
	/* Set the encryption key
	 * A valid key comprises 16 hexidecimal
	 */
	if(!cipher->setKey((unsigned char*)key)){
		exit(-4);
	}
    
	/* Open input file */
   
	FILE* inFile;
	FILE* outFile;
	inFile = fopen(inputFile, "r");
	outFile = fopen(outputFile, "w+");
	if ( inFile == NULL ){
		cout << "ERROR: Could not open input file" << endl;
		exit(-5);
	}

	if ( outFile == NULL ){
		cout << "ERROR: Could not open output file" << endl;
		exit(-6);
	}

	bool status = false;
	if ( mode == "CBC" ){
		status = CBC(inFile, outFile, cipher, command);
	}

	else if ( mode == "CFB" ){
		status = CFB(inFile, outFile, cipher, command);
	}

	else {
		cout << "ERROR: Invalid mode" <<endl;
		exit(-7);
	}

	if ( status ){
		cout << "\nSuccess"<<endl;
	}
	else{
		cout << "\nFailed"<<endl;
	}
	
	delete(cipher);
	fclose(inFile);
	fclose(outFile);
	
	return 0;
}

/* Function to perform CBC mode */
bool CBC(FILE* inFile, FILE* outFile, CipherInterface *cipher, string command){
	
	int size;	
	unsigned char* cipherText;
	unsigned char text[9];
	text[8] = '\0';	// Forcing a '\0' character at the end of text
	unsigned char* decryptedText;
	int padding = 0;
		
	// Initialize a fixed Initialization Vector for Cipher Block Chaining
	unsigned char IV[9] = {'a','b','c','d','e','f','g','h','\0'};	// Forcing a '\0' character at the end of IV
	
	unsigned char tempCharHolder[9];
	tempCharHolder[8] = '\0'; // Forcing a '\0' character at the end of tempCharHolder

	
	while ( (size = fread(text, sizeof(char), 8, inFile)) != 0 ){

		while ( size != 8 ){
			/* Padding */
			padding++;

			if ( size == 7 ){
				text[size] = padding + '0';
			}
			else {
				text[size] = '0';
			}
			size++;
		}

		/* Encryption */
		if ( command == "ENC" ){
			
			// In CBC, input to the encryption is the XORed between IV and a plaintext

			       				
			// Perform an XOR
			for (int i = 0; i < 8; i++){
				tempCharHolder[i] = text[i] ^ IV[i]; // Assuming that plaintext will always be lowercase
			}
				
			/* Perform encryption */
			cipherText = cipher->encrypt((unsigned char*)tempCharHolder);
			printf("%s", cipherText);
			fwrite(cipherText, sizeof(char), 8, outFile);
				
			// Store the cipherText to be used the next round
			memcpy(&IV, cipherText, 8);
				
			free(cipherText);
		}

		/* Decryption */
		else if ( command == "DEC" ){
			
			// In CBC, plaintext is from the XORed between IV and an output of a decryption

			/* Perform decryption */
			decryptedText = cipher->decrypt((unsigned char*)text);
			       
			memcpy(&tempCharHolder, decryptedText, 8);
			// Perform an XOR
			for (int i = 0; i < 8; i++){
				decryptedText[i] = (tempCharHolder[i] ^ IV[i]);
			}
					
			// Store the cipherText to be used the next round
			memcpy(&IV, text, 8);
				
			printf("%s", decryptedText);
			fwrite(decryptedText, sizeof(char), 8, outFile);
				
			free(decryptedText);
		}

		else {
			cout << "ERROR: Incorrect command (ENC/DEC)" <<endl;
			return false;
		}
	}

	/* Remove padding for DES decryption */
	if ( command == "DEC"  ){

		long int outFileSize = ftell(outFile);
		unsigned char lastBlock[8];

		/* Go to the last block of the output file */
		fseek(outFile, -8, SEEK_END);
			
		/* Read in the content of the last block */
		fread(lastBlock, sizeof(char), 8, outFile);

		/* Examine the last character */
		padding = (int) (lastBlock[7] - '0');
		bool isPadding = true;
		if ( padding > 0 && padding < 8 ){
				
			/* Verify the padding */
			while ( padding > 1 ){
				if ( lastBlock[8-padding] != '0' ) isPadding = false;
				padding--;
			}

			if ( isPadding ){
					
				/* Truncate the output file */
				ftruncate(fileno(outFile), outFileSize - (int)(lastBlock[7] - '0') );		        
			}
		}
	}

	return true;
	
}

/* Function to perform CFB mode */
bool CFB(FILE* inFile, FILE* outFile, CipherInterface *cipher, string command){

	int size;	
	unsigned char* cipherText;
	unsigned char text[4];
	unsigned char* decryptedText;
	int padding = 0;
		
	// Initialize a fixed Initialization Vector for Cipher Block Chaining
	unsigned char IV[8] = {'a','b','c','d','e','f','g','h'};	// Forcing a '\0' character at the end of IV
	
	unsigned char tempCharHolder[4];

	/* Read in 4 bytes at the time */
	while ( (size = fread(text, sizeof(char), 4, inFile)) != 0 ){

		
		/* Encryption */
		if ( command == "ENC" ){
				
			/* Perform encryption on IV */
			cipherText = cipher->encrypt((unsigned char*)IV);

			/* Grab the first 4 bytes of cipherText and XOR with text */

			for ( int i = 0; i < size; i++){
				tempCharHolder[i] = text[i] ^ cipherText[i];
			}

			fwrite(tempCharHolder, sizeof(char), size, outFile);
			printf("%s\n", tempCharHolder);	
			/* Shift the last 4 bytes of IV to the right, and copy the first 4 bytes of
			   of tempCharHolder to IV */

			for ( int i = 0; i < 8 - size; i++){
				IV[i] = IV[i+size];
			}

			for ( int i = 0; i < size; i++){
				IV[8-size+i] = tempCharHolder[i];
			}
		}

		/* Decryption */
		else if ( command == "DEC" ){

		        /* Perform encryption on IV */
			cipherText = cipher->encrypt((unsigned char*)IV);

			/* Grab the first 4 bytes of cipherText and XOR with text */

			for ( int i = 0; i < size; i++){
				tempCharHolder[i] = text[i] ^ cipherText[i];
			}

			fwrite(tempCharHolder, sizeof(char), size, outFile);
			printf("%s\n", tempCharHolder);
			
			/* Shift the last 4 bytes of IV to the right, and copy the first 4 bytes of
			   of text to IV */
			for ( int i = 0; i < 8 - size; i++){
				IV[i] = IV[i+size];
			}

			for ( int i = 0; i < size; i++){
				IV[8-size+i] = text[i];
			}				

		}

		else {
			cout << "ERROR: Incorrect command (ENC/DEC)" <<endl;
			return false;
		}
	}

	return true;

}
