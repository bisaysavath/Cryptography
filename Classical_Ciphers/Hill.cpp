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
    
    textRow = MATRIX_WIDTH;
    textCol = temp_plaintext.length() / textRow;
    
    // Do all the letters in a plaintext fit in all columns with a given row size?
    if (temp_plaintext.length() % textRow != 0) {
        // If not, plaintextCol gets one more column
        textCol += 1;
    }
    
    // Create a matrix to hold plaintext
    createCharMatrix(textRow, textCol, temp_plaintext);
    
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
    
    for (int charMatrixCol = 0; charMatrixCol < textCol; charMatrixCol++)
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
    // Get the inverse matrix key
    if ( !createInveseMatrix() )
    {
        return "";
    }
    
    // Convert ciphertext to uppercase
    string temp_ciphertext = ciphertext;
    transform(temp_ciphertext.begin(), temp_ciphertext.end(), temp_ciphertext.begin(), ::toupper);
    
    textRow = MATRIX_WIDTH;
    textCol = temp_ciphertext.length() / textRow;

    // Create a matrix to hold plaintext
    createCharMatrix(textRow, textCol, temp_ciphertext);
    
    // Decrypt using Matrix multiplication
    string plaintext = "";
    
    /**
     charMatrix will be setup by,
     say 3 cols by 5 rows, we'd have
     
     0 1 2 3 4
     0 [ | | | | ]
     1 [ | | | | ]
     2 [ | | | | ]
     
     this makes it easier to do a matrix maltiplication
     **/
    
    for (int charMatrixCol = 0; charMatrixCol < textCol; charMatrixCol++)
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
            plaintext += numToChar(matrixOutput[index]);
        }
    }
    
    cout << setw(OUTPUT_WIDTH) << left << "Ciphertext: " << temp_ciphertext << endl;
    cout << setw(OUTPUT_WIDTH) << left << "Plaintext: " << plaintext << endl;
    
    return plaintext;
}

/**
 * Creates a char matrix with a given row, col and text
 * Pad the column with 'X' if the column is not filled up
 * @param row - row needed, col - column needed, text - text to be input
 * @return - none
 */
void Hill::createCharMatrix(const int& row, const int& col, const string& text)
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
    int textLength = text.length();

    for(int i = 0; i < col; i++)
    {
        charMatrix[i] = new int[row];
        
        for (int j = 0; j < row; j++, k++) {
            if (textLength > k) {
                charMatrix[i][j] = charToNum(text[k]);
            }
            else {
                // Padding the extra column
                charMatrix[i][j] = 23; // Padding the column with a character 23 = 'X'
            }
        }
    }
}

/**
 * Creates an inverse matrix for a key during a decryption process
 * @param none
 * @return - boolean value whether a process is sussessful or not
 */
bool Hill::createInveseMatrix(void)
{
    /**
                   [e|f] [d|f] [d|e]
                  +[h|i]-[g|i]+[g|h]
         [a|b|c]   [b|c] [a|c] [a|b]
     A = [d|e|f] =-[h|i]+[g|i]-[g|h] mod 26
         [g|h|i]   [b|c] [a|c] [a|b]
                  +[e|f]-[d|f]+[d|e]
     
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
    
    int determinant = ((a * (e * i - f * h)) - (b * (d * i - f * g)) + (c * (d * h - e * g))) % ALPHABETH_COUNT;
    int inverseOfDeterminant = -1;
    
    for (int count = 0; count < ALPHABETH_COUNT; count++) {
        if ((determinant * count) % ALPHABETH_COUNT == 1) {
            inverseOfDeterminant = count;
            break;
        }
    }
    
    if (inverseOfDeterminant == -1) {
        cout << "Error: the key matrix is invertible. Program is terminating." << endl;
        return false;
    }
    
    keyMatrix[0][0] = (e * i - f * h);      // a
    keyMatrix[0][1] = (d * i - f * g) * -1; // b
    keyMatrix[0][2] = (d * h - e * g);      // c
    keyMatrix[1][0] = (b * i - c * h) * -1; // d
    keyMatrix[1][1] = (a * i - c * g);      // e
    keyMatrix[1][2] = (a * h - b * g) * -1; // f
    keyMatrix[2][0] = (b * f - c * e);      // g
    keyMatrix[2][1] = (a * f - c * d) * -1; // h
    keyMatrix[2][2] = (a * e - b * d);      // i
    
    /**
     doing a transpose
         [a|d|g]
     A = [b|e|h]
         [c|f|i]
     **/
    
    a = keyMatrix[0][0];
    b = keyMatrix[0][1];
    c = keyMatrix[0][2];
    d = keyMatrix[1][0];
    e = keyMatrix[1][1];
    f = keyMatrix[1][2];
    g = keyMatrix[2][0];
    h = keyMatrix[2][1];
    i = keyMatrix[2][2];
    
    keyMatrix[0][1] = d; // b becomes d
    keyMatrix[0][2] = g; // c becomes g
    keyMatrix[1][0] = b; // d becomes b
    keyMatrix[1][2] = h; // f becomes h
    keyMatrix[2][0] = c; // g becomes c
    keyMatrix[2][1] = f; // h becomes f
    
    // keyMatrix % 26, keyMatrix multiplies the inverseOfDeterminant and mod to 26 again
    for(int i = 0; i < MATRIX_WIDTH; i++)
    {
        for(int j = 0; j < MATRIX_WIDTH; j++)
        {
            int mod = keyMatrix[i][j] % ALPHABETH_COUNT;
            // When modula returns nagative output
            if (mod < 0) {
                keyMatrix[i][j] = mod + ALPHABETH_COUNT;
            }
            else
            {
                keyMatrix[i][j] = mod;
            }
            
            keyMatrix[i][j] *= inverseOfDeterminant;
            keyMatrix[i][j] %= ALPHABETH_COUNT;
        }
    }
    
    return true;
}

/**
 * Takes a character type and return the position of it in the alphabeth
 * @param letter - a char type letter
 * @return - position of it in the alphabeth
 */

int Hill::charToNum(const char& letter)
{
    return (UPPER_ALPHA_ASCII_BEGIN + letter) % ALPHABETH_COUNT;
}

/**
 * Takes a position of it in the alphabeth and return a character
 * @param num - position in the alphabeth
 * @return - a char type letter
 */
char Hill::numToChar(const int& num)
{
    return UPPER_ALPHA_ASCII_BEGIN + (num % ALPHABETH_COUNT);
}


