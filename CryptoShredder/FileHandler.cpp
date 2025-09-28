#include "FileHandler.h"


/**
 * @brief Constructor of class.
 */
FileHandler::FileHandler(vector<File*>& fileList, SignalProxy* signal) : fileList(fileList), signal(signal), Observer() {
	try {
		for (File* file : this->fileList) //iterate over fileList
			file->AddObserver(this); //add this as observer to each file object	
	}
	catch (const exception& e) { //catch exeptions that might be thrown
		this->signal->SendSignalMessageBox("Error", "Failed initializing file handler.", "Critical"); //show a messagebox with error in GUI
		cout << string("Failed initializing file handler: ") + e.what() << endl; //print the error message
	}
}


/**
 * @brief Destructor of class.
 */
FileHandler::~FileHandler() {
	for (File* file : this->fileList) { //we iterate over fileList
		file->RemoveObserver(this); //remove observer from each file object
		delete file; //delete each file that we finished processing
	}
	this->fileList.clear(); //clear the fileList vector
}


/**
 * @brief Method to initiate the file threads and start wipe process on given files.
 * @param int passes
 * @param bool toRemove
 */
void FileHandler::InitWipe(int passes, bool toRemove) {
	this->wipe = true; //set the wipe flag to true
	this->numOfThreads = this->fileList.size(); //set the numOfThreads value to size of fileList
	this->threadsRunning = true; //set threadsRunning flag to true
	try {
		for (File* file : this->fileList) { //we iterate over fileList
			thread fileThread(&File::WipeFile, file, passes, toRemove); //we initiate a new thread with wipe method and given parameters
			fileThread.detach(); //detach each thread so they run independently
		}
	}
	catch (const exception& e) { //catch exeptions that might be thrown
		this->signal->SendSignalMessageBox("Error", "Failed initializing wipe operation.", "Critical"); //show a messagebox with error in GUI
		cout << string("Failed initializing wipe operation: ") + e.what() << endl; //print the error message
	}
}


/**
 * @brief Method to initiate the file threads and start cipher process on given files.
 * @param string key
 * @param bool decrypt
 */
void FileHandler::InitCipher(const string& key, bool decrypt) {
	this->decrypt = decrypt; //set decrypt flag 
	this->numOfThreads = this->fileList.size(); //set the numOfThreads value to size of fileList
	this->threadsRunning = true; //set threadsRunning flag to true
	try {
		for (File* file : this->fileList) { //we iterate over fileList
			thread fileThread(&File::CipherFile, file, key, this->decrypt); //we initiate a new thread with cipher method and given parameters
			fileThread.detach(); //detach each thread so they run independently
		}
	}
	catch (const exception& e) { //catch exeptions that might be thrown
		this->signal->SendSignalMessageBox("Error", "Failed initializing cipher operation.", "Critical"); //show a messagebox with error in GUI
		cout << string("Failed initializing cipher operation: ") + e.what() << endl; //print the error message
	}
}


/**
 * @brief Method of observer, responsible to notify the GUI when thread is finished it work.
 * @param Observable* observable
 * @param bool state
 */
void FileHandler::Update(Observable* observable, bool state) {
	this->threadMutex.lock(); //lock the mutex to prevent race conditions with threads
	try {
		File* file = dynamic_cast<File*>(observable); //casting the Observable object to File object with dynamic_cast
		if (file) { //if cast was successful we emit the signal
			u16string fileFullName = file->GetFullName(); //get file full name from file object
			u16string fileName = (fileFullName.size() > 34) ? file->GetName().substr(0, 30) + u"..." + file->GetExtention() : fileFullName; //get adjusted file name based on length for GUI 
			if (state) { //if operation was successful
				if (!File::GetIsCanceled()) { //if true we emit success message
					if (this->wipe) //if wipe flag is true we emit a signal that wipe has finished
						this->signal->SendSignalUpdateListView(fileFullName, fileName, " - Wiped Successfully"); //emit a signal to GUI to indicate that file has been wiped
					else { //else we're encrypting/decrypting
						if (!this->decrypt) //if true we emit a signal that encryption has finished
							this->signal->SendSignalUpdateListView(fileFullName, fileName, " - Encrypted Successfully"); //emit a signal to GUI to indicate that file has been encrypted
						else //else we emit a signal that decryption has finished
							this->signal->SendSignalUpdateListView(fileFullName, fileName, " - Decrypted Successfully"); //emit a signal to GUI to indicate that file has been decrypted
					}
				}
				else //else we emit cancel message
					this->signal->SendSignalUpdateListView(fileFullName, fileName, " - Canceled"); //emit a signal to GUI to indicate that wipe canceled on file
			}
			else //else operation failed
				this->signal->SendSignalUpdateListView(fileFullName, fileName, " - Failed"); //emit a signal to GUI to indicate that we failed perofrming operation
		}
		else { //else cast failed 
			this->signal->SendSignalMessageBox("Error", "Thread: Failed to update GUI.", "Critical"); //we emit signal to show messagebox with error
			cout << "Thread: Failed to update GUI." << endl; //print the error message
		}
		this->threadMutex.unlock(); //unlock mutex
	}
	catch (const exception& e) { //catch exeptions that might be thrown
		this->signal->SendSignalMessageBox("Error", "Thread: Failed to update GUI.", "Critical"); //show a messagebox with error in GUI
		cout << string("Thread: Failed to update GUI: ") + e.what() << endl; //print the error message
		this->threadMutex.unlock(); //unlock mutex
	}
}


/**
 * @brief Method to notify threads that user wants to cancel process.
 */
void FileHandler::CancelProcess() {
	if (!File::GetIsCanceled())
		File::SetIsCanceled(true); //set the isCanceled flag to true
}