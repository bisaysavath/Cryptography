#ifndef HILLCIPHER_H
#define HILLCIPHER_H

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

class HillCipher::public CipherInterface
{
	/** The public members **/
public:
	~HillCipher();
	
	virtual bool setKey(const string&key);
	

	virtual string encrypt(const string& plaintext);

	virtual string decrypt(const string& ciphertext);

	void createMatrix();

	/* the private members */
private:
	string keyNum;
	int row;
	int col;
	char ** matrix;
	int keymatrix[5][5];
};

#endif
