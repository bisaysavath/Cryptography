#include "Hill.h"

const int ALPHABETH_COUNT = 26;
const int UPPER_ALPHA_ASCII_BEGIN = 65;
const int MATRIX_WIDTH = 2;
const string UPPER_ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//const string UPPER_ALPHA_DESC = "ZYXWVUTSRQPONMLKJIHGFEDCBA";

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
    
//    for(int i = 0; i < MATRIX_WIDTH; i++)
//    {
//        for(int j = 0; j < MATRIX_WIDTH; j++)
//            cout << keyMatrix[i][j] << " ";
//        
//        cout << endl;
//    }
    
    return true;
}

string Hill::encrypt(const string& plaintext)
{
    // Convert plaintext to uppercase
    string temp_plaintext = plaintext;
    transform(temp_plaintext.begin(), temp_plaintext.end(), temp_plaintext.begin(), ::toupper);
    cout << temp_plaintext << endl;
    
    int plaintextRow = MATRIX_WIDTH;
    int plaintextCol = temp_plaintext.length() / plaintextRow;
    
    // Do all the letters in a plaintext fit in all rows?
    if (temp_plaintext.length() % plaintextRow != 0) {
        // If not, plaintextCol gets one more column
        plaintextCol += 1;
    }
    
    // Create a matrix to hold plaintext
    createCharMatrix(plaintextRow, plaintextCol, temp_plaintext);
    
    // Encrypt using Matrix multiplication
    string ciphertext = "";
    
    /**
     charMatrix will be setup by,
     say 3 cols by 5 rows, we'd have
     
        0 1 2 3 4
     0 [ | | | | ]
     1 [ | | | | ]
     2 [ | | | | ]
     
     this makes it easier to do a matrix maltiplication
     **/
    
    for (int charMatrixCol = 0; charMatrixCol < plaintextCol; charMatrixCol++)
    {
        int matrixOutput[MATRIX_WIDTH] = {};
        
        for (int keyMatrixRow = 0; keyMatrixRow < MATRIX_WIDTH; keyMatrixRow++)
        {
            for (int index = 0; index < MATRIX_WIDTH; index++)
            {
                matrixOutput[keyMatrixRow] += keyMatrix[keyMatrixRow][index] * charMatrix[charMatrixCol][index];
            }
        }
        
        for (int index = 0; index < MATRIX_WIDTH; index++) {
            ciphertext += numToChar(matrixOutput[index]);
        }
    }
    
    cout << ciphertext << endl;
    
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
void Hill::createCharMatrix(const int& row, const int& col, const string& plaintext)
{
    /**
     charMatrix will be setup by,
     say 3 cols by 5 rows, we'd have
     
        0 1 2 3 4
     0 [ | | | | ]
     1 [ | | | | ]
     2 [ | | | | ]
     **/
    
	charMatrix = new int*[col];
    
    int k = 0, l = 0;
    int textLength = plaintext.length();

    for(int i = 0; i < col; i++)
    {
        charMatrix[i] = new int[row];
        
        for (int j = 0; j < row; j++, k++) {
            if (textLength > k) {
                charMatrix[i][j] = charToNum(plaintext[k]);
            }
            else {
                // Padding the extra column
                charMatrix[i][j] = charToNum(UPPER_ALPHA[l]);
                l++;
            }
        }
    }
    
//    for(int i = 0; i < col; i++)
//    {
//        for(int j = 0; j < row; j++)
//            cout << charMatrix[i][j] << " ";
//        
//        cout << endl;
//    }
}


int Hill::charToNum(const char& letter)
{
    return (UPPER_ALPHA_ASCII_BEGIN + letter) % ALPHABETH_COUNT;
}

char Hill::numToChar(const int& num)
{
    return UPPER_ALPHA_ASCII_BEGIN + (num % ALPHABETH_COUNT);
}


