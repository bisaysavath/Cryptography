#include "Hill.h"

const int OUTPUT_WIDTH = 15;

const int ALPHABETH_COUNT = 26;
const int UPPER_ALPHA_ASCII_BEGIN = 65;
const int MATRIX_WIDTH = 3; // 3 is a good size

/**
 * Sets the key to use
 * @param key - the key to use
 * @return - True if the key is valid and False otherwise
 */
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
    
    if (getMatrixDeterminant() % ALPHABETH_COUNT == 0) {
        cout << "WARNING: This key matrix is not invertible. Thus, decryption is not possible." << endl;
        cout << "Would you like to continue? (Y/N): " << endl;
        char ans;
        cin >> ans;
        return (ans == 'Y') ? true : false;
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

/**
 * Encrypts a plaintext string
 * @param plaintext - the plaintext string
 * @return - the encrypted ciphertext string
 */
string Hill::encrypt(const string& plaintext)
{
    // Convert plaintext to uppercase
    string temp_plaintext = plaintext;
    transform(temp_plaintext.begin(), temp_plaintext.end(), temp_plaintext.begin(), ::toupper);
    
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
    
    cout << setw(OUTPUT_WIDTH) << left << "Plaintext: " << temp_plaintext << endl;
    cout << setw(OUTPUT_WIDTH) << left << "Ciphertext: " << ciphertext << endl;
    
	return ciphertext;
}

/**
 * Decrypts a string of ciphertext
 * @param ciphertext - the ciphertext
 * @return - the plaintext
 */
string Hill::decrypt(const string& ciphertext)
{
    
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
     this makes it easier to do a matrix maltiplication
     **/
    
	charMatrix = new int*[col];
    
    int k = 0;
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
                charMatrix[i][j] = 23; // Padding the column with a character 23 = 'X'
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

int Hill::getMatrixDeterminant(void)
{
    /**
         [a|b|c]
     A = [d|e|f]
         [g|h|i]
     
     |A| = a (ei -fh) - b(di - fg) + c(dh -eg)
     **/
    
    int a = keyMatrix[0][0];
    int b = keyMatrix[0][1];
    int c = keyMatrix[0][2];
    int d = keyMatrix[1][0];
    int e = keyMatrix[1][1];
    int f = keyMatrix[1][2];
    int g = keyMatrix[2][0];
    int h = keyMatrix[2][1];
    int i = keyMatrix[2][2];
    
    return ((a * (e * i - f * h)) - (b * (d * i - f * g)) + (c * (d * h - e * g)));
}

int Hill::charToNum(const char& letter)
{
    return (UPPER_ALPHA_ASCII_BEGIN + letter) % ALPHABETH_COUNT;
}

char Hill::numToChar(const int& num)
{
    return UPPER_ALPHA_ASCII_BEGIN + (num % ALPHABETH_COUNT);
}


