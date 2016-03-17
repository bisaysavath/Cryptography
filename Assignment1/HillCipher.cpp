#include "HillCipher.h"
#include <string>
#include <sstream>

bool HillCipher::setKey(const string& key)
{
	vector <int> testKey;
	stringstream stream(key);
	int Num;
	if(key.find_first_not_of(" -0123456789") == string::npos)
	{
		while(stream >> Num)
		{
			if(Num == "-")
			{
				return false;
			}
			testKey.push_back(Num);
		}
		if(testKey.size() != 25)
		{
			return false;
		}
	}
this->keyNum = key;
return true;
}

string HillCipher::encrypt(const string& plaintext)
{
	vector <int> enterKey;
	string ciphertext = "";
	stringstream stream(key);
	int Num;
	while(stream >> Num)
	{
		enterKey.push_back(Num);
	}
	int k = 0;
	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 5; ++j)
		{
			keymatrix[i][j] = enterKey[k];
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


int HillCipher::chartoNum(char chartoNumber)
{
	switch(chartoNumber)
	{
		case 'a':
			return 0;
		case 'b':
			return 1;
		case 'c':
			return 2;
		case 'd':
			return 3;
		case 'e':
			return 4;
		case 'f':
			return 5;
		case 'g':		
			return 6;
		case 'h':
			return 7;
		case 'i':
			return 8;
		case 'j':
			return 9;
		case 'k':
			return 10;
		case 'l':
			return 11;
		case 'm':		
			return 12;
		case 'n':		
			return 13;
		case 'o':
			return 14;
		case 'p':
			return 15;
		case 'q':
			return 16;
		case 'r':
			return 17;
		case 's':
			return 18;
		case 't':
			return 19;
		case 'u':
			return 20;
		case 'v':
			return 21;
		case 'w':		
			return 22;
		case 'x':
			return 23;
		case 'y':
			return 24;
		case 'z':
			return 25;
	}
	return -1;
}


