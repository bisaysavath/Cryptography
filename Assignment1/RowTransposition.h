#ifndef ROWTRANSPOSITION_H
#define ROWTRANSPOSITION_H

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "CipherInterface.h"

using namespace std;

/**
 *This class implement the Row Transposition cipher.
 *The class extends the abstract class
 *CipherInterface.
*/

class RowTransposition:public CipherInterface
{
	/** The public members **/
public:
	~RowTransposition();
	
	virtual bool setKey(const string& key);
	/** 
	 *sets the key to use
	 *@param key - the key to use
	 *@return - True if the key is valid and False otherwise
	 */

	virtual string encrypt(const string& plaintext);
	/** 
	 * Encrypts a plaintext string
	 * @param plaintext - the plaintext string
	 * @return - the encrypted ciphertext string
	*/

	virtual string decrypt(const string& ciphertext);
	/**
	 *Decrypts a  string of ciphertext
	 *@param ciphertext - the ciphertext
	 *@return - the plaintext
	*/

	void createMatrix();

	/* The private members */
private:
	int keyNum;
	int row;
	int col;
	char ** matrix;

};

#endif
