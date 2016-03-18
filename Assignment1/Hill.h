#ifndef HILL_H
#define HILL_H

#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <algorithm>

#include "CipherInterface.h"

using namespace std;

/**
 * This class implement the Hill Cipher.
 * The class extends the abstract class
 * CipherInterface.
**/

class Hill: public CipherInterface
{
	/** The public members **/
public:
    
    /**m
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
	int **charMatrix;
	int **keyMatrix;
    
    void createCharMatrix(const int& row, const int& col, const string& plaintext);
    int charToNum(const char& letter);
    char numToChar(const int& num);
};

#endif
