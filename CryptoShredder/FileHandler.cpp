#include "FileHandler.h"


/**
 * @brief Constructor of class.
 */
FileHandler::FileHandler(vector<File*>& fileList, SignalProxy* signal) : fileList(fileList), signal(signal), Observer() {
	try {
		for (File* file : this->fileList) //iterate over fileList
			file->addObserver(this); //add this as observer to each file object	
	}
	catch (const exception& e) { //catch a runtime error that might be thrown
		this->signal->sendSignalMessageBox("Error", e.what(), "Critical"); //show a messagebox with error in GUI
		cout << e.what() << endl; //print the error message
	}
}


/**
 * @brief Destructor of class.
 */
FileHandler::~FileHandler() {
	for (File* file : this->fileList) { //we iterate over fileList
		file->removeObserver(this); //remove observer from each file object
		delete file; //delete each file that we finished processing
	}
	this->fileList.clear(); //clear the fileList vector
}


/**
 * @brief Method to initiate the file threads and start wipe process on given files.
 * @param int passes
 * @param bool toRemove
 */
void FileHandler::initWipe(int passes, bool toRemove) {
	this->wipe = true; //set the wipe flag to true
	this->numOfThreads = this->fileList.size(); //set the numOfThreads value to size of fileList
	this->threadsRunning = true; //set threadsRunning flag to true
	try {
		for (File* file : this->fileList) { //we iterate over fileList
			thread fileThread(&File::WipeFile, file, passes, toRemove); //we initiate a new thread with wipe method and given parameters
			fileThread.detach(); //detach each thread so they run independently
		}
	}
	catch (const exception& e) { //catch a runtime error that might be thrown
		this->signal->sendSignalMessageBox("Error", e.what(), "Critical"); //show a messagebox with error in GUI
		cout << e.what() << endl; //print the error message
	}
}


/**
 * @brief Method to initiate the file threads and start cipher process on given files.
 * @param string key
 * @param bool decrypt
 */
void FileHandler::initCipher(const string& key, bool decrypt) {
	this->decrypt = decrypt; //set decrypt flag 
	this->numOfThreads = this->fileList.size(); //set the numOfThreads value to size of fileList
	this->threadsRunning = true; //set threadsRunning flag to true
	try {
		for (File* file : this->fileList) { //we iterate over fileList
			thread fileThread(&File::CipherFile, file, key, this->decrypt); //we initiate a new thread with cipher method and given parameters
			fileThread.detach(); //detach each thread so they run independently
		}
	}
	catch (const exception& e) { //catch a runtime error that might be thrown
		this->signal->sendSignalMessageBox("Error", e.what(), "Critical"); //show a messagebox with error in GUI
		cout << e.what() << endl; //print the error message
	}
}


/**
 * @brief Method of observer, responsible to notify the GUI when thread is finished it work.
 * @param Observable* observable
 * @param bool state
 */
void FileHandler::update(Observable* observable, bool state) {
	this->threadMutex.lock(); //lock the mutex to prevent race conditions with threads
	File* file = dynamic_cast<File*>(observable); //casting the Observable object to File object with dynamic_cast
	if (file) { //if cast was successful we emit the signal
		u16string fileFullName = file->getFullName(); //get file full name from file object
		u16string fileName = (fileFullName.size() > 34) ? file->getName().substr(0, 30) + u"..." + file->getExtention() : fileFullName; //get adjusted file name based on length for GUI 
		if (state) { //if operation was successful
			if (!File::getIsCanceled()) { //if true we emit success message
				if (this->wipe) //if wipe flag is true we emit a signal that wipe has finished
					this->signal->sendSignalUpdateListView(fileFullName, fileName, " - Wiped Successfully"); //emit a signal to GUI to indicate that file has been wiped
				else { //else we're encrypting/decrypting
					if (!this->decrypt) //if true we emit a signal that encryption has finished
						this->signal->sendSignalUpdateListView(fileFullName, fileName, " - Encrypted Successfully"); //emit a signal to GUI to indicate that file has been encrypted
					else //else we emit a signal that decryption has finished
						this->signal->sendSignalUpdateListView(fileFullName, fileName, " - Decrypted Successfully"); //emit a signal to GUI to indicate that file has been decrypted
				}
			}
			else //else we emit cancel message
				this->signal->sendSignalUpdateListView(fileFullName, fileName, " - Canceled"); //emit a signal to GUI to indicate that wipe canceled on file
		}
		else //else operation failed
			this->signal->sendSignalUpdateListView(fileFullName, fileName, " - Failed"); //emit a signal to GUI to indicate that we failed perofrming operation
	}
	else { //else cast failed 
		this->signal->sendSignalMessageBox("Error", "Thread: Failed to update GUI.", "Critical"); //we emit signal to show messagebox with error
		cout << "Thread: Failed to update GUI." << endl; //print the error message
	}
	this->threadMutex.unlock(); //unlock mutex
}


/**
 * @brief Method to notify threads that user wants to cancel process.
 */
void FileHandler::cancelProcess() {
	if (!File::getIsCanceled())
		File::setIsCanceled(true); //set the isCanceled flag to true
}