#ifndef VIGENERE_H
#define VIGENERE_H

#include <vector>   /* For vectors */
#include <string>   /* For C++ strings */
#include <stdio.h>
#include <iostream> /* For standard I/O */
#include <stdlib.h> /* For miscellenous C functions */
#include <algorithm> /* For collection of useful functions */
#include <iomanip>  /* For I/O manipulations */

#include "CipherInterface.h"

using namespace std;

/**
 * This class implements the Vigenere cipher.
 * The class extends the abstract class
 * CipherInterface.
 */

class Vigenere: public CipherInterface
{
    /** The public members **/
public:
    
    /** Destructor **/
    ~Vigenere();
    
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
protected:
    string key;
    char **vigenereSquare;
    
    /** Variable to hold on col and row of the intersect point on VS**/
    struct Pair {
        int row;
        int col;
    }intersection;
    
    /**
     * Create a Vigenere square
     * @param - none
     * @return - none
     */
    void createVigenereSqure(void);
    
    /**
     * Look for an intersection of letter and key
     * @param key - a key letter, letter - a plaintext letter
     * @return - A Pair struct containing row and col
     */
    void findIntersection(const char& key, const char& letter = '\0');
};

#endif
