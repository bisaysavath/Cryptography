#include <iostream>
#include <fstream>
#include <string>
#include "CipherInterface.h"
#include "DES.h"

using namespace std;

int main(int argc, char** argv)
{
	
	/* Check number of arguments */
	if (argc != 6 )
		{
			cout << "Not enough arguments" <<endl;
			cout << "Usage: " << argv[0] <<" DES <KEY> <ENC/DEC> <INPUTFILE> <OUTPUT FILE>" <<endl;
			exit (-1);
		}
    
	string cipherName = argv[1];
	char* key = argv[2];
	string mode = argv[3];
	char* inputFile = argv[4];
	char* outputFile = argv[5];

	/* Create an instance of the DES cipher */	
	CipherInterface* cipher = new DES();
	
	/* Error checks */
	if(!cipher)
		{
			fprintf(stderr, "ERROR [%s %s %d]: could not allocate memory\n",	
				__FILE__, __FUNCTION__, __LINE__);
			exit(-2);
		}
	
	/* Set the encryption key
	 * A valid key comprises 16 hexidecimal
	 */
	if(!cipher->setKey((unsigned char*)key))
		{
			cout << "ERROR: Key should have 16 hexadecimal digits." << endl;
			exit(-3);
		}
    
	/* Open input file */
   
	FILE* inFile;
	FILE* outFile;
	inFile = fopen(inputFile, "r");
	outFile = fopen(outputFile, "w");
	if ( inFile == NULL )
		{
			cout << "ERROR: Could not open input file" << endl;
			exit(-3);
		}

	if ( outFile == NULL )
		{
			cout << "ERROR: Could not open output file" << endl;
		}
	
	int size;	
	unsigned char* cipherText;
	unsigned char text[8];
	unsigned char* decryptedText;
	while ( (size = fread(text, 1, 8, inFile)) != 0 )
		{

		        while ( size != 8 ){

				/* Padding */
				text[size] = 'x'; 
				size++;
			}
			if ( mode == "ENC" ){
				
				/* Perform encryption */
				cipherText = cipher->encrypt((unsigned char*)text);
				printf("%s\n", cipherText);
				fwrite(cipherText, 1, 8, outFile);
				free(cipherText);
			}

			if ( mode == "DEC" ){
				
				/* Perform decryption */		
				decryptedText = cipher->decrypt((unsigned char*)text);	
				printf("%s\n", decryptedText);
				fwrite(decryptedText, 1, 8, outFile);
				free(decryptedText);
				
			}
		}

        fclose(inFile);
	fclose(outFile);
	cout << "Success!!" <<endl;
	
	return 0;
}
