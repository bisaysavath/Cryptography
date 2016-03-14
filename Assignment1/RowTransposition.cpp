#include "RowTransposition.h"
#include <string>
#include <sstream>
bool RowTransposition::setKey(const string& key)
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
this->keyNum = key;
return true;


}

string RowTransposition::encrypt(const string& plaintext)
{
	
	vector <int> key;
	stringstream stream(this->keyNum);
	int Num;
	while(stream >> Num)
	{
		key.push_back(Num);
	}
	//col's length of matrix
	col = key.size();
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
	for(unsigned int i = 0; i < key.size(); ++i)	
	{
		for(int j = 0; j < row; ++j)
		{
			ciphertext.push_back(this->matrix[j][key[i] - 1]);
		}
	}
	return ciphertext;
}

//decrypt the ciphertext
string RowTransposition::decrypt(const string& ciphertext)
{	//convert the int keyNum(key provided) into string key for easy to use later
	vector<int> key;
	stringstream stream(this->keyNum);
	int Num;
	while(stream >> Num)
	{
		key.push_back(Num);
	}	
	col = key.size();
	row = ciphertext.length() / col;
	string plaintext = "";
	// create the matrix has row and col above
	createMatrix();
	//put the ciphertext into matrix by key provided
	unsigned int getThr = 0;
	while(getThr < ciphertext.length())
	{
		for(unsigned int j = 0; j < key.size(); ++j)
		{
			for (int k = 0; k < row; ++k)
			{
				this->matrix[getThr % row][key[j] - 1] = ciphertext[getThr];
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
