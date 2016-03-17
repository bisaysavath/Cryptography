#include "Hill.h"

#include <string>
#include <sstream>
#include <math.h>

const int ALPHABETH_COUNT = 26;
const int LOWER_ALPHA_ASCII_BEGIN = 97;
const int MATRIX_WIDTH = 5;

//Hill::Hill(void)
//{
//
//}

bool Hill::setKey(const string& key)
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
        
		if(testKey.size() != pow(MATRIX_WIDTH, 2))
		{
            cout << "ERROR: Key size must be: " << pow(MATRIX_WIDTH, 2) << endl;
			return false;
		}
	}
    else
    {
        cout << "ERROR: Numbers only." << endl;
        return false;
    }
    
    keyMatrix = new int*[MATRIX_WIDTH];
    int k = 0;
    
    for(int i = 0; i < MATRIX_WIDTH; i++)
    {
        keyMatrix[i] = new int[MATRIX_WIDTH];
        
        for(int j = 0; j < MATRIX_WIDTH; j++, k++)
        {
            keyMatrix[i][j] = testKey[k];
        }
    }
    
    for(int i = 0; i < MATRIX_WIDTH; i++)
    {
        for(int j = 0; j < MATRIX_WIDTH; j++, k++)
            cout << keyMatrix[i][j] << " ";
        
        cout << endl;
    }
    
    return true;
}

string Hill::encrypt(const string& plaintext)
{
	string ciphertext = "";
	row = MATRIX_WIDTH;
    
	int testCol = plaintext.length() % row;
    
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
				this->matrix[i][j] += keyMatrix[i][k] * matrix[k][j];
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
            
			char returnLetter = static_cast<char>('a' + tempNum);
			stringstream ss;
			string tempEl;
			ss << returnLetter;
			ss >> tempEl;
			ciphertext.push_back(tempEl);
		}
	}
    
	return ciphertext;
}

string Hill::decrypt(const string& ciphertext)
{
//	vector <int> enterKey;
//	string ciphertext = "";
//	stringstream stream(key);
//	int Num;
//	while(stream >> Num)
//	{
//		enterKey.push_back(Num);
//	}
    return "";
}

//create the matrix when we know the row and the column
void Hill::createMatrix()
{
	matrix = new char*[row];
	for(int i = 0; i < row; ++i)
	{
		matrix[i] = new char[col];
	}
	
	for(int i = 0; i < row; ++i)
	{
		for(int j = 0; j < col; ++j)
		{
			matrix[i][j] = -1;
		}
	}
}


int Hill::charToNum(const char& letter)
{
    return (LOWER_ALPHA_ASCII_BEGIN + letter) % ALPHABETH_COUNT;
}


