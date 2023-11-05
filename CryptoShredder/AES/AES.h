#ifndef _AES_H
#define _AES_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iomanip> 

using namespace std;

class AES {
private:
	static const unsigned char SBOX[16][16]; //represent the SBOX table for AES
	static const unsigned char INVSBOX[16][16]; //represent the inverse SBOX table for AES
	static const unsigned char GaloisMult[15][256]; //represents the Galois multiplication tables for AES
	static const size_t Nb = 4; //number of columns in the state (always 4 for AES)
	static size_t Nk; //number of 32-bit words in the key 
	static size_t Nr; //number of rounds (AES-128 has 10 rounds, AES-192 has 12 rounds, AES-256 has 14 rounds)
	static const size_t BlockSize = Nb * Nb; //represents the size of AES block that is always 16 bytes (128-bit)

protected:
	static const vector<unsigned char> EncryptBlock(vector<unsigned char>& text, const vector<vector<unsigned char>>& roundKeys);
	static const vector<unsigned char> DecryptBlock(vector<unsigned char>& text, const vector<vector<unsigned char>>& roundKeys);
	static const vector<vector<unsigned char>> KeySchedule(const vector<unsigned char>& key);
	static void SetOperationMode(const size_t keySize);
	static const vector<unsigned char> RotWord(const vector<unsigned char>& word);
	static const vector<unsigned char> SubWord(const vector<unsigned char>& word);
	static const unsigned char Rcon(const unsigned char& value);
	static const vector<unsigned char> SubBytes(vector<unsigned char>& state, const bool inverse=false);
	static const vector<unsigned char> ShiftRows(vector<unsigned char>& state, const bool inverse=false);
	static const vector<unsigned char> MixColumns(vector<unsigned char>& state, const bool inverse=false);
	static const vector<unsigned char> XOR(vector<unsigned char>& first, const vector<unsigned char>& second, const bool immutable=false);
	static const vector<vector<unsigned char>> SplitIntoKeyWords(const vector<unsigned char>& key);

public:
	static const vector<unsigned char> Encrypt(vector<unsigned char>& text, const vector<unsigned char>& key);
	static const vector<unsigned char> Decrypt(vector<unsigned char>& text, const vector<unsigned char>& key);
	static const vector<unsigned char> Encrypt_ECB(vector<unsigned char>& text, const vector<unsigned char>& key);
	static const vector<unsigned char> Decrypt_ECB(vector<unsigned char>& text, const vector<unsigned char>& key);
	static const vector<unsigned char> Encrypt_CBC(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv);
	static const vector<unsigned char> Decrypt_CBC(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv);
	static const vector<unsigned char> Encrypt_CFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv);
	static const vector<unsigned char> Decrypt_CFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv);
	static const vector<unsigned char> Encrypt_OFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv);
	static const vector<unsigned char> Decrypt_OFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv);
	static const vector<unsigned char> Encrypt_CTR(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv);
	static const vector<unsigned char> Decrypt_CTR(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv);
	static void PrintVector(const vector<unsigned char>& vector);
	static void PrintMatrix(const vector<vector<unsigned char>>& matrix);
};
#endif