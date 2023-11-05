#ifndef _FileHandler_H
#define _FileHandler_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "File.h"
#include "SignalProxy.h"

using namespace std;

/// <summary>
/// Class that represent shredding multiple files
/// </summary>
class FileHandler : public Observer {
private:
	unordered_map<wstring, File*> fileDictionary; //file dictionary for wipe
	size_t fileDictionarySize; //represents the size of dictionary
	int numOfThreads = 0; //represents the current amount of running threads
	bool threadsRunning = false; //flag for indicating if threads are running
	bool wipe = false; //flag for indicating if we wipe
	bool decrypt = false; //flag for indicating if we decrypt
	recursive_mutex threadMutex; //mutex for thread-safe operations
	SignalProxy* signal; //signal object for communicating with GUI

public:
	FileHandler(const vector<string> filePathList, SignalProxy* signal);
	virtual ~FileHandler();
	void initWipe(int passes=1, bool toRemove=false);
	void initCipher(const string& key, bool decrypt=false);
	void update(Observable* observable) override;
	void cancelProcess();
	bool areThreadsRunning() { return this->threadsRunning; }
	int getNumOfThreads() { return this->numOfThreads; }
	void setThreadsRunning(bool state) { this->threadsRunning = state; }
	void setNumOfThreads(int number) { this->numOfThreads = number; }
};
#endif