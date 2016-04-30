#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "CipherInterface.h"
#include "DES.h"
#include "RSA.h"

using namespace std;

int main(int argc, char** argv){
	
	/* Check number of arguments */
	if (argc != 6 ){
		cout << "Not enough arguments" << endl;
		cout << "Usage: " << argv[0] <<" DES <KEY> <ENC/DEC> <INPUTFILE> <OUTPUT FILE>" << endl;
		exit (-1);
	}
    
	string cipherName = argv[1];
	char* key = argv[2];
	string mode = argv[3];
	char* inputFile = argv[4];
	char* outputFile = argv[5];

	CipherInterface* cipher;
	
	/* Select a cipher */
	if(cipherName == "DES"){
		/* Create an instance of the DES cipher */	
		cipher = new DES();
	}
	else if(cipherName == "RSA"){
		cipher = new RSA_433();
	}
	else if(cipherName == "CBC"){
		cipher = new DES();
	}
	else if(cipherName == "CFB"){
		cout << "CFB" << endl;
		exit(-9);
	}
	else{
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
	
	if(cipherName == "DES" || cipherName == "CBC"){
		int size;	
		unsigned char* cipherText;
		unsigned char text[9];
		unsigned char* decryptedText;
		int padding = 0;
		text[8] = '\0';	// Forcing a '\0' character at the end of text
		
		// Initialize a fixed Initialization Vector for Cipher Block Chaining
		unsigned char IV[9] = {'a','b','c','d','e','f','g','h','\0'};	// Forcing a '\0' character at the end of IV
		unsigned char tempCharHolder[9];
		tempCharHolder[8] = '\n'; // Forcing a '\0' character at the end of tempCharHolder
		
		cout << ((mode == "ENC") ? "Cipher text: " : "Plaintext: ");
		
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

			if ( mode == "ENC" ){
				// In CBC, input to the encryption is the XORed between IV and a plaintext
				if(cipherName == "CBC"){
					// On a first run, XOR the text with IV
					// (cipherText will be null for the first run)
					if(!cipherText){
						strcpy((char *)tempCharHolder, (const char *)IV);
					}
					
					// Perform an XOR
					for (int i = 0; text[i] != '\0'; i++){
						text[i] = (text[i] ^ tempCharHolder[i]); // Assuming that plaintext will always be lowercase
					}
				}
				
				/* Perform encryption */
				cipherText = cipher->encrypt((unsigned char*)text);
				printf("%s", cipherText);
				fwrite(cipherText, sizeof(char), 8, outFile);
				
				// Store the cipherText to be used the next round
				if(cipherName == "CBC"){
					strcpy ((char *)tempCharHolder, (const char *)cipherText);
				}
				
				free(cipherText);
			}

			if ( mode == "DEC" ){
				// In CBC, plaintext is from the XORed between IV and an output of a decryption
				if(cipherName == "CBC"){
					if(!decryptedText){
						strcpy((char *)tempCharHolder, ((const char *)IV));
					}
				}

				/* Perform decryption */		
				decryptedText = cipher->decrypt((unsigned char*)text);
				
				if(cipherName == "CBC"){
					// Perform an XOR
					for (int i = 0; tempCharHolder[i] != '\0'; i++){
						decryptedText[i] = (decryptedText[i] ^ tempCharHolder[i]); // Assuming that plaintext will always be lowercase
					}
					
					// Store the cipherText to be used the next round
					strcpy ((char *)tempCharHolder, (const char *)text);
				}
				
				printf("%s", decryptedText);
				fwrite(decryptedText, sizeof(char), 8, outFile);
				
				free(decryptedText);
			}
		}

		/* Remove padding for DES decryption */
		if ( mode == "DEC" ){

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
	
	delete(cipher);
	fclose(inFile);
	fclose(outFile);
	cout << "\nSuccess!!" << endl;
	
	return 0;
}
