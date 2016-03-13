#include "Railfence.h"

bool Railfence::setKey(const string& key){

	int temp_key = 0;
	for ( int i = 0; i < key.length(); i++){
		if ( !isdigit(key[i])){
			cout << "Key is not valid!" <<endl;
			return false;
		}
		else {
			temp_key = temp_key*10 + (key[i] - '0');
		}
	}
        
	this->key = temp_key%26;
	return true;
}

string Railfence::encrypt(const string& plaintext){

	row = key;
	string text = "";
	
	for ( int i = 0; i < plaintext.length(); i++){
		if ( isalpha(plaintext[i])) text.push_back(plaintext[i]);
	}
	
	transform(text.begin(), text.end(), text.begin(), ::tolower);
	int len = text.length();
	col = 0;
	string ciphertext = "";
	if ( len%row > 0 ) col =  len/row +1;
	else col = len/row;

	createMatrix();

	// copy the plaintext over to the matrix
	for ( int i = 0; i < len; i++){

		this->matrix[i%row][i/row] = text[i];
        
	}

	printMatrix();

	for ( int i = 0; i < row; i++){
		for ( int j = 0; j < col; j++){
			if ( this->matrix[i][j] != ' ' ) ciphertext.push_back(this->matrix[i][j]);
		}
	}

	transform(text.begin(), text.end(), text.begin(), ::toupper);

	return ciphertext;
}

string Railfence::decrypt(const string& ciphertext){
	
	row = key;
	string text = ciphertext;
	int len = ciphertext.length();
	col = 0;
	string plaintext = "";
	int extra;
	if ( len%row > 0 ) {
		col =  len/row +1;
		extra = len%row;
	}
	else col = len/row;

	// Convert all charaters to lowercase
	transform(text.begin(), text.end(), text.begin(), ::tolower);
	
	// create a matrix to store the plaintext
        createMatrix();

	int number_of_column = col;
	int index = 0;
	// copy the ciphertext over to the matrix
	for ( int i = 0; i < row; i++){
		for ( int j = 0; j < number_of_column; j++){
			this->matrix[i][j] = text[index];
			index++;
		}
		extra--;
		if ( extra == 0 ) number_of_column--;
		
	}
	
	for ( int j = 0; j < col; j++){
		for ( int i = 0; i < row; i++){
			if ( this->matrix[i][j] != ' ' ) plaintext.push_back(this->matrix[i][j]);        
		}
	}

	return plaintext;
}

void Railfence::createMatrix(){

	// create a matrix to store the text
        this->matrix = new char*[row];
	for ( int i = 0; i < row; i++ ) this->matrix[i] = new char[col];

	// Initialize the matrix
	for ( int i = 0; i < row; i++ ){
		for ( int j = 0; j < col; j++) {
			matrix[i][j] = ' ';
		}
	}

	
}
Railfence::~Railfence(){

       	// clear the matrix
	for(int i = 0; i < row; ++i) {
		delete [] matrix[i];
	}
	delete [] matrix;
}

void Railfence::printMatrix(){

	for ( int i = 0; i < row; i++ ){
		for ( int j = 0; j < col; j++) {
			cout << matrix[i][j];
		}
		cout << endl;
	}

}
