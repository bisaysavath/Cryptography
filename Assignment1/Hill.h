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

class Hill: public CipherInterface
{
	/** The public members **/
public:
    
    /** Constructor and destructor **/
    // Initialize the const MATRIX_WIDTH for Hill
//    Hill(void);
//	~Hill();
	
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
	int row;
	int col;
	char **matrix;
	int **keyMatrix;
    void createMatrix();
    int charToNum(const char& letter);
};

#endif
