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
void File::removeFile(File* file) {
    try {
		if (!filesystem::remove(file->fullPath)) //try to remove the file, if true then file does not exist
			throw runtime_error("Thread: Failed deleting file, file does not exist."); //throw runtime error
    }
	catch (const filesystem::filesystem_error& e) { //catch filesystem error that may be thrown
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
    fstream outputFile(filesystem::path(file->fullPath), ios::in | ios::out | ios::binary); //open the file in binary mode for reading and writing

    if (!outputFile.is_open() || outputFile.fail()) { //we check if we failed opening the file
        outputFile.close(); //close the file due to error
        File::setIsFailed(true); //set isFailed to true to indicate of failure
        file->notify(false); //notify all observers that we finished the task
        return; //finish the function if error occured
    }

    try {
        random_device randomDevice; //for random bytes generator
        mt19937_64 generator(randomDevice()); //secure random byte generator

        size_t fileSize = file->length; //set fileSize to be file size in bytes
        size_t currentSize = 0; //set currentSize to be zero to indicate the beginning of file
        size_t chunkSize = 0; //set chunkSize to be zero and later calculate minimal chunk to read
        const size_t maxBufferSize = 1024LL * 1024LL; //set maxBufferSize to be 1MB for efficiency
        size_t bufferSize = min(fileSize, maxBufferSize); //set minimal bufferSize for memory efficiency
        vector<char> buffer(bufferSize); //create buffer vector based on bufferSize

        //we iterate in a loop each pass and wipe the file's contents
        for (int pass = 0; pass < passes; pass++) {
            outputFile.seekp(0); //we start from the beginning of the file each pass
            currentSize = 0; //reset currentSize in each pass to start from beginning

            //wiping the file with random data with Mersenne Twister algorithm
            while (currentSize < fileSize) {
                chunkSize = min(fileSize - currentSize, bufferSize); //set chunkSize based on the minimum between the fileSize - currentSize and bufferSize

                if (File::isCanceled) { //if true we stop the file wipe
                    outputFile.close(); //close the file for cancelation
                    file->notify(true); //notify all observers that we finished the task
                    return; //finish the function if we need to cancel
                }

                //generate random bytes for our buffer vecctor
                for (size_t i = 0; i < chunkSize; i++)
                    buffer[i] = (unsigned char)(generator() & 0xFF); //generate a random byte and insert it into the buffer

                outputFile.seekp(currentSize); //set cursor in currentSize position for writing
                outputFile.write(buffer.data(), chunkSize); //write buffer data to the file
                if (outputFile.fail()) { //if true we failed to write data
                    outputFile.close(); //close the file due to error
                    File::setIsFailed(true); //set isFailed to true to indicate of failure
                    file->notify(false); //notify all observers that we finished the task
                    return; //finish the function if error occured
                }

                outputFile.flush(); //flush the file 
                currentSize += chunkSize; //add chunkSize to currentSize for indication to point where we need to write more data in next iteration
            }
        }

        outputFile.close(); //after we finish we close the file
        if (toRemove) //if true we need to remove the file
            removeFile(file); //call removeFile function to remove the file
        file->notify(true); //notify all observers that we finished the task
    }
    catch (const exception& e) { //catch exceptions that may be thrown
        outputFile.close(); //close the file due to error
        File::setIsFailed(true); //set isFailed to true to indicate of failure
        file->notify(false); //notify all observers that we finished the task
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
    fstream outputFile(filesystem::path(file->fullPath), ios::in | ios::out | ios::binary); //open the file in binary mode for reading and writing

    if (!outputFile.is_open() || outputFile.fail()) { //we check if we failed opening the file
        outputFile.close(); //close the file due to error
        File::setIsFailed(true); //set isFailed to true to indicate of failure
        file->notify(false); //notify all observers that we finished the task
        return; //finish the function if error occured
    }

    try {
        size_t fileSize = file->length; //set fileSize to be file size in bytes
        size_t currentSize = 0; //set currentSize to be zero to indicate the beginning of file
        size_t chunkSize = 0; //set chunkSize to be zero and later calculate minimal chunk to read
        const size_t maxBufferSize = 1024LL * 1024LL; //set maxBufferSize to be 1MB for efficiency
        size_t bufferSize = min(fileSize, maxBufferSize); //set minimal bufferSize for memory efficiency
        vector<char> buffer(bufferSize); //create buffer vector based on bufferSize

        const vector<unsigned char> keyVec(key.begin(), key.end()); //save the given key in vector
        vector<unsigned char> ivVec(keyVec.begin(), keyVec.begin() + 16); //create a initialization vector with first 16 bytes of given keyVec
        ivVec = AES::Encrypt_ECB(ivVec, keyVec); //we encrypt the initialization vector using AES ECB mode with given key
        //apply XOR operation between encrypted ivVec and keyVec
        for (size_t i = 0; i < ivVec.size(); i++)
            ivVec[i] ^= keyVec[i]; //XOR between each byte

        //encrypt or decrypt the file using AES algorithm in CTR mode
        while (currentSize < fileSize) {
            chunkSize = min(fileSize - currentSize, bufferSize); //set chunkSize based on the minimum between the fileSize - currentSize and bufferSize

            if (File::isCanceled) { //if true we stop the file wipe
                outputFile.close(); //close the file for cancelation
                file->notify(true); //notify all observers that we finished the task
                return; //finish the function if we need to cancel
            }

            outputFile.seekg(currentSize); //set cursor in currentSize position for reading
            outputFile.read(buffer.data(), chunkSize); //read chunk into our buffer
            if (outputFile.fail()) { //if true we failed to write data
                outputFile.close(); //close the file due to error
                File::setIsFailed(true); //set isFailed to true to indicate of failure
                file->notify(false); //notify all observers that we finished the task
                return; //finish the function if error occured
            }

            vector<unsigned char> processedBuffer(buffer.begin(), buffer.begin() + chunkSize); //create processedBuffer with chunkSize for encryption/decryption
            if (!decrypt) //if true we encrypt file
                processedBuffer = AES::Encrypt_CTR(processedBuffer, keyVec, ivVec); //we encrypt using AES CTR mode with given key and iv 
            else //else we decrypt file
                processedBuffer = AES::Decrypt_CTR(processedBuffer, keyVec, ivVec); //we decrypt using AES CTR mode with given key and iv 

            outputFile.seekp(currentSize); //set cursor in currentSize position for writing
            outputFile.write(reinterpret_cast<const char*>(processedBuffer.data()), chunkSize); //write buffer data to the file
            if (outputFile.fail()) { //if true we failed to write data
                outputFile.close(); //close the file due to error
                File::setIsFailed(true); //set isFailed to true to indicate of failure
                file->notify(false); //notify all observers that we finished the task
                return; //finish the function if error occured
            }

            outputFile.flush(); //flush the file 
            currentSize += chunkSize; //add chunkSize to currentSize for indication to point where we need to write more data in next iteration
        }

        outputFile.close(); //after we finish we close the file
        file->notify(true); //notify all observers that we finished the task
    }
    catch (const exception& e) { //catch exceptions that may be thrown
        outputFile.close(); //close the file due to error
        File::setIsFailed(true); //set isFailed to true to indicate of failure
        file->notify(false); //notify all observers that we finished the task
		cout << e.what() << endl; //print the error message
        return; //finish the function if error occured
    }
}