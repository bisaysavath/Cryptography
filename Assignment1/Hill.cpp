#include "Hill.h"

#include <string>
#include <sstream>
#include <math.h>

const int ALPHABETH_COUNT = 26;
const int LOWER_ALPHA_ASCII_BEGIN = 97;

const int MATRIX_SIZE = 5;

bool HillCipher::setKey(const string& key)
{
	vector <int> testKey;
	stringstream stream(key);
	int Num;
	if(key.find_first_not_of(" 0123456789") == string::npos)
	{
		while(stream >> Num)
		{
			testKey.push_back(Num);
		}
        
		if(testKey.size() != pow(MATRIX_SIZE, 2))
		{
			return false;
		}
	}
    else
    {
        return false;
    }
    
    this->keyNum = testKey;
    return true;
}

string HillCipher::encrypt(const string& plaintext)
{
	string ciphertext = "";

	int k = 0;
	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 5; ++j)
		{
			keymatrix[i][j] = this->keyNum[k];
			++k;
		}
	}

	row = 5;
	testCol = plaintext.length() % row;
	if(testCol != 0)
	{
		col = plaintext.length() / row + 1;
	}
	else
	{
		col = plaintext.length() / row;
	}
	createMatrix();
	
	for(int i = 0; i < plaintext.length(); ++i)
	{
		char convert;
		convert = plaintext.at(i);
		int result = convert - 'a';
		this->matrix[i%row][i/row] = result;
	}
	
	if(testCol != 0)
	{
		for(int j = testCol; j < row; ++j)
		{
			this->matrix[j][col-1] = 23;
		}
	}
	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < col; ++j)
		{
			for(int k = 0; k < 5; ++k)
			{
				this->matrix[i][j] += keymatrix[i][k] * matrix[k][j];
			}
		}
	}
	for(int i = 0; i < col; ++i)	
 	{
		for(int j = 0; j < 5; ++j)
		{
			int tempNum = this->matrix[i][j];
			while(tempNum < 0)
			{
				tempNum += 27;
			}
			char returnLetter = static_cast<char>('a' + temNum);
			stringstream ss;
			string tempEl;
			ss << returnLetter;
			ss >> tempEl;
			ciphertext.push_back(tempEl);
		}
	}
	return ciphertext; 
}

string HillCipher::decrypt(const string& ciphertext)
{
	vector <int> enterKey;
	string ciphertext = "";
	stringstream stream(key);
	int Num;
	while(stream >> Num)
	{
		enterKey.push_back(Num);
	}
	
}
//create the matrix when we know the row and the column
void HillCipher::createMatrix()
{
	this->matrix = new int[row];
	for(int i = 0; i < row; ++i)
	{
		this->matrix[i] = new int[col];
	}
	
	for(int i = 0; i < row; ++i)
	{
		for(int j = 0; j < col; ++j)
		{
			matrix[i][j] = -1;
		}
	}
}


int HillCipher::charToNum(const char& letter)
{
    return (LOWER_ALPHA_ASCII_BEGIN + letter) % ALPHABETH_COUNT;
}


