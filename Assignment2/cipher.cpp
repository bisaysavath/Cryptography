#include <iostream>
#include <fstream>
#include <string>
#include "CipherInterface.h"
#include "DES.h"

using namespace std;

int main(int argc, char** argv)
{
	/* REPLACE THIS PART WITH YOUR CODE 
	 * THE CODE BELOW IS A SAMPLE TO 
	 * ILLUSTRATE INSTANTIATION OF CLASSES
	 * THAT USE THE SAME INTERFACE.
	 */	
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
    ifstream inFile(inputFile);
    if ( !inFile.is_open() )
    {
        cout << "Couldn't open input file" << endl;
        exit (-4);
    }
    
    /* Open output file */
    ofstream outFile(outputFile);
    if ( !outFile.is_open() )
    {
        cout << "Couldn't open output file" << endl;
        exit(-5);
    }
    
    unsigned char* getCipherText;
    string line;
    string text = "";
    
    while (getline(inFile, line))
    {
        while (line.length() > 0)
        {
            const char* get8Byte;
            
            // Grab 8 bytes from a line
            get8Byte = line.substr(0, 8).c_str();
            line.erase(0, 8);
            
            // Is get8Byte is is less than 8 byte?
            if (strlen(get8Byte) < 8)
            {
                // Pad get8Byte with "x"
                string tempString = get8Byte;
                for (int curLength = strlen(get8Byte); curLength < 8; curLength++)
                {
                    tempString += "x";
                }
                get8Byte = tempString.c_str();
            }
            
            if (mode == "ENC")
            {
                /* Perform encryption */
                getCipherText = cipher->encrypt((unsigned char*)get8Byte);
            }
            else if (mode == "DEC")
            {
                /* Perform encryption */
                getCipherText = cipher->decrypt((unsigned char*)get8Byte);
            }
            else
            {
                cout << "Could not determine a cipher mode" << endl;
                exit(-6);
            }
            
            /* Convert from unsigned char* to String */
            string charPToString(reinterpret_cast<char*>(getCipherText));
            text += charPToString;
        }
        
        outFile << text;
    }
    
    delete cipher;
    inFile.close();
    outFile.close();
    cout << "Success!!" <<endl;
	
	return 0;
}
