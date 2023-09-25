#ifndef _Shredder_H
#define _Shredder_H
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

class Shredder : public Observer {
private:
	unordered_map<string, File*> fileDictionary; //file dictionary for wipe
	size_t fileDictionarySize; //represents the size of dictionary
	int passes; //represents the number of passes for wipe
	bool toRemove; //indication if we need to remove files after wipe
	int numOfThreads = 0; //represents the current amount of running threads
	bool threadsRunning = false; //flag for indicating if threads are running
	recursive_mutex threadMutex; //mutex for thread-safe operations
	SignalProxy* signal; //signal object for communicating with GUI

public:
	Shredder(const vector<string> filePathList, int passes, bool toRemove, SignalProxy* signal);
	virtual ~Shredder();
	void initShred();
	void update(Observable* observable) override;
	void cancelWipe();
	bool areThreadsRunning() { return this->threadsRunning; }
	int getNumOfThreads() { return this->numOfThreads; }
	void setThreadsRunning(bool state) { this->threadsRunning = state; }
	void setNumOfThreads(int number) { this->numOfThreads = number; }
};
#endif