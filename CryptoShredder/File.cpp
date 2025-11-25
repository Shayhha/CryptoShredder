#include "File.h"


bool File::isCanceled = false; //initialization of static isCanceled flag
bool File::isFailed = false; //initialization of static isFailed flag


/**
 * @brief Constructor of class.
 */
File::File(const filesystem::path& filePath) : Observable() {
    this->name = filePath.stem().u16string(); //set the name of file
    this->extention = filePath.extension().u16string(); //set the extention 
    this->fullName = filePath.stem().u16string() + filePath.extension().u16string(); //set the file full name
    this->fullPath = filePath.u16string(); //set full path of file
    this->length = filesystem::file_size(filePath); //set the length with file_size method
}


/**
 * @brief Function that removes a file from computer storage.
 * @param File* file
 * @throws runtime_error if file does not exist or error occurs.
 */
void File::RemoveFile(File* file) {
    try {
		if (!filesystem::remove(filesystem::path(file->fullPath))) //try to remove the file, if true then file does not exist
			throw runtime_error("Thread: Failed deleting file, file does not exist."); //throw runtime error
    }
	catch (const filesystem::filesystem_error& e) { //catch filesystem error that might be thrown
		throw runtime_error(string("Thread: Error deleting file: ") + e.what()); //throw runtime error
    }
}


/**
 * @brief Function that handles wiping the file contents securely using cryptographically secure random bytes.
 * @param File* file
 * @param int passes
 * @param bool toRemove
 */
void File::WipeFile(File* file, int passes, bool toRemove) {
	fstream outputFile; //create fstream object for file operations

    try {
        if (file->length == 0) { //if true the file is empty
            if (toRemove) //if true we need to remove the file
                File::RemoveFile(file); //call removeFile function to remove the file
            file->Notify(true); //notify all observers that we finished the task
            return; //finish the function if file is empty
	    }

        outputFile.open(filesystem::path(file->fullPath), ios::in | ios::out | ios::binary); //open the file in binary mode for reading and writing
        if (!outputFile.is_open() || outputFile.fail()) { //we check if we failed opening the file
            outputFile.close(); //close the file due to error
            File::SetIsFailed(true); //set isFailed to true to indicate of failure
            file->Notify(false); //notify all observers that we finished the task
            return; //finish the function if error occured
        }

        random_device randomDevice; //create random device for generating random bytes
        size_t fileSize = file->length; //set fileSize to be file size in bytes
        size_t currentSize = 0; //set currentSize to be zero to indicate the beginning of file
        size_t chunkSize = 0; //set chunkSize to be zero and later calculate minimal chunk to read
        const size_t maxBufferSize = 1024LL * 1024LL; //set maxBufferSize to be 1MB for efficiency
        size_t bufferSize = min(fileSize, maxBufferSize); //set minimal bufferSize for memory efficiency
        vector<unsigned char> buffer(bufferSize); //create buffer vector based on bufferSize

        //we iterate in a loop each pass and wipe the file's contents
        for (int pass = 0; pass < passes; pass++) {
            outputFile.seekp(0, ios::beg); //we start from the beginning of the file each pass
            currentSize = 0; //reset currentSize in each pass to start from beginning

            //wiping the file with random data with Mersenne Twister algorithm
            while (currentSize < fileSize) {
                chunkSize = min(fileSize - currentSize, bufferSize); //set chunkSize based on the minimum between the fileSize - currentSize and bufferSize

                if (File::isCanceled) { //if true we stop the file wipe
                    if (outputFile.is_open()) //if true the file is open
                        outputFile.close(); //close the file for cancelation
                    file->Notify(true); //notify all observers that we finished the task
                    return; //finish the function if we need to cancel
                }

				//fill the buffer with random bytes using random device
                for (size_t i = 0; i < chunkSize; i++)
                    buffer[i] = (unsigned char)(randomDevice() & 0xFF); //insert random byte to buffer using random device

                outputFile.seekp(currentSize, ios::beg); //set cursor in currentSize position for writing
                outputFile.write(reinterpret_cast<const char*>(buffer.data()), chunkSize); //write buffer data to the file
                if (outputFile.fail()) { //if true we failed to write data
                    if (outputFile.is_open()) //if true the file is open
                        outputFile.close(); //close the file due to error
                    File::SetIsFailed(true); //set isFailed to true to indicate of failure
                    file->Notify(false); //notify all observers that we finished the task
                    return; //finish the function if error occured
                }

                currentSize += chunkSize; //add chunkSize to currentSize for indication to point where we need to write more data in next iteration
            }
        }

		if (outputFile.is_open()) { //if true the file is open
            outputFile.flush(); //flush the file 
            outputFile.close(); //after we finish we close the file
        }

        if (toRemove) //if true we need to remove the file
            File::RemoveFile(file); //call removeFile function to remove the file

        file->Notify(true); //notify all observers that we finished the task
    }
    catch (const exception& e) { //catch exeptions that might be thrown
		if (outputFile.is_open()) //if true the file is open
            outputFile.close(); //close the file due to error
        File::SetIsFailed(true); //set isFailed to true to indicate of failure
        file->Notify(false); //notify all observers that we finished the task
        cout << e.what() << endl; //print the error message
        return; //finish the function if error occured
    }
}


/**
 * @brief Function that handles encryption/decryption on given file using custom AES library.
 * @param File* file
 * @param string key
 * @param bool decrypt
 */
void File::CipherFile(File* file, const string& key, bool decrypt) {
    fstream outputFile; //create fstream object for file operations

    try {
        if (file->length == 0) { //if true the file is empty
            file->Notify(true); //notify all observers that we finished the task
            return; //finish the function if file is empty
	    }

        outputFile.open(filesystem::path(file->fullPath), ios::in | ios::out | ios::binary); //open the file in binary mode for reading and writing
        if (!outputFile.is_open() || outputFile.fail()) { //we check if we failed opening the file
            outputFile.close(); //close the file due to error
            File::SetIsFailed(true); //set isFailed to true to indicate of failure
            file->Notify(false); //notify all observers that we finished the task
            return; //finish the function if error occured
        }

        size_t fileSize = file->length; //set fileSize to be file size in bytes
        size_t currentSize = 0; //set currentSize to be zero to indicate the beginning of file
        size_t chunkSize = 0; //set chunkSize to be zero and later calculate minimal chunk to read
        size_t numOfBlocks = 0; //set number of blocks used for incrementing initialization vector
        const size_t maxBufferSize = 1024LL * 1024LL; //set maxBufferSize to be 1MB for efficiency
        size_t bufferSize = min(fileSize, maxBufferSize); //set minimal bufferSize for memory efficiency
        vector<unsigned char> buffer(bufferSize); //create buffer vector based on bufferSize
        vector<unsigned char> keyVec(key.begin(), key.end()); //create vector for given key
        vector<unsigned char> ivVec(16); //create initialization vector for cipher operation

		//derive the IV vector based on cipher mode
        if (!decrypt) { //if true we encrypt file
            ivVec = AES::Create_IV(16); //create a random IV vector for encryption

			outputFile.seekp(0, ios::end); //set cursor to the end of the file for appending IV
			outputFile.write(reinterpret_cast<const char*>(ivVec.data()), ivVec.size()); //append IV to the end of the file
            if (outputFile.fail()) { //if true we failed to write data
                if (outputFile.is_open()) //if true the file is open
                    outputFile.close(); //close the file due to error
                File::SetIsFailed(true); //set isFailed to true to indicate of failure
                file->Notify(false); //notify all observers that we finished the task
                return; //finish the function if error occured
            }
        }
        else { //else we decrypt file
            if (fileSize < ivVec.size()) { //if true the file is smaller than the IV size
                if (outputFile.is_open()) //if true the file is open
                    outputFile.close(); //close the file due to error
                File::SetIsFailed(true); //set isFailed to true to indicate of failure
                file->Notify(false); //notify all observers that we finished the task
                return; //finish the function if error occured
            }
            outputFile.seekg(fileSize - ivVec.size(), ios::beg); //set cursor to read IV from the end of the file
            outputFile.read(reinterpret_cast<char*>(ivVec.data()), ivVec.size()); //read IV into ivVec
            fileSize -= ivVec.size(); //adjust fileSize to exclude IV
        }

        //transform the IV vecor using AES ECB mode and XOR with the key
        ivVec = AES::Encrypt_ECB(ivVec, keyVec); //we encrypt IV using AES ECB mode with given key
        //apply XOR operation between encrypted ivVec and keyVec
        for (size_t i = 0; i < ivVec.size(); i++)
            ivVec[i] ^= keyVec[i]; //XOR between each byte
		//set right half of ivVec to zero for counter
		fill(ivVec.begin() + 8, ivVec.end(), 0x00); //set right half of ivVec to zero

        //encrypt or decrypt the file using AES algorithm in CTR mode
        while (currentSize < fileSize) {
            chunkSize = min(fileSize - currentSize, bufferSize); //set chunkSize based on the minimum between the fileSize - currentSize and bufferSize
            numOfBlocks = (chunkSize + 15) / 16; //calculate number of blocks used for incrementing IV vector

            if (File::isCanceled) { //if true we stop the file wipe
                if (outputFile.is_open()) //if true the file is open
                    outputFile.close(); //close the file for cancelation
                file->Notify(true); //notify all observers that we finished the task
                return; //finish the function if we need to cancel
            }

            outputFile.seekg(currentSize, ios::beg); //set cursor in currentSize position for reading
            outputFile.read(reinterpret_cast<char*>(buffer.data()), chunkSize); //read chunk into our buffer
            if (outputFile.fail()) { //if true we failed to write data
                if (outputFile.is_open()) //if true the file is open
                    outputFile.close(); //close the file due to error
                File::SetIsFailed(true); //set isFailed to true to indicate of failure
                file->Notify(false); //notify all observers that we finished the task
                return; //finish the function if error occured
            }

            if (!decrypt) //if true we encrypt file
                buffer = AES::Encrypt_CTR(buffer, keyVec, ivVec); //we encrypt using AES CTR mode with given key and IV
            else //else we decrypt file
                buffer = AES::Decrypt_CTR(buffer, keyVec, ivVec); //we decrypt using AES CTR mode with given key and IV

            //increment ivVec for next chunk
            for (size_t i = ivVec.size(); numOfBlocks && i-- > ivVec.size() / 2;) { //iterate over ivVec from end to start
                numOfBlocks += ivVec[i]; //add current byte to numOfBlocks
                ivVec[i] = (unsigned char)(numOfBlocks & 0xFF); //set current byte to be the last byte of numOfBlocks
                numOfBlocks >>= 8; //right shift numOfBlocks by 8 bits
            }

            outputFile.seekp(currentSize, ios::beg); //set cursor in currentSize position for writing
            outputFile.write(reinterpret_cast<const char*>(buffer.data()), chunkSize); //write buffer data to the file
            if (outputFile.fail()) { //if true we failed to write data
                if (outputFile.is_open()) //if true the file is open
                    outputFile.close(); //close the file due to error
                File::SetIsFailed(true); //set isFailed to true to indicate of failure
                file->Notify(false); //notify all observers that we finished the task
                return; //finish the function if error occured
            }

            currentSize += chunkSize; //add chunkSize to currentSize for indication to point where we need to write more data in next iteration
        }

        if (outputFile.is_open()) { //if true the file is open
            outputFile.flush(); //flush the file
            outputFile.close(); //after we finish we close the file
        }

        if (decrypt) //if true we decrypt file and need to resize the file to exclude the IV at the end
            filesystem::resize_file(filesystem::path(file->fullPath), fileSize); //resize the file to exclude the IV at the end

        AES::ClearVector(keyVec); //clear keyVec for added security
        AES::ClearVector(ivVec); //clear ivVec for added security
        file->Notify(true); //notify all observers that we finished the task
    }
    catch (const exception& e) {  //catch exeptions that might be thrown
        if (outputFile.is_open()) //if true the file is open
            outputFile.close(); //close the file due to error
        File::SetIsFailed(true); //set isFailed to true to indicate of failure
        file->Notify(false); //notify all observers that we finished the task
        cout << e.what() << endl; //print the error message
        return; //finish the function if error occured
    }
}