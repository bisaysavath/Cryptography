#include "Caesar.h"

using namespace std;


/**
* Sets the key to use
* @param key - the key to use
* @return - True if the key is valid and False otherwise
*/
bool Caesar::setKey(const string& key)
{
	if (!key.empty() && key.find_first_not_of("0123456789") == std::string::npos)
	{
		//using stoic function to change string key to int key
		this->key = stoi(key);
		return true;
	}
	else
	    return false;

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
			char c;
	        c = plaintext[i];
	        c = tolower(c);
			//Using ASCII number
			//a is 97 dec
	        c = (((c-97)+ key) % 26) + 97;
	        cipherText.push_back(c);
	    }
	}

	cout << "Encrypt called" << endl;
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
			//Using ASCII number
			//a is 97 dec
	        c = ((((c - 97) - key) + 26) % 26) + 97;
	        plainText.push_back(c);
	    }
	}

	cout << "Decrypt called" << endl;
	return plainText;
}




