#include <iostream>
#include <string>
#include <fstream>
#include "CipherInterface.h"
#include "DES.h"

using namespace std;

int main(int argc, char** argv)
{
//	/* REPLACE THIS PART WITH YOUR CODE 
//	 * THE CODE BELOW IS A SAMPLE TO 
//	 * ILLUSTRATE INSTANTIATION OF CLASSES
//	 * THAT USE THE SAME INTERFACE.
//	 */	
//		
//
//	/* Create an instance of the DES cipher */	
//	CipherInterface* cipher = new DES();
//	
//	/* Error checks */
//	if(!cipher)
//	{
//		fprintf(stderr, "ERROR [%s %s %d]: could not allocate memory\n",	
//		__FILE__, __FUNCTION__, __LINE__);
//		exit(-1);
//	}
//	
//	/* Set the encryption key
//	 * A valid key comprises 16 hexidecimal
//	 * characters. Below is one example.
//	 * Your program should take input from
//	 * command line.
//	 */
//	cipher->setKey((unsigned char*)"0123456789abcdef");
//	
//	/* Perform encryption */
//	//string cipherText = cipher->encrypt("hello world");
//	
//	/* Perform decryption */
//	//cipher->decrypt(cipherText);
    
    /* Check number of arguments */
    if (argc != 6 ){
        cout << "Not enough arguments" <<endl;
        cout << "Usage: " << argv[0] <<" <CIPHER NAME> <KEY> <ENC/DEC> <INPUTFILE> <OUTPUT FILE>" <<endl;
        exit (-1);
    }
    
    string cipherName = argv[1];
    char* key = argv[2];
    string command = argv[3];
    char* inputFile = argv[4];
    char* outputFile = argv[5];
    
    CipherInterface* cipher;
    
    if (cipherName == "DES") {
        /*Creating an instance of DES cipher */
        cipher = new DES();
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
    
    /* Set the encryption key
     * A valid key comprises 16 hexidecimal
     * characters.
     */
    if( !cipher->setKey((unsigned char*)key))
    {
        cout << "The key format is not correct" << endl;
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
    unsigned char* text;
    while ( getline(inFile, line)){
        
        //Convert string to char pointer
        char *cstr = &line[0u];
        
        if ( command == "ENC" ){
            
            /* Perform encryption */
            text = cipher->encrypt((unsigned char*)cstr);
        }
        else if ( command == "DEC" ){
            /* Perform decryption */
            text = cipher->decrypt((unsigned char*)cstr);
        }
        
        outFile << text << endl;
    }
    
    delete cipher;
    
    inFile.close();
    outFile.close();
    
    cout << "Success!!" <<endl;
	
	return 0;
}
