#ifndef _FileHandler_H
#define _FileHandler_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "File.h"
#include "SignalProxy.h"

using namespace std;

/**
 * Class that represent a FileHandler object for handling file operations like wipe and cipher.
 */
class FileHandler : public Observer {
private:
	vector<File*>& fileList; //vector that represents chosen files as objects
	SignalProxy* signal = NULL; //signal object for communicating with GUI
	recursive_mutex threadMutex; //mutex for thread-safe operations
	size_t numOfThreads = 0; //represents the current amount of running threads
	bool threadsRunning = false; //flag for indicating if threads are running
	bool wipe = false; //flag for indicating if we wipe
	bool decrypt = false; //flag for indicating if we decrypt

public:
	FileHandler(vector<File*>& fileList, SignalProxy* signal);
	virtual ~FileHandler();
	void InitWipe(int passes = 1, bool toRemove = false);
	void InitCipher(const string& key, bool decrypt = false);
	void Update(Observable* observable, bool state = true) override;
	void CancelProcess();
	bool AreThreadsRunning() const { return this->threadsRunning; }
	size_t GetNumOfThreads() const { return this->numOfThreads; }
	void SetThreadsRunning(bool state) { this->threadsRunning = state; }
	void SetNumOfThreads(size_t number) { this->numOfThreads = number; }
};
#endif