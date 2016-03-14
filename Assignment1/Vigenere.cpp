#include "Vigenere.h"

const int ALPHABETH_COUNT = 26;
const int UPPER_ALPHA_ASCII_BEGIN = 65;
const int OUTPUT_WIDTH = 15;

/** Destructor **/
Vigenere::~Vigenere()
{
   if (this->vigenereSquare) {
       // Clean up Vigenere square's memory
       for (int i = 0; i < ALPHABETH_COUNT; i++) {
           delete [] this->vigenereSquare[i];
       }
   
       delete [] this->vigenereSquare;
   }
}

/**
 * Sets the key to use
 * @param key - the key to use
 * @return - True if the key is valid and False otherwise
 */
bool Vigenere::setKey(const string& key)
{
    for ( string::size_type i = 0; i < key.length(); i++) {
        if ( !isalpha(key[i]) ) return false;
    }
    
    this->key = key;
    
    // Using algorithm library function to convert a string to uppercase
    transform(this->key.begin(), this->key.end(), this->key.begin(), ::toupper);

    return true;
}

/**
 * Encrypts a plaintext string
 * @param plaintext - the plaintext string
 * @return - the encrypted ciphertext string
 */
string Vigenere::encrypt(const string& plaintext)
{
    // Convert plaintext to uppercase
    string tmp_plaintext = plaintext;
    transform(tmp_plaintext.begin(), tmp_plaintext.end(), tmp_plaintext.begin(), ::toupper);
    
    // Using autokey.
    // If key's length is shorter, then concat the plaintext to a key
    for (string::size_type i = 0; this->key.length() < tmp_plaintext.length(); i++)
        this->key += tmp_plaintext[i];
    
    // Create a Vigenere square
    this->createVigenereSqure();
    
    /** Encryption begins **/
    string cipherText;
    
    for (string::size_type i = 0; i < tmp_plaintext.length(); i++) {
        findIntersection(this->key[i], tmp_plaintext[i]);
        cipherText += this->vigenereSquare[intersection.row][intersection.col];
    }
    
    cout << setw(OUTPUT_WIDTH) << left << "Text:" << tmp_plaintext << endl;
    cout << setw(OUTPUT_WIDTH) << left << "Key:" << this->key << endl;
    cout << setw(OUTPUT_WIDTH) << left << "Cipher Text:" << cipherText << endl;
    cout << endl;
    
    return cipherText;
}

/**
 * Decrypts a string of ciphertext
 * @param cipherText - the ciphertext
 * @return - the plaintext
 */
string Vigenere::decrypt(const string& cipherText)
{
    // Convert cipher to uppercase
    string tmp_cipherText = cipherText;
    transform(tmp_cipherText.begin(), tmp_cipherText.end(), tmp_cipherText.begin(), ::toupper);
    
    // Create a Vignere square
    this->createVigenereSqure();
    
    /** Decryption begins **/
    string plaintext;
    
    char encouterLetterFromLeft;
    char encouterLetterFromRight;
    
    int foundAtCol = 0;
    
    for (string::size_type i = 0; i < cipherText.length(); i++) {
        // Find a row of a key letter
        findIntersection(this->key[i]);
        

        // Search for a column contains a plaintext
        for (int col = 0; col < ALPHABETH_COUNT / 2; col++) {
            
            // Look for a letter from left and right side of the column
            encouterLetterFromLeft  = vigenereSquare[intersection.row][col];
            encouterLetterFromRight = vigenereSquare[intersection.row][ALPHABETH_COUNT - col - 1];
            
            if (cipherText[i] == encouterLetterFromLeft) {
                foundAtCol = col;
                break;
            }
            
            if  (cipherText[i] == encouterLetterFromRight) {
                foundAtCol = ALPHABETH_COUNT - col - 1;
                break;
            }
        }

        // Concat a found plaintext since we are using Vigenere Cipher withh autokey
        this->key += vigenereSquare[0][foundAtCol];
        plaintext += vigenereSquare[0][foundAtCol];
    }
    
    cout << setw(OUTPUT_WIDTH) << left << "Ciphertext: " << cipherText << endl;
    cout << setw(OUTPUT_WIDTH) << left << "Key: " << this->key << endl;
    cout << setw(OUTPUT_WIDTH) << left << "Plain text: " << plaintext << endl;
    cout << endl;
    
    return plaintext;
}

/**
 * Create a Vigenere square
 * @param - none
 * @return - none
 */
void Vigenere::createVigenereSqure(void)
{
    // Allocate 26 rows on vigenereSquare
    this->vigenereSquare = new char*[ALPHABETH_COUNT];
    
    for (int row = 0; row < ALPHABETH_COUNT; row++) {
        // Allocate 26 cols on vigenereSquare's row
        this->vigenereSquare[row] = new char[ALPHABETH_COUNT];
        
        for (int col = 0; col < ALPHABETH_COUNT; col++)
            // Save integer that represents a letter in ASCII to vigenereSquare
            // Since it's a char type, an integer turns to a correspond ASCII letter
            this->vigenereSquare[row][col] = UPPER_ALPHA_ASCII_BEGIN + ((col + row) % ALPHABETH_COUNT);
    }
}

/**
 * Look for an intersection of letter and key
 * This function turns an uppercase letter to number ranging from 0(A) - 25(Z)
 * @param key - a key letter, letter - a plaintext letter
 * @return - A Pair struct containing row and col
 */
void Vigenere::findIntersection(const char& key, const char& letter)
{
    intersection.row = (UPPER_ALPHA_ASCII_BEGIN + key) % 26;
    
    // Skip this when letter was not passed in
    if ( letter != '\0' )
        intersection.col = (UPPER_ALPHA_ASCII_BEGIN + letter) % 26;
}
