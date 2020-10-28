//============================================================================
// Name        : ToyStreamCipher.cpp
// Author      : Teo Kok Yong
// Version     :
// Copyright   : 
// Description : Implementation of ToyStreamCipher
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
void Encrypt(const char * keyFile, const char * plainText, const char * cipherText);
void Decrypt(const char * keyFile, const char * cipherText, const char * plainText);

int main(int argc, char *argv[]) {

	if(!strcmp(argv[2],"-e")){
		Encrypt(argv[1], argv[3], argv[4]);
	}
	else if(!strcmp(argv[2],"-d")){
		Decrypt(argv[1], argv[3], argv[4]);
	}
	else{
		cout << "Invalid option. Usage: tsc key [-e/-d] input_file output_file." << endl;
	}
	return 0;
}

void Encrypt(const char * keyFile, const char * plainText, const char * cipherText){
	vector <unsigned char> key = PseudoRandomNumberGenerator(KeyScheduling(ReadFile(keyFile)));
	vector <unsigned char> plain = ReadFile(plainText);
	vector <unsigned char> output;
	
	//ofstream debugfile("debug.txt");
	//debugfile << plain.size() << endl;
	for(unsigned int i=0; i < plain.size(); i++){
		unsigned char plainChar = plain[i];
		unsigned char keyChar = key[i%key.size()];
		unsigned char cipherChar = plainChar ^ keyChar;
		if(cipherChar != 26){ //the dreadful EOF symbol
			output.push_back(cipherChar);
		}else{
			output.push_back(plainChar);
		}
		//debugfile << plainChar << " : "  << cipherChar << endl;
	}
	//debugfile.close();
	
	ofstream outfile(cipherText);
	for(unsigned int i=0;i<output.size();i++){
		outfile << output[i];
	}
	outfile.close();
}

void Decrypt(const char * keyFile, const char * cipherText, const char * plainText){
	return Encrypt(keyFile, cipherText, plainText);
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


