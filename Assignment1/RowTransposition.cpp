#include "RowTransposition.h"
#include <string>
#include <sstream>
bool RowTransposition::setKey(const string& key)
{
	//create a vector to hold all the element of the key
	vector <int> testKey;
	int tempKey = 0;
	// test the key if it has the numbers from 1-9	
	if(key.find_first_not_of("123456789") == string::npos)
	{
		// push all element of the key to vector testkey
		for(int i = 0; i < key.length(); ++i)
		{
			//convert each of the string element to integer and
			// add into vector testkey
			char a = key.at(i);
			int element = a - '0';
			testKey.push_back(element);
			//convert the key string to tempkey integer
			tempKey = tempKey*10 + (key[i] -'0');
		}
		//sorted the test key
		sort(testKey.begin(), testKey.end());
		// check if the first element of the testkey has to start with 1
		if(testKey[0] == 1)
		{	// test the whole testKey to know it is from least to greatest or ascending order
			// and it has to be the arithmetic sequence 1,2,3,4,5...n or linear nature with common different d =1
			for(int i = 0; i < testKey.size()-1; ++i)
			{
				if((testKey[i+1] - testKey[i]) != 1)
				{
					return false;
				}
			}
		}
		// if the first element of  testkey is not start with 1 return false
		else
		{
			return false;
		}
	}
	// return false if the key don't have all the numbers from 1-9
	else
	{
		return false;
	}
//make the keyNum = to the tempKey ( both in integer format)
this->keyNum = tempKey;
return true;


}

string RowTransposition::encrypt(const string& plaintext)
{	// convert the keyNum to string key
	string key;
	ostringstream convert;
	convert << keyNum;
	key = 	convert.str();
	//col's length of matrix
	col = key.length();
	//testRow represent to remainder of plaintext.length() and col
	int testRow = plaintext.length() % col;
	// create string ciphertext
	string ciphertext = "";
	// figure out the total row need to create
	row = 0;
	// when we have remainder it means testRow is not equal to 0
	if(testRow != 0)
	{	
		row = plaintext.length() / col + 1;
	}
	//when testRow = 0
	else
	{
		row = plaintext.length() / col;
	}
	// create matrix has row and col for row transposition 
	createMatrix();	
	//put all the element of plaintext into matrix
	for(int i = 0; i < plaintext.length(); ++i)
	{
		this->matrix[i / col][i % col] = plaintext[i];
	}
	//fill out x for the test of the matrix if testRow is not equal to 0
	if(testRow != 0)
	{
		for(int j = testRow; j < col; ++j)
		{
			this->matrix[row - 1][j] = 'x';
		}
	}
	//encrypt the plaintext by they key provided
	for(unsigned int i = 0; i < key.length(); ++i)	
	{
		char a = key.at(i);
		int element = a - '0';
		for(int j = 0; j < row; ++j)
		{
			ciphertext.push_back(this->matrix[j][element - 1]);
		}
	}
	return ciphertext;
}

//decrypt the ciphertext
string RowTransposition::decrypt(const string& ciphertext)
{	//convert the int keyNum(key provided) into string key for easy to use later
	string key;
	ostringstream convert;
	convert << keyNum;
	key = convert.str();
	col = key.length();
	row = ciphertext.length() / col;
	string plaintext = "";
	// create the matrix has row and col above
	createMatrix();
	//put the ciphertext into matrix by key provided
	unsigned int getThr = 0;
	while(getThr < ciphertext.length())
	{
		for(unsigned int j = 0; j < key.length(); ++j)
		{
			char a = key.at(j);
			int  element = a - '0';
			for (int k = 0; k < row; ++k)
			{
				this->matrix[getThr % row][element - 1] = ciphertext[getThr];
			++getThr;		//increase the element of the string ciphertext
			}
		}
	}
	// put all the element of the matrix into plaintext
	// or decrypt the ciphertext is done
	for(int i = 0; i < row; ++i)
	{
		for(int j = 0; j < col; ++j)
		{
			plaintext.push_back(this->matrix[i][j]);
		}
	}
	return plaintext;

}	


// create the matrix when we know the row and the col 
void RowTransposition::createMatrix()
{
	this->matrix = new char*[row];
	for(int i = 0; i < row; ++i)
	{
		this->matrix[i] = new char[col];
	}
	
	for (int i = 0; i < row; ++i)
	{
		for(int j = 0; j < col; ++j)
		{
			matrix[i][j] = '\0';
		}
	}
}

RowTransposition::~RowTransposition()
{
	//clear the matrix
	for(int i = 0; i < row; ++i)
	{
		delete[] matrix[i];
	}
	delete [] matrix;
}
