#ifndef _File_H
#define _File_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <future>
#include <random>
#include "Observer.h"
#include "AES/AES.h"

using namespace std;

/// <summary>
/// Class that represent a File object
/// </summary>
class File : public Observable {
private:
	wstring name; //represents file name
	wstring extention; //represents file extention
	wstring fullName; //represents file full name
	wstring fullPath; //represents file path
	size_t length; //represents file length
	static bool isCanceled; //static flag for canceled wipe

public:
	File(const string& filePath, Observer& observer);
	virtual ~File() {}
	static string ToString(const wstring& wstr) { return wstring_convert<codecvt_utf8<wchar_t>, wchar_t>{}.to_bytes(wstr); }
	static wstring ToWString(const string& str) { return wstring_convert<codecvt_utf8<wchar_t>, wchar_t>{}.from_bytes(str); }
	static void removeFile(const File& file);
	static void WipeFile(const File& file, int passes=1, bool toRemove=false);
	static void CipherFile(const File& file, const string& key, bool decrypt=false);
	wstring getName() { return this->name; }
	wstring getExtention() { return this->extention; }
	wstring getFullName() { return this->fullName; }
	wstring getFullPath() { return this->fullPath; }
	size_t getLength() { return this->length; }
	static bool getIsCanceled() { return isCanceled; }
	static void setIsCanceled(bool state) { isCanceled = state; }
};
#endif
