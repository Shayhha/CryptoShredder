#include "CryptoShredder.h"


QLocalServer* CryptoShredder::server = NULL; //set server to null
QString CryptoShredder::serverName = "CryptoShredder"; //set server name
bool CryptoShredder::isWipe = true; //set wipe flag to true by default
bool CryptoShredder::isClosing = false; //set isClosing flag to false by default


/**
 * @brief Constructor of GUI.
 * @param QWidget* parent
 */
CryptoShredder::CryptoShredder(QWidget* parent) : QMainWindow(parent) {
    //setup ui elements
    this->ui.setupUi(this);

    //setup GUI elements at start of program//
    this->setWindowIcon(QIcon("images/CryptoShredder.ico")); //set window icon
    this->ui.cipherFrame->setVisible(false); //hide cipherFrame and contents
    this->ui.wipeFrame->setVisible(true); //show wipeFrame and contents
    this->listViewModel = new QStringListModel(); //initialize the model for listView in GUI
    this->keyValidator = new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9\\W]{16}|[a-zA-Z0-9\\W]{24}|[a-zA-Z0-9\\W]{32}")); //regular expression for key 
    this->signal = new SignalProxy(); //initialize the signal proxy for the foreigner classes
    this->ui.KeyLineEdit->setValidator(this->keyValidator); //set the key validator
    this->ui.FileListView->setModel(listViewModel); //set the list model for listView in GUI to add elements
    this->ui.FileListView->setEditTriggers(QAbstractItemView::NoEditTriggers); //set the listView to be uneditable
    this->ui.PassesSpinBox->findChild<QLineEdit*>()->setReadOnly(true); //remove the lineEdit in spinBox

    //slots for ui elements//
    connect(this->ui.ProcessButton, &QPushButton::clicked, this, &CryptoShredder::processFiles);
    connect(this->ui.CancelButton, &QPushButton::clicked, this, &CryptoShredder::cancelProcess);
    connect(this->ui.ChooseFilesButton, &QPushButton::clicked, this, &CryptoShredder::openFileDialog);
    connect(this->ui.ClearScreenButton, &QPushButton::clicked, this, &CryptoShredder::clearContents);
    connect(this->ui.OptionsButton, &QPushButton::clicked, this, &CryptoShredder::changeOperationMode);
    connect(this->ui.InfoButton, &QPushButton::clicked, this, &CryptoShredder::showInfoWindow);
    connect(this->ui.FileListView, &QAbstractItemView::doubleClicked, this, &CryptoShredder::doubleClickedFile);
    connect(this->ui.CipherCheckBox, &QCheckBox::checkStateChanged, this, &CryptoShredder::changeCipherMode);
    connect(this->ui.KeyLineEdit, &QLineEdit::textChanged, this, &CryptoShredder::checkLineEditValidator);
    connect(this->signal, &SignalProxy::signalUpdateListView, this, &CryptoShredder::updateListView);
    connect(this->signal, &SignalProxy::signalSetListViewTags, this, &CryptoShredder::setListViewTags);
    connect(this->signal, &SignalProxy::signalMessageBox, this, [this](const QString& title, const QString& message, const QString& type) { CustomMessageBox::showMessageBox(title, message, type); });
}


/**
 * @brief Destructor of GUI.
 */
CryptoShredder::~CryptoShredder() {
    for (File* file : this->fileList) { //we iterate over fileList
        delete file; //delete each file that we initialzied
    }
	if (this->listViewModel) //if listViewModel object is initialized
		delete this->listViewModel; //delete the listViewModel object
	if (this->keyValidator) //if keyValidator object is initialized
		delete this->keyValidator; //delete the keyValidator object
    if (this->fileHandler) //if fileHandler object is initialized
        delete this->fileHandler; //delete the fileHandler object
    if (this->signal) //if signal object is initialized
        delete this->signal; //delete the signal object
}


/**
 * @brief Method for exiting program securely and managing threads correctly.
 * @param QCloseEvent* event
 */
void CryptoShredder::closeEvent(QCloseEvent* event) {
    if (this->fileHandler && this->isClosing) { //if user already chose to cancel the process
        CustomMessageBox::showMessageBox("Canceling Process", "Please wait for current process to cancel before closing the program.", "Information"); //show messagebox
        this->isClosing = false; //set isClosing flag back to false
        event->ignore(); //ignore the closing event 
    }
    else if (this->fileHandler) { //if exiting and threads still running
        this->cancelProcess(); //call cancelProcess method
        this->isClosing = true; //set isClosing flag to true
        event->ignore(); //ignore the closing event
    }
    else //else there are no threads running
        CryptoShredder::closeServer(); //close listening server
        event->accept(); //accept the closing event 
}


/**
 * @brief Function for initializing listening server for managing one instance.
 */
bool CryptoShredder::initServer() {
    //check if server is already initialized
    if (CryptoShredder::server)
        return true; //return true if already initialized

    //create server to listen for new instances
    CryptoShredder::server = new QLocalServer();

    //check if failed to listen on our server name, if so we remove old entries and try again
    if (!CryptoShredder::server->listen(CryptoShredder::serverName)) {
        QLocalServer::removeServer(CryptoShredder::serverName); //clear server name entries
        //try to listen again for our server name, if failed we return false
        if (!CryptoShredder::server->listen(CryptoShredder::serverName)) {
            delete CryptoShredder::server; //delete server instance
            CryptoShredder::server = NULL; //set server back to null
            return false; //return false to indicate failure
        }
    }
    return true; //return true if server listening successfully
}


/**
 * @brief Function for checking if listening server is running.
 */
bool CryptoShredder::checkServer() {
    QLocalSocket socket; //create socket for checking if server is running
    socket.connectToServer(CryptoShredder::serverName); //try to connect to server
    //wait for server to respond to our request, if we receive response we return true
    if (socket.waitForConnected(100))
        return true; //return true to indicate that server is running
    return false; //return false to indicate that server is down
}


/**
 * @brief Function for closing listening server.
 */
void CryptoShredder::closeServer() {
    //check if listening server is initialized
    if (CryptoShredder::server) {
        CryptoShredder::server->close(); //close listening server
        QLocalServer::removeServer(serverName); //remove server entry
        delete CryptoShredder::server; //delete server instance
        CryptoShredder::server = NULL; //set server back to null
    }
}


/**
 * @brief Method for adding elements to the FileListView in GUI.
 * @param QString item
 */
void CryptoShredder::addItemToListView(const QString& item) {
    QStringList itemList = this->listViewModel->stringList(); //get current item list in the listView
    itemList.append(item); //add the item to item list 
    this->listViewModel->setStringList(itemList); //add item list to the GUI FileListView
}


/**
 * @brief Method to update the listView when thread finishes its work.
 * @param string fileFullName
 * @param string fileName
 * @param string tag
 */
void CryptoShredder::updateListView(const QString& fileFullName, const QString& fileName, const QString& tag) {
    this->GUIMutex.lock(); //lock the GUI mutex to prevent race conditions with threads
    if (this->fileHandler) { //update only if fileHandler object is initialized
        int fileIndex = (int)this->fileDictionary[fileFullName.toStdU16String()]; //get the index of the file we need to update in fileDictionary
        QModelIndex index = this->listViewModel->index(fileIndex, 0); //convert the integer index to QModelIndex for Qt list model
        this->listViewModel->setData(index, fileName + tag, Qt::EditRole); //set the tag in the specific file to indicate that its finished
        this->checkThreads(); //call checkThreads method to determine if we can delete allocated memory
    }
    this->GUIMutex.unlock(); //unlock mutex
}


/**
 * @brief Method to check if threads finished their work so we can delete unnecessary memory.
 */
void CryptoShredder::checkThreads() {
    this->GUIMutex.lock();  //lock the GUI mutex to prevent race conditions with threads
    if (this->fileHandler) { //check only if fileHandler object is initialized
        this->fileHandler->setNumOfThreads(this->fileHandler->getNumOfThreads() - 1); //we decrease the amount of runnning threads with getter and setter methods
        int incrementAmount = 100 / (int)this->fileList.size(); //increment the progress bar with an amount that represents the division of num of files
        this->ui.progressBar->setValue(this->ui.progressBar->value() + incrementAmount); //increment progress bar
        if (this->fileHandler->getNumOfThreads() == 0) { //means that all threads finished their work 
            this->fileHandler->setThreadsRunning(false); //set threadsRunning flag to false indicating all threads finished work
            this->ui.progressBar->setValue(100); //we set the progress bar to 100 when we finish the operation
        }
        if (!(this->fileHandler->areThreadsRunning())) { //if true the operation finished so we clear the necessary elements
            delete this->fileHandler; //delete fileHandler object
            this->fileHandler = NULL; //set pointer of fileHandler to NULL for next operation
            this->fileDictionary.clear(); //clear file dictionary
            this->fileList.clear(); //clear file list
            this->fileCounter = 0; //set the fileCounter back to zero
            if (File::getIsCanceled()) { //if user canceled the operation
                if (this->isClosing) { // if user wants to close program
                    this->isClosing = false; //set isClosing flag to false
                    this->close(); //call close method to close program
                }
                else { //else user didn't want to close program, we show messagebox and continue
                    File::setIsCanceled(false); //set the isCanceled flag back to false
                    if (CryptoShredder::isWipe) //if we're wiping
                        CustomMessageBox::showMessageBox("Wipe Canceled", "Secure wiping for selected files has been canceled, files that were in process were canceled.", "Information"); //show messagebox
                    else { //else we encrypt/decrypt
                        if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                            CustomMessageBox::showMessageBox("Encryption Canceled", "Secure encryption for selected files has been canceled, files that were in process were canceled.", "Information"); //show messagebox
                        else //else we show messagebox for decryption
                            CustomMessageBox::showMessageBox("Decryption Canceled", "Secure decryption for selected files has been canceled, files that were in process were canceled.", "Information"); //show messagebox
                    }
                }
            }
            else if (File::getIsFailed()) { //else if one of threads failed operation
                File::setIsFailed(false); //set the isFailed flag back to false
                if (CryptoShredder::isWipe) //if we're wiping
                    CustomMessageBox::showMessageBox("Wipe Failed", "Secure wiping has failed for one or more selected files.", "Critical"); //show messagebox
                else { //else we encrypt/decrypt
                    if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                        CustomMessageBox::showMessageBox("Encryption Failed", "Secure encryption has failed for one or more selected files.", "Critical"); //show messagebox
                    else //else we show messagebox for decryption
                        CustomMessageBox::showMessageBox("Decryption Failed", "Secure decryption has failed for one or more selected files.", "Critical"); //show messagebox
                }
            }
            else { //else we finished the operation successfully
                if (CryptoShredder::isWipe) //if we're wiping
                    CustomMessageBox::showMessageBox("Wipe Finished", "Secure wiping for selected files completed successfully.", "Information"); //show messagebox with success message
                else { //else we encrypt/decrypt
                    if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                        CustomMessageBox::showMessageBox("Encryption Finished", "Secure encryption for selected files completed successfully.", "Information"); //show messagebox
                    else //else we show messagebox for decryption
                        CustomMessageBox::showMessageBox("Decryption Finished", "Secure decryption for selected files completed successfully.", "Information"); //show messagebox
                }
            }
            this->ui.RemoveFilesCheckBox->setEnabled(true); //set the remove checkBox to be unclickable
            this->ui.CipherCheckBox->setEnabled(true); //set the cipher checkBox to be clickable
            this->ui.PassesSpinBox->setEnabled(true); //set the passes spin box to be clickable
            this->ui.KeyLineEdit->setEnabled(true); //set the key line edit to be clickable
            this->ui.progressBar->setTextVisible(false); //set the progressBar text to false not showing the text
            this->ui.progressBar->setValue(0); //set the progress bar back to zero
        }
    }
    this->GUIMutex.unlock(); //unlock mutex
}


/**
 * @brief Method to set tags for files in the listView in GUI.
 * @param QString tag
 * @param QString currentTag
 */
void CryptoShredder::setListViewTags(const QString& tag, const QString& currentTag) {
    QStringList items = this->listViewModel->stringList(); //get item list from the listView model
    if (!items.isEmpty()) { //check if the listView is not empty
        if (currentTag.isEmpty()) { //if string is not empty we need to add the tags to items that in the fileDictionary
            for (const auto& [fileName, index] : this->fileDictionary) { //iterate over fileDictionary items
                items[index] += tag; //add the tag to the items
            }
        }
        else { //if not empty we need to add the tag to all items
            for (int i = 0; i < items.size(); i++) { //iterate over listView items
                int lastPosition = items[i].lastIndexOf(currentTag); //get the last index position of the file name without current tag
                if (lastPosition != -1) //if true we found the position we need to add new tag from
                    items[i] = items[i].left(lastPosition) + tag; //add the new tag to the file name
            }
        }
        this->listViewModel->setStringList(items); //set the list with updated tags
    }
}


/**
 * @brief Method for processing files for wiping or encryption/decryption.
 */
void CryptoShredder::processFiles() {
    if (this->fileHandler == NULL && !(this->fileDictionary.empty())) { //if true we can start the wipe
		bool result = true; //represents result from user choice in message box
        if (CryptoShredder::isWipe) { //if wipe flag is set we're wiping the files
            result = CustomMessageBox::showMessageBox("Starting File Wiping", "All data in the selected files will be permanently erased and cannot be recovered. Proceed?", "Question");
            if (!result) //if user chose "No" we do not start the wiping process
                return; //finish the method's work

            //initialize fileHandler and start wiping process
            this->fileHandler = new FileHandler(this->fileList, this->signal); //initialize the fileHandler with the required parameters
            if (this->fileHandler) { //check if we successfully initialized the fileHandler
                int numOfPasses = this->ui.PassesSpinBox->value(); //get number of passes from GUI
                bool toRemove = this->ui.RemoveFilesCheckBox->isChecked(); //check if user wants to delete files after wipe
                this->ui.progressBar->setTextVisible(true); //set the progressBar text to be visible
                this->ui.RemoveFilesCheckBox->setEnabled(false); //set the remove checkBox to be unclickable
                this->ui.PassesSpinBox->setEnabled(false); //set the passes spin box to be unclickable
                this->setListViewTags(" - Processing..."); //set tags to the items in listView indicating that wiping started
                this->fileHandler->initWipe(numOfPasses, toRemove); //call our initWipe method to initiate wipe on specified files
            }
            else //if we failed to initialize fileHandler we show error message
                CustomMessageBox::showMessageBox("Failed Establishing Process", "Error, couldn't establish process request. Please try again.", "Critical");
        }
        else { //else we encrypt/decrypt the files
            string key = this->ui.KeyLineEdit->text().toStdString(); //get the key from GUI
            //check if given key is valid and show an error message if not
            if (key.length() == 0) {
                CustomMessageBox::showMessageBox("Invalid key", "Error, the key field cannot be empty. Please enter a valid AES key of length 16, 24 or 32 characters.", "Critical");
                return; //stop the method
            }
            else if (key.length() != 16 && key.length() != 24 && key.length() != 32) {
                CustomMessageBox::showMessageBox("Invalid key", "Error, the provided key is invalid. Please enter a valid AES key of length 16, 24, or 32 characters.", "Critical");
                return; //stop the method
            }

            if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                result = CustomMessageBox::showMessageBox("Starting File Encryption", "The selected files will be encrypted and can only be accessed with the correct key. Proceed?", "Question");
            else //else we show messagebox for decryption
                result = CustomMessageBox::showMessageBox("Starting File Decryption", "The selected files will be decrypted using the given key. Incorrect key may corrupt recovered data. Proceed?", "Question");
            if (!result) //if user chose "No" we do not start the encryption/decryption process
                return; //finish the method's work

            //initialize fileHandler and start encryption/decryption process
            this->fileHandler = new FileHandler(this->fileList, this->signal); //initialize the fileHandler with the required parameters
            if (this->fileHandler) { //check if we successfully initialized the fileHandler
                bool decrypt = this->ui.CipherCheckBox->isChecked(); //get state of decrypt checkbox from GUI
                this->ui.progressBar->setTextVisible(true); //set the progressBar text to be visible
                this->ui.CipherCheckBox->setEnabled(false); //set the cipher checkBox to be unclickable
                this->ui.KeyLineEdit->setEnabled(false); //set the key line edit to be unclickable
                this->setListViewTags(" - Processing..."); //set tags to the items in listView indicating that wiping started
                this->fileHandler->initCipher(key, decrypt); //call our initCipher method to initiate encryption/decryption on specified files
            }
            else //if we failed to initialize fileHandler we show error message
                CustomMessageBox::showMessageBox("Failed Establishing Process", "Error, couldn't establish process request. Please try again.", "Critical");
        }
    }
    else if (this->fileHandler == NULL && this->fileDictionary.empty()) { //if user didn't choose files we show messagebox with error
        if (CryptoShredder::isWipe) //if we're wiping
            CustomMessageBox::showMessageBox("No Files Chosen Error", "Error, please choose files before starting wipe.", "Critical");
        else { //else we encrypt/decrypt
            if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                CustomMessageBox::showMessageBox("No Files Chosen Error", "Error, please choose files before starting encryption.", "Critical");
            else //else we show messagebox for decryption
                CustomMessageBox::showMessageBox("No Files Chosen Error", "Error, please choose files before starting decryption.", "Critical");
        }
    }
    else if (this->fileHandler != NULL) { //if there's a wipe in progress we show messagebox with error
        if (CryptoShredder::isWipe) //if we're wiping
            CustomMessageBox::showMessageBox("Wiping In Progress Error", "Error, please wait for current wiping process to finish.", "Warning");
        else { //else we encrypt/decrypt
            if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                CustomMessageBox::showMessageBox("Encryption In Progress Error", "Error, please wait for current encryption process to finish.", "Warning");
            else //else we show messagebox for decryption
                CustomMessageBox::showMessageBox("Decryption In Progress Error", "Error, please wait for current decryption process to finish.", "Warning");
        }
    }
    else { //else unknown error occurred
        CustomMessageBox::showMessageBox("Unknown Error", "Unknown error occurred, please try again.", "Critical");
    }
}


/**
 * @brief Method to open the file dialog for choosing files for wipe.
 */
void CryptoShredder::openFileDialog() {
    if (this->fileHandler == NULL) {
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation); //we set a QString to point to the desktop directory 
        //set the file dialog with appropriate settings for our wiping
        QStringList selectedFiles = QFileDialog::getOpenFileNames(nullptr, "Select Files", desktopPath, "All Files (*);;Text Files (*.txt);;Image Files (*.png *.jpg);;PDF Files (*.pdf);;Word Files (*.docx *.doc);;PowerPoint Files (*.pptx *.ppt);;Excel Files (*.xlsx *.xls)");

        //check if files were selected
        if (!selectedFiles.isEmpty()) {
            bool isMaxFiles = false; //flag for indicating that user added more then the maximum files allowed
            size_t MaxNumOfFiles; //represents the maximum allowed files to wipe at once
            bool isMaxFileSize = false; //flag for indicating files exceeds maximum size allowed
            size_t MaxFileSize; //represents the maximum allowed file size
            if (CryptoShredder::isWipe) {
                this->setListViewTags(" - Finished", " - Wiped Successfully"); //set previous files tags that were wiped to finished 
                MaxNumOfFiles = 20; //set max number of files to 20
                MaxFileSize = 600LL * 1024LL * 1024LL; // set max file size to 600 MB
            }
            else {
                this->setListViewTags(" - Finished", " - Encrypted Successfully"); //set previous files tags that were encrypted to finished
                this->setListViewTags(" - Finished", " - Decrypted Successfully"); //set previous files tags that were decrypted to finished
                MaxNumOfFiles = 10; //set max number of files to 10
                MaxFileSize = 30LL * 1024LL * 1024LL; // set max file size to 30 MB
            }
            for (const QString& filePath : selectedFiles) { //we process the selected file paths
                filesystem::path FilePath(filePath.toStdU16String()); //get file path using filesystem path method
                if (this->fileCounter >= MaxNumOfFiles) //if true we exceeded the maximum files allowed
                    isMaxFiles = true; //set isMaxFiles flag to true
                if (filesystem::file_size(FilePath) > MaxFileSize) //if file exceeds maximum allowed size
                    isMaxFileSize = true; //set isMaxFileSize flag to true indicating user added large file
                else { //else file is below maximum size we proccess it
                    u16string fileName = FilePath.stem().u16string() + FilePath.extension().u16string(); //get file name with filesystem
                    if (this->fileDictionary.find(fileName) == this->fileDictionary.end()) { //we check if the file already exists in our file dictionary
                        if (isMaxFiles) //if isMaxFiles flag is set
                            break; //break from the loop
						File* file = new File(FilePath); //initialize a new file object with the given path
                        this->addItemToListView((file->getFullName().size() > 34) ? QString::fromStdU16String(file->getName().substr(0, 30) + u"..." + file->getExtention()) : QString::fromStdU16String(file->getFullName())); //add the file name to our FileListView in GUI
                        this->fileDictionary[file->getFullName()] = this->listViewCounter; //add the file name as the key and the counter representing its index in the listView as value
                        this->listViewFileDictionary[this->listViewCounter] = file->getFullPath(); //add the file to listViewFileDictionary for later use in file viewer
                        this->fileList.push_back(file); //add each file object to our filList
                        this->listViewCounter++; //increase listView counter
                        this->fileCounter++; //increase file counter
                    }
                }
            }
            if (isMaxFileSize && isMaxFiles) //if both flags are set we show messagebox with combined message indicating that both file size and number of files were exceeded
                CustomMessageBox::showMessageBox("File Size and File Count Exceeded", "Several files exceed " + QString::number(MaxFileSize / (1024LL * 1024LL)) + " MB and the total number of files exceeds " + QString::number(MaxNumOfFiles) + ". These files were not added.", "Information");            
            else if (isMaxFileSize) //if isMaxFileSize flag is set we show messagebox that notifies the user for a file that exceeded allowed file size
                CustomMessageBox::showMessageBox("File Size Exceeded", "Several files exceed " + QString::number(MaxFileSize / (1024LL * 1024LL)) + " MB and were not added.", "Information");
            else if (isMaxFiles) //if isMaxFiles flag is set we show messagebox that notifies the user he exceeded the allowed number of files for wipe
                CustomMessageBox::showMessageBox("File Count Exceeded", "Maximum number of files allowed at a time is " + QString::number(MaxNumOfFiles) + ". Extra files were not added.", "Information");
        }
    }
    else { //else we show messagebox indicating user cannot add files while wipe in process
        if (CryptoShredder::isWipe) //if we're wiping
            CustomMessageBox::showMessageBox("Wipe In Progress Error", "Error, cannot add files while wipe in progress.", "Warning");
        else { //else we encrypt/decrypt
            if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                CustomMessageBox::showMessageBox("Encryption In Progress Error", "Error, cannot add files while encryption in progress.", "Warning");
            else //else we show messagebox for decryption
                CustomMessageBox::showMessageBox("Decryption In Progress Error", "Error, cannot add files while decryption in progress.", "Warning");
        }
    }
}


/**
 * @brief Method to clear the listView items.
 */
void CryptoShredder::clearContents() {
    if (this->fileHandler == NULL) { //means there's no wipe in progress
        for (File* file : this->fileList) { //we iterate over fileList
            delete file; //delete each file that we initialzied
        }
        this->fileList.clear(); //clear fileList
        this->fileDictionary.clear(); //clear file dictionary
        this->listViewFileDictionary.clear(); //clear listView file dictionary
        this->listViewModel->setStringList(QStringList()); //clear current listView items
        this->listViewCounter = 0; //set the listViewCounter back to zero
        this->fileCounter = 0; //set the fileCounter back to zero
    }
    else { //else we can't clear the screen
        if (CryptoShredder::isWipe) //if we're wiping
            CustomMessageBox::showMessageBox("Unable To Clear Screen", "Error, cannot clear the screen while wipe in progress.", "Warning"); //we show messagebox with error
        else { //else we encrypt/decrypt
            if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                CustomMessageBox::showMessageBox("Unable To Clear Screen", "Error, cannot clear the screen while encryption in progress.", "Warning"); //we show messagebox with error
            else //else we show messagebox for decryption
                CustomMessageBox::showMessageBox("Unable To Clear Screen", "Error, cannot clear the screen while decryption in progress.", "Warning"); //we show messagebox with error
        }
    }
}


/**
 * @brief Method to stop the wiping process.
 */
void CryptoShredder::cancelProcess() {
    if (this->fileHandler) {
        bool result = true; //represents result from user choice in message box
        if (CryptoShredder::isWipe) //if we're wiping
            result = CustomMessageBox::showMessageBox("Cancel Wipe", "Stopping the wipe will permanently corrupt the files currently being processed. Proceed?", "Question");        
        else { //else we encrypt/decrypt
            if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                result = CustomMessageBox::showMessageBox("Cancel Encryption", "Stopping encryption will permanently corrupt the files currently being processed. Proceed?", "Question");
            else //else we show messagebox for decryption
                result = CustomMessageBox::showMessageBox("Cancel Decryption", "Stopping decryption will permanently corrupt the files currently being processed. Proceed?", "Question");
        }
        if (!result) //if user chose "No" we do not cancel the wiping process
            return; //finish the method's work
        this->fileHandler->cancelProcess(); //else we call cancelProcess method to indicate for cancelation
    }
}


/**
 * @brief Method for double click events in file listView.
 * @param QModelIndex index
 */
void CryptoShredder::doubleClickedFile(const QModelIndex& index) {
    if (this->fileHandler == NULL) { //if true we can open file for viewing
        int fileIndex = index.row(); //get fileIndex in integer
        u16string filePath = this->listViewFileDictionary[fileIndex]; //get file path from the listView file dictionary
        filesystem::path FilePath(filePath); //get file path using filesystem path method
        if (!filesystem::exists(FilePath)) { //check if file still exists
            CustomMessageBox::showMessageBox("File Not Found", "Error, the selected file could not be found. It might have been moved or deleted.", "Critical"); //show messagebox with error
            return; //stop the method
		}
        u16string fileName = FilePath.stem().u16string() + FilePath.extension().u16string(); //get file name with filesystem
        this->fileViewer = FileViewer::getInstance(this, QString::fromStdU16String(filePath), (fileName.size() > 34) ? QString::fromStdU16String(FilePath.stem().u16string().substr(0, 30) + u"..." + FilePath.extension().u16string()) : QString::fromStdU16String(fileName)); //create a new instance of fileViewer to show file's content to user
    }
    else { //else wipe in progress
        if (CryptoShredder::isWipe) //if we're wiping
            CustomMessageBox::showMessageBox("Unable To Open File", "Error, cannot open file while wipe in progress.", "Warning"); //show messagebox with error indicating wipe in progress
        else { //else we encrypt/decrypt
            if (!this->ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                CustomMessageBox::showMessageBox("Unable To Open File", "Error, cannot open file while encryption in progress.", "Warning"); //show messagebox with error indicating wipe in progress
            else //else we show messagebox for decryption
                CustomMessageBox::showMessageBox("Unable To Open File", "Error, cannot open file while decryption in progress.", "Warning"); //show messagebox with error indicating wipe in progress
        }
    }
}


/**
 * @brief Method to check if key is valid AES key in GUI.
 */
void CryptoShredder::checkLineEditValidator() {
    QString text = this->ui.KeyLineEdit->text(); //get the text inside key line edit
    if (text.length() != 0 && text.length() != 16 && text.length() != 24 && text.length() != 32) { //if true the text isn't valid
        QString styleSheet = "QLineEdit {"
            "background-color: rgba(32,33,35,255);"
            "color: rgb(245,245,245);"
            "border-radius: 15px;"
            "border-style: outset;"
            "border-width: 2px;"
            "border-color: rgb(139,0,0);"
            "padding: 4px;"
            "}";
        this->ui.KeyLineEdit->setStyleSheet(styleSheet); //set the style sheet with red border
    }
    else { //else the text is valid 
        QString styleSheet = "QLineEdit {"
            "background-color: rgba(32,33,35,255);"
            "color: rgb(245,245,245);"
            "border-radius: 15px;"
            "border-style: outset;"
            "border-width: 2px;"
            "border-color: black;"
            "padding: 4px;"
            "}";
        this->ui.KeyLineEdit->setStyleSheet(styleSheet); //set the style sheet with regular border
    }
}


/**
 * @brief Method for handling operation modes selection in GUI.
 */
void CryptoShredder::changeOperationMode() {
    if (this->fileHandler) { //if true there's a process in progress
        CustomMessageBox::showMessageBox("Unable To Change Mode", "Error, cannot change operation mode while current process in progress.", "Warning"); //show error messagebox
    }
    else { //else we can change operation mode
        if (!CryptoShredder::isWipe) { //if true we change to wipe mode
			this->ui.OptionsButton->setIcon(QIcon("images/cipherIcon.png")); //set OptionsButton icon
            this->ui.OptionsButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Encrypt/Decrypt files.</span></p></body></html>");
            this->ui.TitleIconLabel->setPixmap(QPixmap("images/wipeIcon.png")); //set TitleIconLabel
            this->ui.CipherCheckBox->setChecked(false); //reset CipherCheckBox state
            this->ui.KeyLineEdit->clear(); //clear KeyLineEdit
            this->ui.KeyLineEdit->clearFocus(); //clear KeyLineEdit focus
            this->ui.cipherFrame->setVisible(false); //hide cipherFrame and its contents
            this->ui.wipeFrame->setVisible(true); //show wipeFrame and its contents
            this->ui.TopLabel->setText("Files Scheduled For Wipe"); //set TopLabel
            this->ui.ProcessButton->setText("Wipe Files"); //set ProcessButton
            this->ui.ChooseFilesButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Choose files, maximum 20 files.</span></p></body></html>");
            this->ui.ProcessButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Start secure wipe on selected files.</span></p></body></html>");
            this->clearContents(); //clear current contents
            CryptoShredder::isWipe = true; //set isWipe flag to indicate wipe mode
        }
        else { //else we change to enryption/decryption mode
            this->ui.OptionsButton->setIcon(QIcon("images/wipeIcon.png")); //set OptionsButton icon
            this->ui.OptionsButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Wipe files.</span></p></body></html>");
            this->ui.TitleIconLabel->setPixmap(QPixmap("images/cipherIcon.png"));
            this->ui.PassesSpinBox->setValue(1); //reset PassesSpinBox state
            this->ui.RemoveFilesCheckBox->setChecked(false);//reset RemoveFileCheckBox state
            this->ui.wipeFrame->setVisible(false); //hide wipeFrame and contents
            this->ui.cipherFrame->setVisible(true); //show cipherFrame and its contents
            this->changeCipherMode(); //set GUI elements
            this->ui.ChooseFilesButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Choose files, maximum 10 files.</span></p></body></html>");
            this->clearContents(); //clear current contents
            CryptoShredder::isWipe = false; //set isWipe flag to indicate wipe mode isn't active
        }
    }
}


/**
 * @brief Method for handling cipher modes selection in GUI.
 */
void CryptoShredder::changeCipherMode() {
    if (this->ui.CipherCheckBox->isChecked()) { //if true we set GUI to decryption mode
        this->ui.TopLabel->setText("Files Scheduled For Decryption"); //set TopLabel
        this->ui.ProcessButton->setText("Decrypt Files"); //set ProcessButton
        this->ui.ProcessButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Start decryption on selected files.</span></p></body></html>");
    }
    else { //else we set GUI to encryption mode
        this->ui.TopLabel->setText("Files Scheduled For Encryption"); //set TopLabel
        this->ui.ProcessButton->setText("Encrypt Files"); //set ProcessButton
        this->ui.ProcessButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Start encryption on selected files.</span></p></body></html>");
    }
}


/**
 * @brief Method to open the general Information window to display Information about FileShredder.
 */
void CryptoShredder::showInfoWindow() {
    this->infoWindow = InfoWindow::getInstance(this); //create new instance of InfoWindow
}