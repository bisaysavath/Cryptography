#include "Caesar.h"

using namespace std;

const int ALPHABET_COUNT = 26;
const int LOWER_ALPHA_ASCII_BEGIN = 97;

/**
* Sets the key to use
* @param key - the key to use
* @return - True if the key is valid and False otherwise
*/
bool Caesar::setKey(const string& key)
{
	
    //Check if every character in key is a number
    for (int i = 0; i < key.length(); i ++){
        
        if (!isdigit(key[i]))
            return false;
            
    }

    //Using stoic function to change string key to int key
    this->key = stoi(key);
    
    return true;
}


/**
* Encrypts a plaintext string
* @param plaintext - the plaintext string
* @return - the encrypted ciphertext string
*/
string Caesar::encrypt(const string& plaintext)
{
	string cipherText = "";
    
    
	for (int i = 0; i < plaintext.length(); i++)
	{
	    if (isalpha(plaintext[i]))
	    {
			char c = plaintext[i];
	        c = tolower(c);
            
            //Shifting c "key" steps to the right
	        c = (((c - LOWER_ALPHA_ASCII_BEGIN)+ key) % ALPHABET_COUNT) + LOWER_ALPHA_ASCII_BEGIN;
	        cipherText.push_back(c);
	    }
	}

	return cipherText;
}

/**
* Decrypts a string of ciphertext
* @param cipherText - the ciphertext
* @return - the plaintext
*/
string Caesar::decrypt(const string& cipherText)
{
	string plainText = "";

	for (int i = 0; i < cipherText.length(); i++)
	{
	    if (isalpha(cipherText[i]))
	    {
			char c;
	        c = cipherText[i];
			c = tolower(c);
            
            //Shifting c "key" steps to the left
	        c = ((((c - LOWER_ALPHA_ASCII_BEGIN) - key) + ALPHABET_COUNT) % ALPHABET_COUNT) + LOWER_ALPHA_ASCII_BEGIN;
	        plainText.push_back(c);
	    }
	}

	return plainText;
}




