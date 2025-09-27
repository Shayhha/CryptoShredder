#include "File.h"


bool File::isCanceled = false; //initialization of static flag 


/**
 * @brief Constructor of class.
 */
File::File(const string& filePath, Observer& observer) : Observable() {
    filesystem::path p(File::ToWString(filePath)); //create path object to get info from filePath
    if (!exists(p)) { //check if the path exists
        throw runtime_error("Error, the path: " + filePath + " does not exist."); //if not we throw runtime error
    }
    this->name = p.stem().wstring(); //set the name of file
    this->extention = p.extension().wstring(); //set the extention 
    this->fullName = p.stem().wstring() + p.extension().wstring(); //set the file full name
    this->fullPath = p.wstring(); //set full path of file
    this->length = filesystem::file_size(p); //set the length with file_size method
    this->addObserver(&observer); //add the observer to observer list
}


/**
 * @brief Function to remove a file from computer storage.
 * @param File file
 */
void File::removeFile(const File& file) {
    if (_wremove(file.fullPath.c_str()) != 0) { //if true we failed removing file
        throw runtime_error("Error trying to delete file: " + File::ToString(file.fullPath)); //throw exception with error
    }
}


/**
 * @brief Function that handles wiping the file contents securely using crypto methods.
 * @param File file
 * @param int passes
 * @param bool toRemove
 */
void File::WipeFile(const File& file, int passes, bool toRemove) {
    fstream outputFile(file.fullPath, ios::in | ios::out | ios::binary); //open the file in binary mode for reading and writing

    if (!outputFile.is_open() || outputFile.fail()) { //we check if we failed opening the file
        throw runtime_error("Error opening file: " + File::ToString(file.fullName)); //throw runtime error exception if failed to open file
        return; //stop the method
    }
    try {
        random_device randomDevice; //for random bytes generator
        mt19937_64 generator(randomDevice()); //secure random byte generator 

        //we iterate in a loop each pass and wipe the file's contents
        for (int pass = 0; pass < passes; pass++) {
            outputFile.seekp(0); //we start from the beginning of the file each pass
            size_t fileSize = file.length; //set the fileSize to be file size in bytes 
            size_t currentSize = 0; //set currentSize to be zero to indicate the beginning of file
            const size_t bufferSize = fileSize / 4; //set a bufferSize to be a quarter of original file size for memory efficiency
            vector<char> buffer(bufferSize); //create a vector that uses the bufferSize

            //wiping the file with random data with Mersenne Twister algorithm
            while (currentSize != file.length) { //we write until we reach the original size of file
                const size_t chunkSize = min(fileSize, bufferSize); //set chunkSize based on the minimum between the fileSize and bufferSize
                outputFile.seekp(currentSize); //set the pointer for replacing bytes with the currentSize parameter to write in chunks
                //generate random data
                for (size_t i = 0; i < chunkSize; i++) {
                    if (File::isCanceled) { //if true we stop the file wipe
                        outputFile.close(); //after we finish we close the file
                        file.notify(); //notify all observers that we finished the task
                        return; //finish the function if we need to cancel
                    }
                    buffer[i] = (unsigned char)(generator() & 0xFF); //generate a random byte and insert it into the buffer
                }
                outputFile.write(buffer.data(), chunkSize); //write buffer data to the file
                outputFile.flush(); //flush the file 
                currentSize += chunkSize; //add chunkSize to currentSize for indication and for the point where we need to write more data in next iteration
                fileSize -= chunkSize; //subtract chunkSize we wrote to file from the total fileSize
            }
        }
    }
    catch (const exception& e) { //catch exceptions that may be thrown
        outputFile.close(); //after we finish, we close the file
        file.notify(); //notify all observers that we finished the task
        throw runtime_error(e.what()); //throw runtime error
    }

    outputFile.close(); //after we finish we close the file
    if (toRemove) //if true we need to remove the file
        removeFile(file); //call removeFile function to remove the file
    file.notify(); //notify all observers that we finished the task
}


/**
 * @brief Function that handles encryption/decryption on given file using custom AES library.
 * @param File file
 * @param string key
 * @param bool decrypt
 */
void File::CipherFile(const File& file, const string& key, bool decrypt) {
    fstream outputFile(file.fullPath, ios::in | ios::out | ios::binary); //open the file in binary mode for reading and writing

    if (!outputFile.is_open() || outputFile.fail()) { //check if we failed opening the file
        throw runtime_error("Error opening file: " + File::ToString(file.fullName)); //throw a runtime error exception if failed to open the file
        return; //stop the method
    }

    try {
        outputFile.seekp(0); //start from the beginning of the file each pass
        size_t fileSize = file.length; //set the fileSize to be the file size in bytes 
        size_t currentSize = 0; //set currentSize to be zero to indicate the beginning of the file
        const size_t bufferSize = fileSize / 4; //set a bufferSize to be a quarter of the original file size for memory efficiency
        vector<char> buffer(bufferSize); //create a vector that uses the bufferSize

        const vector<unsigned char> keyVec(key.begin(), key.end()); //save the given key in a vector
        vector<unsigned char> ivVec(keyVec.begin(), keyVec.begin() + 16); //create a initialization vector with first 16 bytes of given keyVec
        ivVec = AES::Encrypt_ECB(ivVec, keyVec); //we encrypt the initialization vector using AES ECB mode with given key
        //apply XOR operation between encrypted ivVec and keyVec
        for (size_t i = 0; i < ivVec.size(); i++)
            ivVec[i] ^= keyVec[i]; //XOR between each byte

        while (currentSize != file.length) { //we write until we reach the original size of the file
            const size_t chunkSize = min(fileSize, bufferSize); //set chunkSize based on the minimum between the fileSize and bufferSize

            //read asynchronously
            future<void> readFuture = async(launch::async, [&]() {
                outputFile.seekg(currentSize); //set the pointer with the currentSize parameter to read in chunks
                vector<unsigned char> buffer(chunkSize); //set the vector for operation
                outputFile.read(reinterpret_cast<char*>(buffer.data()), chunkSize); //read file to buffer

                if (!decrypt) //if true we encrypt file
                    buffer = AES::Encrypt_CTR(buffer, keyVec, ivVec); //we encrypt using AES CTR mode with given key and iv 
                else //else we decrypt file
                    buffer = AES::Decrypt_CTR(buffer, keyVec, ivVec); //we decrypt using AES CTR mode with given key and iv

                if (File::isCanceled) { //if true we stop the file encryption/decryption
                    outputFile.close(); //after we finish we close the file
                    file.notify(); //notify all observers that we finished the task
                    return; //finish the function if we need to cancel
                }

                //write asynchronously
                future<void> writeFuture = async(launch::async, [&]() {
                    outputFile.seekp(currentSize); //set the pointer with the currentSize parameter to write in chunks
                    outputFile.write(reinterpret_cast<char*>(buffer.data()), chunkSize); //write buffer data to the file
                    outputFile.flush(); //flush the file 
                    });

                writeFuture.get(); //wait for the write to finish
                });

            readFuture.get(); //wait for the read and encryption/decryption to finish

            currentSize += chunkSize; //add chunkSize to currentSize for indication and for the point where we need to write more data in next iteration
            fileSize -= chunkSize; //subtract chunkSize we wrote to file from the total fileSize
        }
    }
    catch (const exception& e) { //catch exceptions that may be thrown
        outputFile.close(); //after we finish, we close the file
        file.notify(); //notify all observers that we finished the task
        throw runtime_error(e.what()); //throw runtime error
    }

    outputFile.close(); //after we finish, we close the file
    file.notify(); //notify all observers that we finished the task
}