#ifndef HILL_H
#define HILL_H

#include <string>
#include <iostream>
#include <vector>
#include "CipherInterface.h"

using namespace std;

/**
 * This class implement the Hill Cipher.
 * The class extends the abstract class
 * CipherInterface.
**/

class HillCipher: public CipherInterface
{
	/** The public members **/
public:
	~HillCipher();
	
    /**
     * Sets the key to use
     * @param key - the key to use
     * @return - True if the key is valid and False otherwise
     */
    virtual bool setKey(const string& key);
    
    /**
     * Encrypts a plaintext string
     * @param plaintext - the plaintext string
     * @return - the encrypted ciphertext string
     */
    virtual string encrypt(const string& plaintext);
    
    /**
     * Decrypts a string of ciphertext
     * @param ciphertext - the ciphertext
     * @return - the plaintext
     */
    virtual string decrypt(const string& ciphertext);
    
    /* The protected members */
private:
	vector<int> keyNum;
	int row;
	int col;
	char ** matrix;
	int keymatrix[5][5];
};

#endif
