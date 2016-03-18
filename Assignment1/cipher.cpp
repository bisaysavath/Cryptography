#include <string>
#include <fstream>
#include "CipherInterface.h"
#include "Playfair.h"
#include "Railfence.h"
#include "Vigenere.h"
#include "RowTransposition.h"
#include "Caesar.h"
#include "Hill.h"

using namespace std;

int main(int argc, char** argv)
{

	/* Check number of arguments */
	if (argc != 6 ){
		cout << "Not enough arguments" <<endl;
		cout << "Usage: " << argv[0] <<" <CIPHER NAME> <KEY> <ENC/DEC> <INPUTFILE> <OUTPUT FILE>" <<endl;
		exit (-1);
	}

	string cipherName = argv[1];
	string key = argv[2];
	string command = argv[3];
	char* inputFile = argv[4];
	char* outputFile = argv[5];

	CipherInterface* cipher;
 
	if ( cipherName == "PLF" ){
    
		/* Create an instance of the Playfair cipher */	
		cipher = new Playfair();

	}
	else if ( cipherName == "RFC" ){

		/* Create an instance of the Railfence cipher */	
		cipher = new Railfence();
	}
	else if ( cipherName == "VIG" ) {
		/* Create an instance of the Vigenere cipher */	
		cipher = new Vigenere();
	}
	else if (cipherName == "RTS")
	{	/* Create an instance of the RowTransposition cipher */
		cipher = new RowTransposition();		  
	}
	else if (cipherName == "CES")
	{
		/*Creating an instance of Caesar cipher */
	    cipher = new Caesar();
	}
	else if (cipherName == "HIL")
	{
		/*Creating an instance of Caesar cipher */
	    cipher = new Hill();
	}
	else {
		cout << "Couldn't identify a given cipher" << endl;
		exit (-2);
	}
  
	/* Error checks */
	if(!cipher)
	{
		fprintf(stderr, "ERROR [%s %s %d]: could not allocate memory\n",	
			__FILE__, __FUNCTION__, __LINE__);
		exit(-3);
	}
	
	/* Set the encryption key */
	if(!cipher->setKey(key)){
		cout << "The key's format is not correct"<<endl;
		exit(-4);
	}

	/* Open input file */
	ifstream inFile(inputFile);
	if ( !inFile.is_open() ){
		cout <<"Couldn't open input file"<<endl;
		exit (-5);
	}

	ofstream outFile(outputFile);
	if ( !outFile.is_open() ){
		cout << "Couldn't open output file"<<endl;
		exit(-6);
	}
  
	/* Read file */
	string line;
	string text;
	while ( getline(inFile, line)){
		if ( command == "ENC" ){
    
			/* Perform encryption */
			text = cipher->encrypt(line);
		}
		else if ( command == "DEC" ){
			/* Perform decryption */
			text = cipher->decrypt(line);
		}
		
		outFile << text <<endl;
	}
	
	delete cipher;
  	inFile.close();
	outFile.close();
	cout << "Success!!" <<endl;
	
	return 0;
}
