
#include "RSA.h"
#define MOD_SIZE  2048
/**
 * Sets the key to use
 * @param key - the key to use
 * @return - True if the key is valid and False otherwise
 */
bool RSA_433::setKey(const unsigned char* key)
{
	
	/* Open the key file */
	FILE* fp = fopen((const char*)key, "r");
	
	/* Error checks */
	if(!fp)
	{
		perror("fopen");
		return false;		
	}


	/* Read the public key */
	this->RSA_key = PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
	fclose(fp);
	
	if(!this->RSA_key){
	        
		/* Else read the private key */		
		FILE* fp = fopen((const char*)key, "r");
	        this->RSA_key = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
		fclose(fp);
		
		if(!this->RSA_key){
			fprintf (stdout, "ERROR: Key is not valid\n");
			return false;
		}
		else {
			this->isPublicKey = false;
		}
	}
	else {
		this->isPublicKey = true;
	}
		
	return true;
}

/**	
 * Encrypts a plaintext string
 * @param plaintext - the plaintext string
 * @param size - the size of the plaintext
 * @param encryptLen - the number of bytes encrypted
 * @return - the encrypted ciphertext string
 */
unsigned char* RSA_433::encrypt( const unsigned char* plaintext, const int& size, int& encryptLen )
{
	printf("EPT: RSA_Encrypt\n");
	//fprintf(stderr, "size: %d\n", size);	

	unsigned char* cipherText = (unsigned char*)calloc(MOD_SIZE, sizeof(char));
	
	/* The buffer containing the error message */
	char errorBuff[130];

	if ( this->isPublicKey){
		/* Encrypt the message using public key */
		encryptLen = RSA_public_encrypt(size, plaintext, cipherText, this->RSA_key, RSA_PKCS1_PADDING);
	}
	else{
		/* Encrypt the message using private key */
		encryptLen = encryptLen = RSA_private_encrypt(size, plaintext, cipherText, this->RSA_key, RSA_PKCS1_PADDING);
	}

	/* Check error */
	if(encryptLen < 0) 
	{
	    ERR_load_crypto_strings();
	    ERR_error_string(ERR_get_error(), errorBuff);
	    fprintf(stderr, "Error encrypting message: %s\n", errorBuff);
	    free(cipherText);
	    return NULL;
	}
		
	return cipherText;
}

/**
 * Decrypts a string of ciphertext
 * @param ciphertext - the ciphertext
 * @param size - the size of the ciphertext
 * @param decryptLen - the number of bytes decrypted
 * @return - the plaintext
 */
unsigned char* RSA_433::decrypt( const unsigned char* ciphertext, const int& size, int& decryptLen)
{
	printf("EPT: RSA_Decrypt\n");
	unsigned char* decryptedText = (unsigned char*)calloc(MOD_SIZE, sizeof(char));
	/* The buffer containing the error message */
	char errorBuff[130];

	if ( this->isPublicKey){
		/* Decrypt the message using public key */
		decryptLen = RSA_public_decrypt(size, ciphertext, decryptedText, this->RSA_key, RSA_PKCS1_PADDING);
	}
	else{
		/* Decrypt the message using private key */
		decryptLen = RSA_private_decrypt(size, ciphertext, decryptedText, this->RSA_key, RSA_PKCS1_PADDING);
	}
	
	/* Check error */
	if(decryptLen < 0) 
	{
	    ERR_load_crypto_strings();
	    ERR_error_string(ERR_get_error(), errorBuff);
	    fprintf(stderr, "Error decrypting message: %s\n", errorBuff);
	    free(decryptedText);
	    return NULL;
	}
		
	return decryptedText;
}

