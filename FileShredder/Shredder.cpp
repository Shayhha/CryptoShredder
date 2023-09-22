#include "Shredder.h"


Shredder::Shredder(const vector<string> filePathList, int passes, bool toRemove, SignalProxy* signal) : Observer() {
	const int fileDictionarySize = filePathList.size(); //get size of filePathList and save it in constant integer for memory safety
	this->signal = signal; //set the signal object for GUI updates
	this->fileDictionarySize = fileDictionarySize; //set the size of the fileDictionaryList
	this->passes = passes; //set how many passes we need for shredding
	this->toRemove = toRemove; //set state of file removal (true of false)
	try {
		for (int i = 0; i < fileDictionarySize; i++) { //iterate over filePathList and file Dictionary
			File* file = new File(filePathList[i], *this); //we create a new file with each filePath it the filePathList 
			string fileName = file->getFullName(); //we create new key for the dictionary that will represent the file name and extention
			this->fileDictionary[fileName] = file; //we add the file to the dictionary with the key being the file name including its extention
		}
	}
	catch (const runtime_error& e) { //catch a runtime error that might be thrown
		this->signal->sendSignalMessageBox("Error", e.what(), "critical"); //show a messagebox with error in GUI
	}
}


Shredder::~Shredder() {
	for (const auto& [fileName, file] : this->fileDictionary) { //we iterate over fileDictionary 
		delete file; //delete each file that we finished wiping
	}
}


/// <summary>
/// Method to initiate the file threads and start wiping the given files
/// </summary>
void Shredder::initShred() {
	this->numOfThreads = this->fileDictionarySize; //set the numOfThreads value to size of dictionary
	this->threadsRunning = true; //set threadsRunning flag to true
	try {
		for (const auto& [fileName, file] : this->fileDictionary) { //iterate over the fileDictionary
			thread fileThread(&File::secureWipeFile, *file, this->passes, this->toRemove); //we initiate a new thread with wipe method and given parameters
			fileThread.detach(); //detach each thread so they run independently
		}
	}
	catch (const runtime_error& e) { //catch a runtime error that might be thrown
		this->signal->sendSignalMessageBox("Error", e.what(), "critical"); //show a messagebox with error in GUI
	}
}


/// <summary>
/// Method of observer, responsible to notify the GUI when thread is finished it work
/// </summary>
/// <param name="observable"></param>
void Shredder::update(Observable* observable) {
	this->threadMutex.lock(); //lock the mutex to prevent race conditions with threads
	File* file = dynamic_cast<File*>(observable); //casting the Observable object to File object with dynamic_cast
	if (file) { //if cast was successful we emit the signal
		if(!File::getIsCanceled()) //if true we emit success message
			this->signal->sendSignalUpdateListView(file->getFullName(), " - Wiped Successfully"); //emit a signal to GUI to indicate that file has been wiped
		else //else we emit cancel message
			this->signal->sendSignalUpdateListView(file->getFullName(), " - Canceled"); //emit a signal to GUI to indicate that wipe canceled on file
	}
	else //else cast failed 
		this->signal->sendSignalMessageBox("Error", "Thread: Failed to update GUI", "critical"); //we emit signal to show messagebox with error
	this->threadMutex.unlock(); //unlock mutex
}


/// <summary>
/// Method to notify threads that user wants to stop wiping process
/// </summary>
void Shredder::cancelWipe() {
	if (!File::getIsCanceled()) {
		File::setIsCanceled(true); //set the isCanceled flag to true
	}
}