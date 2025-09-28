#include "CryptoShredder.h"


bool CryptoShredder::wipe = true; //set wipe flag to true by default


/**
 * @brief Constructor of GUI.
 * @param QWidget* parent
 */
CryptoShredder::CryptoShredder(QWidget* parent) : QMainWindow(parent) {
    //setup ui elements
    ui.setupUi(this);

    //setup GUI elements at start of program//
    ui.KeyLabel->setVisible(false); //hide KeyLabel
    ui.KeyLineEdit->setVisible(false); //hide KeyLineEdit
    ui.CipherCheckBox->setVisible(false); //hide CipherCheckBox
    this->listViewModel = new QStringListModel(); //initialize the model for listView in GUI
    this->signal = new SignalProxy(); //initialize the signal proxy for the foreigner classes
    this->infoImageLabel = new ImageLabel("images/infoIcon.png", QPoint(1020, 10), QSize(40, 40), this); //set the info icon in GUI
    this->infoImageLabel->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>General information<br>about CryptoShredder.</span></p></body></html>"); //set toolTip for info icon
    this->optionsImageLabel = new ImageLabel("images/cipherIcon.png", QPoint(955, 10), QSize(40, 40), this); //set the options icon in GUI
    this->optionsImageLabel->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Encrypt/Decrypt files.</span></p></body></html>"); //set toolTip for options icon
    this->keyValidator = new QRegExpValidator(QRegExp("[a-zA-Z0-9\\W]{16}|[a-zA-Z0-9\\W]{24}|[a-zA-Z0-9\\W]{32}")); //regular expression for key 
    ui.KeyLineEdit->setValidator(this->keyValidator); //set the key validator
    ui.FileListView->setModel(listViewModel); //set the list model for listView in GUI to add elements
    ui.FileListView->setEditTriggers(QAbstractItemView::NoEditTriggers); //set the listView to be uneditable
    ui.PassesSpinBox->findChild<QLineEdit*>()->setReadOnly(true); //remove the lineEdit in spinBox

    //slots for ui elements//
    connect(ui.ProcessButton, &QPushButton::clicked, this, &CryptoShredder::processFiles);
    connect(ui.CancelButton, &QPushButton::clicked, this, &CryptoShredder::cancelProcess);
    connect(ui.ChooseFilesButton, &QPushButton::clicked, this, &CryptoShredder::openFileDialog);
    connect(ui.ClearScreenButton, &QPushButton::clicked, this, &CryptoShredder::clearContents);
    connect(ui.FileListView, &QAbstractItemView::doubleClicked, this, &CryptoShredder::doubleClickedFile);
    connect(ui.CipherCheckBox, &QCheckBox::stateChanged, this, &CryptoShredder::cipherCheckBoxClicked);
    connect(ui.KeyLineEdit, &QLineEdit::textChanged, this, &CryptoShredder::checkLineEditValidator);
    connect(this->signal, &SignalProxy::signalUpdateListView, this, &CryptoShredder::updateListView);
    connect(this->signal, &SignalProxy::signalMessageBox, this, &CryptoShredder::showMessageBox);
    connect(this->signal, &SignalProxy::signalSetListViewTags, this, &CryptoShredder::setListViewTags);
    connect(this->infoImageLabel, &ImageLabel::clicked, this, &CryptoShredder::infoLabelClicked);
    connect(this->optionsImageLabel, &ImageLabel::clicked, this, &CryptoShredder::optionsLabelClicked);
}


/**
 * @brief Destructor of GUI.
 */
CryptoShredder::~CryptoShredder() {
    if (this->fileHandler) //if there's an object of fileHandler initialized
        delete this->fileHandler; //delete the fileHandler object
    delete this->listViewModel; //delete the list view model
    delete this->keyValidator; //delete the key validator
    delete this->signal; //delete the signal object
    delete this->infoImageLabel; //delete the infoImageLabel object
    delete this->optionsImageLabel; //delete the optionsImageLabel object
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
 * @param "QString item
 */
void CryptoShredder::updateListView(const QString& fileDictionaryName, const QString& fileName, const QString& tag) {
    this->GUIMutex.lock(); //lock the GUI mutex to prevent race conditions with threads
    if (this->fileHandler) { //update only if fileHandler object is initialized
        int fileIndex = this->fileDictionary[fileDictionaryName.toStdString()]; //get the index of the file we need to update in fileDictionary
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
        int incrementAmount = 100 / this->filePathList.size(); //increment the progress bar with an amount that represents the division of num of files
        ui.progressBar->setValue(ui.progressBar->value() + incrementAmount); //increment progress bar
        if (this->fileHandler->getNumOfThreads() == 0) { //means that all threads finished their work 
            this->fileHandler->setThreadsRunning(false); //set threadsRunning flag to false indicating all threads finished work
            ui.progressBar->setValue(100); //we set the progress bar to 100 when we finish the operation
        }
        if (!(this->fileHandler->areThreadsRunning())) { //if true the operation finished so we clear the necessary elements
            this->filePathList.clear(); //clear filePathList
            this->fileDictionary.clear(); //clear file dictionary
            this->fileCounter = 0; //set the fileCounter back to zero
            delete this->fileHandler; //delete fileHandler object
            this->fileHandler = NULL; //set pointer of fileHandler to NULL for next operation
            if (File::getIsCanceled()) { //if true user canceled the operation
                File::setIsCanceled(false); //set the isCanceled flag back to false
                if (this->wipe) //if we're wiping
                    this->showMessageBox("Wipe Canceled", "Secure wiping for selected files has been canceled, files that were in process were canceled.", "information"); //show messagebox
                else { //else we encrypt/decrypt
                    if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                        this->showMessageBox("Encryption Canceled", "Secure encryption for selected files has been canceled, files that were in process were canceled.", "information"); //show messagebox
                    else //else we show messagebox for decryption
                        this->showMessageBox("Decryption Canceled", "Secure decryption for selected files has been canceled, files that were in process were canceled.", "information"); //show messagebox
                }
            }
            else { //else we finished the operation
                if (this->wipe) //if we're wiping
                    this->showMessageBox("Wipe Finished", "Secure wiping for selected files completed successfully.", "information"); //show messagebox with success message
                else { //else we encrypt/decrypt
                    if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                        this->showMessageBox("Encryption Finished", "Secure encryption for selected files completed successfully.", "information"); //show messagebox
                    else //else we show messagebox for decryption
                        this->showMessageBox("Decryption Finished", "Secure decryption for selected files completed successfully.", "information"); //show messagebox
                }
            }
            ui.RemoveFilesCheckBox->setEnabled(true); //set the remove checkBox to be unclickable
            ui.CipherCheckBox->setEnabled(true); //set the cipher checkBox to be clickable
            ui.PassesSpinBox->setEnabled(true); //set the passes spin box to be clickable
            ui.KeyLineEdit->setEnabled(true); //set the key line edit to be clickable
            ui.progressBar->setTextVisible(false); //set the progressBar text to false not showing the text
            ui.progressBar->setValue(0); //set the progress bar back to zero
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
        if (currentTag == NULL) { //if NULL we need to add the tags to items that in the fileDictionary
            for (const auto& [fileName, index] : this->fileDictionary) { //iterate over fileDictionary items
                items[index] += tag; //add the tag to the items
            }
        }
        else { //if not NULL we need to add the tag to all items
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
 * @brief Method we use to show messagebox with given parameters, also for signals with proxy object.
 * @param QString title
 * @param QString text
 * @param type
 */
QMessageBox::StandardButton CryptoShredder::showMessageBox(const QString& title, const QString& text, const QString& type) {
    if (type == "critical")
        return QMessageBox::critical(nullptr, title, text);
    else if (type == "warning")
        return QMessageBox::warning(nullptr, title, text);
    else if (type == "information")
        return QMessageBox::information(nullptr, title, text);
    else if (type == "question")
        return QMessageBox::question(nullptr, title, text);
    else
        return (QMessageBox::StandardButton)QMessageBox::NoIcon(nullptr, title, text);
}


/**
 * @brief Method for processing files for wiping or encryption/decryption.
 */
void CryptoShredder::processFiles() {
    if (this->fileHandler == NULL && !(this->fileDictionary.empty())) { //if true we can start the wipe
        QMessageBox::StandardButton choice;
        if (this->wipe) //if we're wiping
            choice = showMessageBox("Starting File Wiping", "Please be aware that all data in the selected files will be permanently erased and cannot be recovered. Are you sure you want to proceed?", "question");
        else { //else we encrypt/decrypt
            if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                choice = showMessageBox("Starting File Encryption", "Please be aware that all data in the selected files will be encrypted with the specified key, this means the original data will be transformed and can only be accessed with the correct key. Are you sure you want to proceed?", "question");
            else //else we show messagebox for decryption
                choice = showMessageBox("Starting File Decryption", "Please be aware that all data in the selected files will be decrypted with the specified key, if given key isn't correct key used to encrypt files then the recovered data might not represent the original data. Are you sure you want to proceed?", "question");
        }
        if (choice == QMessageBox::No) //if user chose "No" we do not start the wiping process
            return; //finish the method's work

        if (this->wipe) { //if wipe flag is set we're wiping the files
            this->fileHandler = new FileHandler(this->filePathList, this->signal); //initialize the fileHandler with the required parameters
            if (this->fileHandler) { //check if we successfully initialized the fileHandler
                int numOfPasses = ui.PassesSpinBox->value(); //get number of passes from GUI
                bool toRemove = ui.RemoveFilesCheckBox->isChecked(); //check if user wants to delete files after wipe
                ui.progressBar->setTextVisible(true); //set the progressBar text to be visible
                ui.RemoveFilesCheckBox->setEnabled(false); //set the remove checkBox to be unclickable
                ui.PassesSpinBox->setEnabled(false); //set the passes spin box to be unclickable
                this->setListViewTags(" - Processing.."); //set tags to the items in listView indicating that wiping started
                this->fileHandler->initWipe(numOfPasses, toRemove); //call our initWipe method to initiate wipe on specified files
            }
            else //if we failed to initialize fileHandler we show error message
                this->showMessageBox("Failed Establishing Process", "Error, Couldn't establish process request. Please try again.", "critical");
        }
        else { //else we encrypt/decrypt the files
            string key = ui.KeyLineEdit->text().toStdString(); //get the key from GUI
            if (key.length() == 0) {
                this->showMessageBox("Invalid key", "Error, The key field must not be left empty. Please provide a key that matches AES requirements, accepted key lengths include 16, 24 or 32 characters.", "critical");
                return; //stop the method
            }
            else if (key.length() != 16 && key.length() != 24 && key.length() != 32) { //if true the key is invalid so we show error
                this->showMessageBox("Invalid key", "Error, Please provide a key that matches AES requirements, accepted key lengths include 16, 24 or 32 characters.", "critical");
                return; //stop the method
            }
            this->fileHandler = new FileHandler(this->filePathList, this->signal); //initialize the fileHandler with the required parameters
            if (this->fileHandler) { //check if we successfully initialized the fileHandler
                bool decrypt = ui.CipherCheckBox->isChecked(); //get state of decrypt checkbox from GUI
                ui.progressBar->setTextVisible(true); //set the progressBar text to be visible
                ui.CipherCheckBox->setEnabled(false); //set the cipher checkBox to be unclickable
                ui.KeyLineEdit->setEnabled(false); //set the key line edit to be unclickable
                this->setListViewTags(" - Processing.."); //set tags to the items in listView indicating that wiping started
                this->fileHandler->initCipher(key, decrypt); //call our initCipher method to initiate encryption/decryption on specified files
            }
            else //if we failed to initialize fileHandler we show error message
                this->showMessageBox("Failed Establishing Process", "Error, Couldn't establish process request. Please try again.", "critical");
        }
    }
    else if (this->fileHandler == NULL && this->fileDictionary.empty()) { //if user didn't choose files we show messagebox with error
        if (this->wipe) //if we're wiping
            this->showMessageBox("No Files Chosen Error", "Error, Please choose files before starting wipe.", "critical");
        else { //else we encrypt/decrypt
            if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                this->showMessageBox("No Files Chosen Error", "Error, Please choose files before starting encryption.", "critical");
            else //else we show messagebox for decryption
                this->showMessageBox("No Files Chosen Error", "Error, Please choose files before starting decryption.", "critical");
        }
    }
    else if (this->fileHandler != NULL) { //if there's a wipe in progress we show messagebox with error
        if (this->wipe) //if we're wiping
            this->showMessageBox("Wiping In Progress Error", "Error, Please wait for current wiping process to finish.", "warning");
        else { //else we encrypt/decrypt
            if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                this->showMessageBox("Encryption In Progress Error", "Error, Please wait for current encryption process to finish.", "warning");
            else //else we show messagebox for decryption
                this->showMessageBox("Decryption In Progress Error", "Error, Please wait for current decryption process to finish.", "warning");
        }
    }
    else { //else unknown error occurred
        this->showMessageBox("Unknown Error", "Unknown error occurred, please try again.", "critical");
    }
}


/**
 * @brief Method to open the file dialog for choosing files for wipe.
 */
void CryptoShredder::openFileDialog() {
    if (this->fileHandler == NULL) {
        QString desktopPath = QDir::homePath() + "/Desktop"; //we set a QString to point to the desktop directory 
        //set the file dialog with appropriate settings for our wiping
        QStringList selectedFiles = QFileDialog::getOpenFileNames(nullptr, "Select Files", desktopPath, "All Files (*);;Text Files (*.txt);;Image Files (*.png *.jpg);;PDF Files (*.pdf);;Word Files (*.docx *.doc);;PowerPoint Files (*.pptx *.ppt);;Excel Files (*.xlsx *.xls)");

        //check if files were selected
        if (!selectedFiles.isEmpty()) {
            bool isMaxFiles = false; //flag for indicating that user added more then the maximum files allowed
            int MaxNumOfFiles; //represents the maximum allowed files to wipe at once
            bool isMaxFileSize = false; //flag for indicating files exceeds maximum size allowed
            qint64 MaxFileSize; //represents the maximum allowed file size
            if (this->wipe) {
                this->setListViewTags(" - Finished", " - Wiped Successfully"); //set previous files tags that were wiped to finished 
                MaxNumOfFiles = 20; //set max number of files to 20
                MaxFileSize = 600LL * 1024LL * 1024LL; // set max file size to 600MB
            }
            else {
                this->setListViewTags(" - Finished", " - Encrypted Successfully"); //set previous files tags that were encrypted to finished 
                this->setListViewTags(" - Finished", " - Decrypted Successfully"); //set previous files tags that were decrypted to finished 
                MaxNumOfFiles = 10; //set max number of files to 10
                MaxFileSize = 6LL * 1024LL * 1024LL; // set max file size to 6MB
            }
            for (const QString& filePath : selectedFiles) { //we process the selected file paths
                QFileInfo fileInfo(filePath); //get info about the file
                if (this->fileCounter >= MaxNumOfFiles) //if true we exceeded the maximum files allowed
                    isMaxFiles = true; //set isMaxFiles flag to true
                if (fileInfo.size() > MaxFileSize) //if file exceeds maximum allowed size
                    isMaxFileSize = true; //set isMaxFileSize flag to true indicating user added large file
                else { //else file is below maximum size we proccess it
                    string FilePath = filePath.toStdString(); //save file path in a string variable
                    filesystem::path p(FilePath); //call filesystem path method to get name of file
                    QString fileName = QString::fromStdString(p.stem().string() + p.extension().string()); //save name of file in QString variable for GUI
                    auto dictIterator = this->fileDictionary.find(fileName.toStdString()); //we check if the file already exists in our file dictionary
                    if (dictIterator == this->fileDictionary.end()) { //if true and our dictIterator points to the end of dictionary it means the file is not in dictionary
                        if (isMaxFiles) //if isMaxFiles flag is set
                            break; //break from the loop
                        this->addItemToListView((fileName.size() > 34) ? QString::fromStdString(p.stem().string().substr(0, 30) + "..." + p.extension().string()) : fileName); //add the file name to our FileListView in GUI
                        this->fileDictionary[fileName.toStdString()] = this->listViewCounter; //add the file name as the key and the counter representing its index in the listView as value
                        this->listViewFileDictionary[this->listViewCounter] = FilePath; //add the file to listViewFileDictionary for later use in file viewer
                        this->filePathList.push_back(filePath.toStdString()); //add each file path to out filePathList
                        this->listViewCounter++; //increase listView counter
                        this->fileCounter++; //increase file counter
                    }
                }
            }
            if (isMaxFileSize && isMaxFiles) //if both flags are set we show messagebox with combined message indicating that both file size and number of files were exceeded
                this->showMessageBox("Maximum File Size And Maximum Number Of Files Exceeded", "Files larger than " + QString::number(MaxFileSize / (qint64)(1024 * 1024)) + "MB were detected and cannot be added, also the maximum allowed number of files (limited to " + QString::number(MaxNumOfFiles) + " files) has also been exceeded.", "information");
            else if (isMaxFileSize) //if isMaxFileSize flag is set we show messagebox that notifies the user for a file that exceeded allowed file size
                this->showMessageBox("Maximum File Size", "Files larger then " + QString::number(MaxFileSize / (qint64)(1024 * 1024)) + "MB detected, files that exceed the limit were not added.", "information");
            else if (isMaxFiles) //if isMaxFiles flag is set we show messagebox that notifies the user he exceeded the allowed number of files for wipe
                this->showMessageBox("Maximum Number Of Files Exceeded", "The maximum allowed number of files is limited to " + QString::number(MaxNumOfFiles) + " files at a time.", "information");
        }
        else { //else we show messagebox indicating that user didn't choose files
            if (this->wipe) //if we're wiping
                this->showMessageBox("No Files Chosen Error", "Error, please choose files for wiping.", "critical");
            else { //else we encrypt/decrypt
                if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                    this->showMessageBox("No Files Chosen Error", "Error, please choose files for encryption.", "critical");
                else //else we show messagebox for decryption
                    this->showMessageBox("No Files Chosen Error", "Error, please choose files for decryption.", "critical");
            }
        }
    }
    else { //else we show messagebox indicating user cannot add files while wipe in process
        if (this->wipe) //if we're wiping
            this->showMessageBox("Wipe In Progress Error", "Error, cannot add files while wipe in progress.", "warning");
        else { //else we encrypt/decrypt
            if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                this->showMessageBox("Encryption In Progress Error", "Error, cannot add files while encryption in progress.", "warning");
            else //else we show messagebox for decryption
                this->showMessageBox("Decryption In Progress Error", "Error, cannot add files while decryption in progress.", "warning");
        }
    }
}


/**
 * @brief Method to clear the listView items.
 */
void CryptoShredder::clearContents() {
    if (this->fileHandler == NULL) { //means there's no wipe in progress
        this->filePathList.clear(); //clear filePathList
        this->fileDictionary.clear(); //clear file dictionary
        this->listViewFileDictionary.clear(); //clear listView file dictionary
        this->listViewCounter = 0; //set the listViewCounter back to zero
        this->fileCounter = 0; //set the fileCounter back to zero
        this->listViewModel->setStringList(QStringList()); //clear current listView items
    }
    else { //else we can't clear the screen
        if (this->wipe) //if we're wiping
            this->showMessageBox("Unable To Clear Screen", "Error, cannot clear the screen while wipe in progress.", "warning"); //we show messagebox with error
        else { //else we encrypt/decrypt
            if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                this->showMessageBox("Unable To Clear Screen", "Error, cannot clear the screen while encryption in progress.", "warning"); //we show messagebox with error
            else //else we show messagebox for decryption
                this->showMessageBox("Unable To Clear Screen", "Error, cannot clear the screen while decryption in progress.", "warning"); //we show messagebox with error
        }
    }
}


/**
 * @brief Method to stop the wiping process.
 */
void CryptoShredder::cancelProcess() {
    if (this->fileHandler) {
        QMessageBox::StandardButton choice;
        if (this->wipe) //if we're wiping
            choice = showMessageBox("Cancel Wipe", "Stopping the wiping process will result in irretrievable loss of data for the files currently undergoing wiping. Are you certain you want to proceed?", "question");
        else { //else we encrypt/decrypt
            if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                choice = showMessageBox("Cancel Encryption", "Stopping the encryption process will result in irretrievable loss of data for the files currently undergoing encryption. Are you certain you want to proceed?", "question");
            else //else we show messagebox for decryption
                choice = showMessageBox("Cancel Decryption", "Stopping the decryption process will result in irretrievable loss of data for the files currently undergoing decryption. Are you certain you want to proceed?", "question");
        }
        if (choice == QMessageBox::No) //if user chose "No" we do not cancel the wiping process
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
        string filePath = this->listViewFileDictionary[fileIndex]; //get file path from the listView file dictionary
        filesystem::path p(filePath); //call filesystem path method to get name of file
        QString fileName = QString::fromStdString(p.stem().string() + p.extension().string()); //get file name with filesystem
        this->fileViewer = FileViewer::getInstance(this, QString::fromStdString(filePath), (fileName.size() > 34) ? QString::fromStdString(p.stem().string().substr(0, 30) + "..." + p.extension().string()) : fileName); //create a new instance of fileViewer to show file's content to user
    }
    else { //else wipe in progress
        if (this->wipe) //if we're wiping
            this->showMessageBox("Unable To Open File", "Error, cannot open file while wipe in progress.", "warning"); //show messagebox with error indicating wipe in progress
        else { //else we encrypt/decrypt
            if (!ui.CipherCheckBox->isChecked()) //if true we show messagebox for encryption
                this->showMessageBox("Unable To Open File", "Error, cannot open file while encryption in progress.", "warning"); //show messagebox with error indicating wipe in progress
            else //else we show messagebox for decryption
                this->showMessageBox("Unable To Open File", "Error, cannot open file while decryption in progress.", "warning"); //show messagebox with error indicating wipe in progress
        }
    }
}


/**
 * @brief Method to check if key is valid AES key in GUI.
 */
void CryptoShredder::checkLineEditValidator() {
    QString text = ui.KeyLineEdit->text(); //get the text inside key line edit
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
        ui.KeyLineEdit->setStyleSheet(styleSheet); //set the style sheet with red border
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
        ui.KeyLineEdit->setStyleSheet(styleSheet); //set the style sheet with regular border
    }
}


/**
 * @brief Method to open the general information window to display information about FileShredder.
 */
void CryptoShredder::infoLabelClicked() {
    InfoWindow* info = InfoWindow::getInstance(this); //create new instance of InfoWindow
}


/**
 * @brief Method for handling operation modes selection in GUI.
 */
void CryptoShredder::optionsLabelClicked() {
    if (this->fileHandler) { //if true there's a process in progress
        this->showMessageBox("Unable To Change Mode", "Error, cannot change operation mode while current process in progress.", "warning"); //show error messagebox
    }
    else { //else we can change operation mode
        if (!this->wipe) { //if true we change to wipe mode
            this->optionsImageLabel->setNewImage("images/cipherIcon.png", "Encrypt/Decrypt files."); //set optionsImageLabel
            ui.TitleIconLabel->setPixmap(QPixmap("images/wipeIcon.png")); //set TitleIconLabel
            ui.KeyLabel->setVisible(false); //set KeyLabel to be invisible
            ui.KeyLineEdit->setVisible(false); //set KeyLineEdit to be invisible
            ui.CipherCheckBox->setVisible(false); //set CipherCheckBox to be invisible
            ui.CipherCheckBox->setChecked(false); //set CipherCheckBox to be invisible
            ui.KeyLineEdit->clear(); //clear KeyLineEdit
            ui.KeyLineEdit->clearFocus(); //clear KeyLineEdit focus
            ui.NumberOfPassesLabel->setVisible(true); //set NumberOfPassesLabel to be visible
            ui.PassesSpinBox->setVisible(true); //set PassesSpinBox to be visible
            ui.SpinBoxFrame->setVisible(true); //set SpinBoxFrame to be visible
            ui.RemoveFilesCheckBox->setVisible(true); //set RemoveFilesCheckBox to be visible
            ui.TopLabel->setText("Files Scheduled For Wipe"); //set TopLabel
            ui.ProcessButton->setText("Wipe Files"); //set ProcessButton
            ui.ChooseFilesButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Choose files, maximum 20 files.</span></p></body></html>");
            ui.ProcessButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Start secure wipe on selected files.</span></p></body></html>");
            this->clearContents(); //clear current contents
            this->wipe = true; //set wipe flag to indicate wipe mode 
        }
        else { //else we change to enryption/decryption mode
            this->optionsImageLabel->setNewImage("images/wipeIcon.png", "Wipe files.");
            ui.TitleIconLabel->setPixmap(QPixmap("images/cipherIcon.png"));
            ui.NumberOfPassesLabel->setVisible(false); //set NumberOfPassesLabel to be invisible
            ui.PassesSpinBox->setVisible(false); //set PassesSpinBox to be invisible
            ui.SpinBoxFrame->setVisible(false); //set SpinBoxFrame to be invisible
            ui.PassesSpinBox->setValue(1); //set PassesSpinBox value back to 1
            ui.RemoveFilesCheckBox->setVisible(false); //set RemoveFileCheckBox to be invisible
            ui.RemoveFilesCheckBox->setChecked(false);//set RemoveFileCheckBox to false 
            ui.KeyLabel->setVisible(true); //set KeyLabel to be visible
            ui.KeyLineEdit->setVisible(true); //set KeyLineEdit to be visible
            ui.CipherCheckBox->setVisible(true); //set CipherCheckBox to be visible
            this->cipherCheckBoxClicked(); //set GUI elements
            ui.ChooseFilesButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Choose files, maximum 10 files.</span></p></body></html>");
            this->clearContents(); //clear current contents
            this->wipe = false; //set wipe flag to indicate wipe mode isn't active
        }
    }
}


/**
 * @brief Method that handles GUI state when cipher check box state changes.
 */
void CryptoShredder::cipherCheckBoxClicked() {
    if (ui.CipherCheckBox->isChecked()) { //if true we set GUI to decryption mode
        ui.TopLabel->setText("Files Scheduled For Decryption"); //set TopLabel
        ui.ProcessButton->setText("Decrypt Files"); //set ProcessButton
        ui.ProcessButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Start decryption on selected files.</span></p></body></html>");
    }
    else { //else we set GUI to encryption mode
        ui.TopLabel->setText("Files Scheduled For Encryption"); //set TopLabel
        ui.ProcessButton->setText("Encrypt Files"); //set ProcessButton
        ui.ProcessButton->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>Start encryption on selected files.</span></p></body></html>");
    }
}