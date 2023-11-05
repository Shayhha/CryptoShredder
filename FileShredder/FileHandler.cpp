#include "FileHandler.h"


FileHandler::FileHandler(const vector<string> filePathList, SignalProxy* signal) : Observer() {
	this->fileDictionarySize = filePathList.size();; //set the size of the fileDictionaryList
	this->signal = signal; //set the signal object for GUI update
	try {
		for (int i = 0; i < this->fileDictionarySize; i++) { //iterate over filePathList and file Dictionary
			File* file = new File(filePathList[i], *this); //we create a new file with each filePath it the filePathList 
			string fileName = file->getFullName(); //we create new key for the dictionary that will represent the file name and extention
			this->fileDictionary[fileName] = file; //we add the file to the dictionary with the key being the file name including its extention
		}
	}
	catch (const runtime_error& e) { //catch a runtime error that might be thrown
		this->signal->sendSignalMessageBox("Error", e.what(), "critical"); //show a messagebox with error in GUI
	}
}


FileHandler::~FileHandler() {
	for (const auto& [fileName, file] : this->fileDictionary) { //we iterate over fileDictionary 
		delete file; //delete each file that we finished wiping
	}
}


/// <summary>
/// Method to initiate the file threads and start wipe process on given files
/// </summary>
void FileHandler::initWipe(int passes, bool toRemove) {
	this->wipe = true; //set the wipe flag to true
	this->numOfThreads = this->fileDictionarySize; //set the numOfThreads value to size of dictionary
	this->threadsRunning = true; //set threadsRunning flag to true
	for (const auto& [fileName, file] : this->fileDictionary) { //iterate over the fileDictionary
		thread fileThread([&] { //we initiate a new thread with WipeFile function and given parameters
			try {
				File::WipeFile(*file, passes, toRemove); //call our WipeFile function
			}
			catch (const runtime_error& e) { //catch a runtime error that might be thrown
				this->signal->sendSignalMessageBox("Error", e.what(), "critical"); //emit a signal to show a messagebox with error in GUI
			}
		});
		fileThread.detach(); //detach each thread so they run independently
	}
}


/// <summary>
/// Method to initiate the file threads and start cipher process on given files
/// </summary>
void FileHandler::initCipher(const string& key, bool decrypt) {
	this->decrypt = decrypt; //set decrypt flag 
	this->numOfThreads = this->fileDictionarySize; //set the numOfThreads value to size of dictionary
	this->threadsRunning = true; //set threadsRunning flag to true
	for (const auto& [fileName, file] : this->fileDictionary) { //iterate over the fileDictionary
		thread fileThread([&] { //we initiate a new thread with CipherFile function and given parameters
			try {
				File::CipherFile(*file, key, this->decrypt); //call our CipherFile function
			}
			catch (const runtime_error& e) { //catch a runtime error that might be thrown
				this->signal->sendSignalMessageBox("Error", e.what(), "critical"); //emit a signal to show a messagebox with error in GUI
			}
		});
		fileThread.detach(); //detach each thread so they run independently
	}
}


/// <summary>
/// Method of observer, responsible to notify the GUI when thread is finished it work
/// </summary>
/// <param name="observable"></param>
void FileHandler::update(Observable* observable) {
	this->threadMutex.lock(); //lock the mutex to prevent race conditions with threads
	File* file = dynamic_cast<File*>(observable); //casting the Observable object to File object with dynamic_cast
	if (file) { //if cast was successful we emit the signal
		if (!File::getIsCanceled()) { //if true we emit success message
			if(this->wipe) //if wipe flag is true we emit a signal that wipe has finished
				this->signal->sendSignalUpdateListView(file->getFullName(), " - Wiped Successfully"); //emit a signal to GUI to indicate that file has been wiped
			else { //else we're encrypting/decrypting
				if(!this->decrypt) //if true we emit a signal that encryption has finished
					this->signal->sendSignalUpdateListView(file->getFullName(), " - Encrypted Successfully"); //emit a signal to GUI to indicate that file has been encrypted
				else //else we emit a signal that decryption has finished
					this->signal->sendSignalUpdateListView(file->getFullName(), " - Decrypted Successfully"); //emit a signal to GUI to indicate that file has been decrypted
			}
		}
		else //else we emit cancel message
			this->signal->sendSignalUpdateListView(file->getFullName(), " - Canceled"); //emit a signal to GUI to indicate that wipe canceled on file
	}
	else //else cast failed 
		this->signal->sendSignalMessageBox("Error", "Thread: Failed to update GUI", "critical"); //we emit signal to show messagebox with error
	this->threadMutex.unlock(); //unlock mutex
}


/// <summary>
/// Method to notify threads that user wants to stop process
/// </summary>
void FileHandler::cancelProcess() {
	if (!File::getIsCanceled())
		File::setIsCanceled(true); //set the isCanceled flag to true
}