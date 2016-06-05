#include "Playfair.h"

using namespace std;

typedef struct PAIR {
  char first;
  char second;
} pairType;

const int MAXROW = 4;
const int MINROW = 0;
const int MAXCOL = 4;
const int MINCOL = 0;

/**
 * Sets the key to use
 * @param key - the key to use
 * @return - True if the key is valid and False otherwise
 */
bool Playfair::setKey(const string& key)
{

  bool isValid = true;
  // Verify if the key contains only alphabets
  string text = key;
  text.erase(remove(text.begin(), text.end(), ' '), text.end());
  for ( int i = 0; i < text.length(); i++){
    if ( !isalpha(text[i])){
      isValid = false;
      break;
    }
  }

  if ( isValid) {
    createMatrix(text);
  }
  return isValid;  
}


/**	
 * Encrypts a plaintext string
 * @param plaintext - the plaintext string
 * @return - the encrypted ciphertext string
 */
string Playfair::encrypt(const string& plaintext)
{
  
  string text = "";

  // the plaintext should contains only letters
  for ( int i = 0; i < plaintext.length(); i++){
	  if ( isalpha(plaintext[i])) text.push_back(plaintext[i]);
  }

  // lowercase text
  transform(text.begin(), text.end(), text.begin(), ::tolower);
  pairType a_pair;
  vector<pairType> plainText;
  for ( int i = 0; i < text.length()-1; i++){
    a_pair.first = text[i];
    a_pair.second = text[i+1];
    if ( a_pair.first == a_pair.second ) {
      a_pair.second = 'x';
      plainText.push_back(a_pair);
    }
    else {
      plainText.push_back(a_pair);
      i++;
    }
  }
  
  char lastLetter;
  lastLetter = text[text.length()-1];
  if ( a_pair.second != lastLetter ){
    a_pair.first = lastLetter;
    a_pair.second =  'x';
    plainText.push_back(a_pair);
  }


  vector<pairType>::iterator it;
  char a;
  char b;
  int row_a, row_b, col_a, col_b;
  string cipherText = "";
  for ( it = plainText.begin(); it != plainText.end(); it++){
    a_pair = *it;
    a = a_pair.first;
    b = a_pair.second;
    row_a = getRow(a);
    row_b = getRow(b);
    col_a = getCol(a);
    col_b = getCol(b);
    if ( row_a != -1 && row_b != -1 && col_a != -1 && col_b != -1) {
	    if ( row_a == row_b ){
		    if ( col_a == MAXCOL ) a = matrix[row_a][MINCOL];
		    else a = matrix[row_a][col_a + 1];
		    if ( col_b == MAXCOL ) b = matrix[row_b][MINCOL];
		    else b = matrix[row_b][col_b+1];
	    }
	    else if ( col_a == col_b ){
		    if ( row_a  == MAXROW ) a = matrix[MINROW][col_a];
		    else a = matrix[row_a+1][col_a];
		    if ( row_b == MAXROW ) b = matrix[MINROW][col_b];
		    else b = matrix[row_b+1][col_b];
	    }
	    else {
		    a = matrix[row_a][col_b];
		    b = matrix[row_b][col_a];
	    }
	    cipherText.push_back(a);
	    cipherText.push_back(b);
    }
  }

  transform(cipherText.begin(), cipherText.end(), cipherText.begin(), ::toupper);
  return cipherText; 
}

/**
 * Decrypts a string of ciphertext
 * @param cipherText - the ciphertext
 * @return - the plaintext
 */
string Playfair::decrypt(const string& cipherText)
{

  string text = cipherText;
  text.erase(remove(text.begin(), text.end(), ' '), text.end());
  transform(text.begin(), text.end(), text.begin(), ::tolower);
  
  // Replace j with i
  size_t pos = 0;
  while ((pos = text.find("j", pos)) != string::npos) {
    text.replace(pos, 1, "i");
    pos += 1;
  }
  
  pairType a_pair;
  vector<pairType> ciphertext;
  for ( int i = 0; i < text.length()-1; i++){
    a_pair.first = text[i];
    a_pair.second = text[++i];
    ciphertext.push_back(a_pair);
  }
  
  char lastLetter = text[text.length()-1];
  if ( a_pair.second != lastLetter ){
    a_pair.first = lastLetter;
    a_pair.second =  'x';
    ciphertext.push_back(a_pair);
  }
  
  vector<pairType>::iterator it;
  char a;
  char b;
  int row_a, row_b, col_a, col_b;
  string plainText = "";
  for ( it = ciphertext.begin(); it != ciphertext.end(); it++){
    a_pair = *it;
    a = a_pair.first;
    b = a_pair.second;
    row_a = getRow(a);
    row_b = getRow(b);
    col_a = getCol(a);
    col_b = getCol(b);
    if ( row_a != -1 && row_b != -1 && col_a != -1 && col_b != -1) {
	    if ( row_a == row_b ){
		    if ( col_a == MINCOL ) a = matrix[row_a][MAXCOL];
		    else a = matrix[row_a][col_a-1];
		    if ( col_b == MINCOL ) b = matrix[row_b][MAXCOL];
		    else b = matrix[row_b][col_b-1];
	    }
	    else if ( col_a == col_b ){
		    if ( row_a  == MINROW ) a = matrix[MAXROW][col_a];
		    else a = matrix[row_a-1][col_a];
		    if ( row_b == MINROW ) b = matrix[MAXROW][col_b];
		    else b = matrix[row_b-1][col_b];
	    }
	    else {
		    a = matrix[row_a][col_b];
		    b = matrix[row_b][col_a];
	    }
	    plainText.push_back(a);
	    plainText.push_back(b);
    }
  }

  return plainText; 
	
}


void Playfair::createMatrix(const string& key){

  string temp = key + "abcdefghijklmnopqrstuvwxyz";
  int i = 0;
  // Replace j with i
  size_t pos = 0;
  while ((pos = temp.find("j", pos)) != string::npos) {
    temp.replace(pos, 1, "i");
    pos += 1;
  }

  // Remove duplicate
  map<char, int> m; 
  string Matrix = "";
  for (i = 0; i < temp.length(); i++) {
    if (m.find(temp[i]) == m.end()) {
      m[temp[i]] = 1;
      Matrix += temp[i];
    }
  }

  // Copy Matrix over to the private matrix of the class
  for (i = 0; i < Matrix.length(); i++) {
    matrix[i/5][i%5] = Matrix[i];
  }

  printMatrix();

}

void Playfair::printMatrix(){
  for ( int row = MINROW; row <= MAXROW; row++){
    for ( int col = MINCOL; col <= MAXCOL; col++) cout << matrix[row][col];
    cout << endl;
  }
}

int Playfair::getRow(const char& letter){
  for (int row = MINROW; row <= MAXROW; row++){
    for ( int col = MINCOL; col <= MAXCOL; col++){
      if ( letter == matrix[row][col] ) return row;
    }
  }

  // It should never get here
  return -1;
}

int Playfair::getCol(const char& letter){
  for (int row = MINROW; row <= MAXROW; row++){
    for ( int col = MINCOL; col <= MAXCOL; col++){
      if ( letter == matrix[row][col] ) return col;
    }
  }

  // It should never get here
  return -1;
}
