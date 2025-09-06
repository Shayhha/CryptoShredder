#include "File.h"


bool File::isCanceled = false; //initialization of static isCanceled flag
bool File::isFailed = false; //initialization of static isFailed flag


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
 * @brief Function to convert from wstring to regular string.
 * @param wstring wstr
 */
string File::ToString(const wstring& wstr) {
    string result = "";
    for (wchar_t wc : wstr) {
        if (wc <= 0x7F) {
            result.push_back((char)wc); //single-byte characters
        }
        else if (wc <= 0x7FF) {
            result.push_back((char)(0xC0 | (wc >> 6))); //first byte for multi-byte characters
            result.push_back((char)(0x80 | (wc & 0x3F))); //second byte for multi-byte characters
        }
        else if (wc <= 0xFFFF) {
            result.push_back((char)(0xE0 | (wc >> 12))); //first byte for 3-byte characters
            result.push_back((char)(0x80 | ((wc >> 6) & 0x3F))); //second byte
            result.push_back((char)(0x80 | (wc & 0x3F))); //third byte
        }
        else if (wc <= 0x10FFFF) {
            result.push_back((char)(0xF0 | (wc >> 18))); //first byte for 4-byte characters
            result.push_back((char)(0x80 | ((wc >> 12) & 0x3F))); //second byte
            result.push_back((char)(0x80 | ((wc >> 6) & 0x3F))); //third byte
            result.push_back((char)(0x80 | (wc & 0x3F))); //fourth byte
        }
    }
    return result;
}


/**
 * @brief Function to convert from string to wstring.
 * @param string str
 */
wstring File::ToWString(const string& str) {
    wstring result = L"";
    size_t i = 0;
    while (i < str.size()) {
        unsigned char c1 = str[i];
        if (c1 <= 0x7F) {
            result.push_back((wchar_t)c1); //single-byte characters
            ++i;
        }
        else if ((c1 >> 5) == 0x6) {
            unsigned char c2 = str[i + 1];
            wchar_t wc = ((c1 & 0x1F) << 6) | (c2 & 0x3F); //2-byte character
            result.push_back(wc);
            i += 2;
        }
        else if ((c1 >> 4) == 0xE) {
            unsigned char c2 = str[i + 1];
            unsigned char c3 = str[i + 2];
            wchar_t wc = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F); //3-byte character
            result.push_back(wc);
            i += 3;
        }
        else if ((c1 >> 3) == 0x1E) {
            unsigned char c2 = str[i + 1];
            unsigned char c3 = str[i + 2];
            unsigned char c4 = str[i + 3];
            wchar_t wc = ((c1 & 0x7) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F); //4-byte character
            result.push_back(wc);
            i += 4;
        }
    }
    return result;
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
        outputFile.close(); //close the file due to error
        File::setIsFailed(true); //set isFailed to true to indicate of failure
        file.notify(false); //notify all observers that we finished the task
        return; //finish the function if error occured
    }

    try {
        random_device randomDevice; //for random bytes generator
        mt19937_64 generator(randomDevice()); //secure random byte generator

        size_t fileSize = file.length; //set fileSize to be file size in bytes
        size_t currentSize = 0; //set currentSize to be zero to indicate the beginning of file
        size_t chunkSize = 0; //set chunkSize to be zero and later calculate minimal chunk to read
        const size_t maxBufferSize = 1024 * 1024; //set maxBufferSize to be 1MB for efficiency
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
                    file.notify(true); //notify all observers that we finished the task
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
                    file.notify(false); //notify all observers that we finished the task
                    return; //finish the function if error occured
                }

                outputFile.flush(); //flush the file 
                currentSize += chunkSize; //add chunkSize to currentSize for indication to point where we need to write more data in next iteration
            }
        }

        outputFile.close(); //after we finish we close the file
        if (toRemove) //if true we need to remove the file
            removeFile(file); //call removeFile function to remove the file
        file.notify(true); //notify all observers that we finished the task
    }
    catch (const exception& e) { //catch exceptions that may be thrown
        outputFile.close(); //close the file due to error
        File::setIsFailed(true); //set isFailed to true to indicate of failure
        file.notify(false); //notify all observers that we finished the task
        return; //finish the function if error occured
    }
}


/**
 * @brief Function that handles encryption/decryption on given file using custom AES library.
 * @param File file
 * @param string key
 * @param bool decrypt
 */
void File::CipherFile(const File& file, const string& key, bool decrypt) {
    fstream outputFile(file.fullPath, ios::in | ios::out | ios::binary); //open the file in binary mode for reading and writing

    if (!outputFile.is_open() || outputFile.fail()) { //we check if we failed opening the file
        outputFile.close(); //close the file due to error
        File::setIsFailed(true); //set isFailed to true to indicate of failure
        file.notify(false); //notify all observers that we finished the task
        return; //finish the function if error occured
    }

    try {
        size_t fileSize = file.length; //set fileSize to be file size in bytes
        size_t currentSize = 0; //set currentSize to be zero to indicate the beginning of file
        size_t chunkSize = 0; //set chunkSize to be zero and later calculate minimal chunk to read
        const size_t maxBufferSize = 1024 * 1024; //set maxBufferSize to be 1MB for efficiency
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
                file.notify(true); //notify all observers that we finished the task
                return; //finish the function if we need to cancel
            }

            outputFile.seekg(currentSize); //set cursor in currentSize position for reading
            outputFile.read(buffer.data(), chunkSize); //read chunk into our buffer
            if (outputFile.fail()) { //if true we failed to write data
                outputFile.close(); //close the file due to error
                File::setIsFailed(true); //set isFailed to true to indicate of failure
                file.notify(false); //notify all observers that we finished the task
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
                file.notify(false); //notify all observers that we finished the task
                return; //finish the function if error occured
            }

            outputFile.flush(); //flush the file 
            currentSize += chunkSize; //add chunkSize to currentSize for indication to point where we need to write more data in next iteration
        }

        outputFile.close(); //after we finish we close the file
        file.notify(true); //notify all observers that we finished the task
    }
    catch (const exception& e) { //catch exceptions that may be thrown
        outputFile.close(); //close the file due to error
        File::setIsFailed(true); //set isFailed to true to indicate of failure
        file.notify(false); //notify all observers that we finished the task
        return; //finish the function if error occured
    }
}