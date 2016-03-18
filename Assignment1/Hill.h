#ifndef HILL_H
#define HILL_H

#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <iomanip>
//#include <stdlib.h>

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
	int **charMatrix;
	int **keyMatrix;
    
    int textRow;
    int textCol;
    
    /**
     * Creates a char matrix with a given row, col and text
     * Pad the column with 'X' if the column is not filled up
     * @param row - row needed, col - column needed, text - text to be input
     * @return - none
     */
    void createCharMatrix(const int& row, const int& col, const string& text);
    
    /**
     * Creates an inverse matrix for a key during a decryption process
     * @param none
     * @return - boolean value whether a process is sussessful or not
     */
    bool createInveseMatrix(void);
    
    /**
     * Takes a character type and return the position of it in the alphabeth
     * @param letter - a char type letter
     * @return - position of it in the alphabeth
     */
    int charToNum(const char& letter);
    
    /**
     * Takes a position of it in the alphabeth and return a character
     * @param num - position in the alphabeth
     * @return - a char type letter
     */
    char numToChar(const int& num);
};

#endif
