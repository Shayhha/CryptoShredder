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
	unordered_map<string, File*> fileDictionary;
	int fileDictionarySize;
	int passes;
	bool toRemove;
	int numOfThreads = 0;
	bool threadsRunning = false;
	recursive_mutex threadMutex;
	SignalProxy* signal;

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