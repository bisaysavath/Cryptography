#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "CipherInterface.h"
#include "DES.h"
#include "RSA.h"

/* The modulus size */
#define MOD_SIZE 2048

using namespace std;

int main(int argc, char** argv){
	
	/* Check number of arguments */
	if ( argc != 6 ){
		cout << "Not enough arguments" <<endl;
		cout << "Usage: " << argv[0] <<" <DES/RSA>  <KEY> <ENC/DEC> <INPUTFILE> <OUTPUT FILE>" <<endl;
		exit (-1);
	}

	string cipherName = argv[1];
	char* key = argv[2];
	string command = argv[3];
	char* inputFile = argv[4];
	char* outputFile = argv[5];

	/* Create an instance of the DES cipher */	
	CipherInterface* cipher;

	if ( cipherName == "DES" ){
		cipher = new DES();
	}
	else if ( cipherName == "RSA" ){
		cipher = new RSA_433();
	}
	else{
		cout << "ERROR: Unknown ciphername" <<endl;
		exit (-1);
	}
	/* Error checks */
	if(!cipher){
		fprintf(stderr, "ERROR [%s %s %d]: could not allocate memory\n",	
			__FILE__, __FUNCTION__, __LINE__);
		exit(-2);
	}
	
	/* Set the encryption key
	 * A valid key comprises 16 hexidecimal
	 */
	if(!cipher->setKey((unsigned char*)key)){
		cout << "ERROR: Key should have 16 hexadecimal digits." << endl;
		exit(-3);
	}
    
	/* Open input file */
   
	FILE* inFile;
	FILE* outFile;
	inFile = fopen(inputFile, "r");
	outFile = fopen(outputFile, "w+");
	if ( inFile == NULL ){
		cout << "ERROR: Could not open input file" << endl;
		exit(-4);
	}

	/* Open output file */
	if ( outFile == NULL ){
		cout << "ERROR: Could not open output file" << endl;
		exit(-5);
	}

	int size;	
	unsigned char* cipherText;
	unsigned char* decryptedText;

	/********** DES ENCRYPTION/DECRYPTION ************/
	
	if ( cipherName == "DES"){
		unsigned char text[9];
		memset(text, 0, 8);
		text[8] = '\0';
		int padding = 0;
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
			
			if ( command == "ENC" ){
				
				/* Perform encryption */
				cipherText = cipher->encrypt((unsigned char*)text);
				if ( cipherText != NULL ){
					printf("%s", cipherText);
					fwrite(cipherText, sizeof(char), 8, outFile);
					free(cipherText);
				}
			}

			if ( command == "DEC" ){
				
				/* Perform decryption */		
				decryptedText = cipher->decrypt((unsigned char*)text);
				if ( decryptedText != NULL ){
					printf("%s", decryptedText);
					fwrite(decryptedText, sizeof(char), 8, outFile);
					free(decryptedText);
				}
				
			}
		}

		/* Remove padding for DES decryption */
		if ( command == "DEC" ){

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
	}

	/************* RSA ENCRYPTION/DECRYPTION ******************/
	else {
		unsigned char text[MOD_SIZE/8];
		int cipherTextLen = -1;
		int decryptedTextLen = -1;

		/* Get the size of the file */
		fseek(inFile, 0, SEEK_END);
		size = ftell(inFile);
		rewind(inFile);

		/* Check the size */
		if(size > (MOD_SIZE/8)){
			fprintf(stderr, "Text size (%d bytes) exceeds the modulus size (%d bytes)\n",
			        size, (MOD_SIZE/8));

			fclose(inFile);
			fclose(outFile);
			exit(-1);
		}

		else {
			/* Read the file */
			fread(text, sizeof(char), size, inFile);

			if ( command == "ENC" ){

				/* Perform encryption */
				cipherText = cipher->encrypt(text, size, cipherTextLen);
				if ( cipherText != NULL ){
					printf("%s", cipherText);
					fwrite(cipherText, sizeof(char), cipherTextLen, outFile);
					free(cipherText);
				}
			}
			else if ( command == "DEC" ){

				/* Perform decryption */
				decryptedText = cipher->decrypt(text, size, decryptedTextLen);
				if ( decryptedText != NULL){
					printf("%s", decryptedText);
					fwrite(decryptedText, sizeof(char), decryptedTextLen, outFile);
					free(decryptedText);
				}
			}
			else {
				cout << "ERROR: Invalid command " <<endl;
				fclose(inFile);
				fclose(outFile);
				exit(-1);
			}
		}
	}
	

	fclose(inFile);
	fclose(outFile);
	cout << "\nDone!!" << endl;	
	return 0;
}
