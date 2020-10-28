//============================================================================
// Name        : cfm.cpp
// Author      : Teo Kok Yong
// Version     :
// Copyright   : 
// Description : La Cipher-Feedback Mode, TSC Cipher
//============================================================================


#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

struct ABArray{
	unsigned char *  A;
	unsigned char *  B;
};

vector <unsigned char> ReadFile(const char * filePath);
ABArray KeyScheduling(vector<unsigned char> key);
vector <unsigned char> PseudoRandomNumberGenerator(ABArray ab);
unsigned char TSCEncrypt(vector <unsigned char> keyf, unsigned char plaintext);
unsigned char TSCDecrypt(vector <unsigned char> keyf, unsigned char cipherText);
void Encrypt(const char * key, const char * plainText, const char * cipherText);
void Decrypt(const char * key, const char * cipherText, const char * plainText);

int main(int argc, char *argv[]) {

	if(!strcmp(argv[2],"-e")){
		Encrypt(argv[1], argv[3], argv[4]);
	}
	else if(!strcmp(argv[2],"-d")){
		Decrypt(argv[1], argv[3], argv[4]);
	}
	else{
		cout << "Invalid option. Usage: cfm key [-e/-d] input_file output_file." << endl;
	}
	return 0;
}

void Encrypt(const char * key, const char * plainText, const char * cipherText){
	vector <unsigned char> keyf = ReadFile(key);
	vector <unsigned char> PRNG = PseudoRandomNumberGenerator(KeyScheduling(ReadFile(key)));
	unsigned char TSCOutput; //TSCOutput
	unsigned char IV = 8; //IV
	unsigned char temp1ByteCipher; // 1 byte char to temporary store the cipher
	vector <unsigned char> output; //result of encryption
	vector <unsigned char> plain = ReadFile(plainText);
	unsigned int counter = 0;
 	
	ofstream debugfile("debug.txt");
	debugfile << plain.size() << endl;
	while(counter != plain.size()){
		if(counter == 0){
			//TSC Encrypt key with IV
			TSCOutput = PRNG[0] ^ IV; //get the 1 byte cipher text to xor with plain text
			temp1ByteCipher = plain[counter] ^ TSCOutput; 
		}else{
			TSCOutput = PRNG[0] ^ temp1ByteCipher;
			temp1ByteCipher = plain[counter] ^ TSCOutput;
		}	
		
		unsigned char cipherChar = plain[counter] ^ temp1ByteCipher;
		
		if(cipherChar == 26){
			cipherChar = plain[counter]; //EOF symbol
		}
		
		output.push_back(cipherChar);
		
		debugfile << counter << " : " << plain[counter] << " : " << temp1ByteCipher << " : "  << cipherChar << endl;
		
		counter++;
	}
	debugfile.close();
	
	//write to file
	ofstream outfile(cipherText);
	for(unsigned int i=0;i<output.size();i++){
		outfile << output[i];
	}
	outfile.close();
}

void Decrypt(const char * key, const char * cipherText, const char * plainText){
	vector <unsigned char> keyf = ReadFile(key);
	vector <unsigned char> PRNG = PseudoRandomNumberGenerator(KeyScheduling(ReadFile(key)));
	unsigned char TSCOutput; //TSCOutput
	unsigned char IV = 8; //IV
	vector <unsigned char> TSCOutputStore;
	vector <unsigned char> output;
	vector <unsigned char> cipher = ReadFile(cipherText);
	unsigned int counter = 0;
	
	ofstream debugfile("debug.txt");
	debugfile << cipher.size() << endl;
	
	while(counter != cipher.size()){
		if(counter == 0){
			TSCOutput = PRNG[0] ^ IV;
		}else{
			TSCOutput = PRNG[0] ^ cipher[counter-1];
		}	

		TSCOutputStore.push_back(TSCOutput);
		
		counter++;
	}
	
	for(unsigned int i=0; i < cipher.size(); i ++){
		unsigned char plainChar = TSCOutputStore[i+2] ^ cipher[i];
		output.push_back(plainChar);
		debugfile << i << " : " << cipher[i] << ":" << TSCOutputStore[i+2] << " : " << plainChar << endl;
	}	
	debugfile.close();
	
	//write to file
	ofstream outfile(plainText);
	for(unsigned int i=0;i<output.size();i++){
		outfile << output[i];
	}
	outfile.close();
}

vector <unsigned char> ReadFile(const char * filePath){
	ifstream infile(filePath);
	vector <unsigned char> fileCharVector;
	char ch;
	
	if(infile.is_open()){
		while(infile.get(ch)){
			fileCharVector.push_back(static_cast<unsigned char>(ch));
		}
	}else{
		cout << "filePath is not a valid file." << endl;
	}
	infile.close();
	
	/*for(unsigned int i=0; i <fileCharVector.size(); i++){
		cout << fileCharVector[i];
	}*/
	
	return fileCharVector;
}

ABArray KeyScheduling(vector<unsigned char> key){
	unsigned char * A = new unsigned char[256];
	unsigned char * B = new unsigned char[256];
	
	for(int i=0; i< 256; i++){
		A[i] = i;
		B[i] = i;
	}
	
	int j = 0;
	int k = 0;
	for(int i=0;i<256;i++){
		j = (j+A[i]+key[i%key.size()])%256;
		//swapping
		int temp = A[i];
		A[i] = A[j];
		A[j] = temp;
		
		k = (j+k+A[i]+B[i]+key[i%key.size()])%256;
		B[i] = (A[j]+B[k])%256;
	}
	
	ABArray r;
	r.A = A;
	r.B = B;
	
	return r;
}

vector <unsigned char> PseudoRandomNumberGenerator(ABArray ab){
	vector<unsigned char> generatedResult;
	
	int i = 0;
	int j = 0;
	for(int k=0; k< 256; k++){
		i = (i+1)% 256;
		j = (j+ab.A[i]+ab.B[i])%256;
		int temp = ab.A[i];
		ab.A[i] = ab.B[j];
		ab.B[j] = temp;
		
		generatedResult.push_back(ab.A[(ab.A[i]+ab.B[j])%256]);
	}
	
	return generatedResult;
}
