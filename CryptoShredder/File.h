#ifndef _File_H
#define _File_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <random>
#include "Observer.h"
#include "AES/AES.h"

using namespace std;

/**
 * @brief Class that represent a File object.
 */
class File : public Observable {
private:
	u16string name; //represents file name
	u16string extention; //represents file extention
	u16string fullName; //represents file full name
	u16string fullPath; //represents file path
	size_t length; //represents file length
	static bool isCanceled; //static flag for canceled wipe
	static bool isFailed; //static flag for failed operation

public:
	File(const filesystem::path& filePath);
	virtual ~File() {}
	u16string GetName() const { return this->name; }
	u16string GetExtention() const { return this->extention; }
	u16string GetFullName() const { return this->fullName; }
	u16string GetFullPath() const { return this->fullPath; }
	size_t GetLength() const { return this->length; }
	static void RemoveFile(File* file);
	static void WipeFile(File* file, int passes = 1, bool toRemove = false);
	static void CipherFile(File* file, const string& key, bool decrypt = false);
	static bool GetIsCanceled() { return File::isCanceled; }
	static void SetIsCanceled(bool state) { File::isCanceled = state; }
	static bool GetIsFailed() { return File::isFailed; }
	static void SetIsFailed(bool state) { File::isFailed = state; }
};
#endif