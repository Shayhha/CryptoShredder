#include "FileShredder.h"


/// <summary>
/// Constructor of GUI
/// </summary>
/// <param name="QWidget parent"></param>
FileShredder::FileShredder(QWidget *parent) : QMainWindow(parent) {
    //setup ui elements
    ui.setupUi(this); 

    //FileListView model initialization//
    this->listViewModel = new QStringListModel(this); //initialize the model for listView in GUI
    this->signal = new SignalProxy(); //initialize the signal proxy for the foreigner classes
    ui.FileListView->setModel(listViewModel); //set the list model for listView in GUI to add elements
    ui.FileListView->setEditTriggers(QAbstractItemView::NoEditTriggers); //set the listView to be uneditable

    //slots for ui elements//
    connect(ui.WipeButton, &QPushButton::clicked, this, &FileShredder::wipeFiles);
    connect(ui.CancelWipeButton, &QPushButton::clicked, this, &FileShredder::cancelWipe);
    connect(ui.ChooseFilesButton, &QPushButton::clicked, this, &FileShredder::openFileDialog);
    connect(ui.ClearScreenButton, &QPushButton::clicked, this, &FileShredder::clearContents);
    connect(this->signal, &SignalProxy::signalUpdateListView, this, &FileShredder::updateListView);
    connect(this->signal, &SignalProxy::signalMessageBox, this, &FileShredder::showMessageBox);
    connect(this->signal, &SignalProxy::signalSetListViewTags, this, &FileShredder::setListViewTags);
}


/// <summary>
/// Destructor of GUI
/// </summary>
FileShredder::~FileShredder() {
    if (this->shredder) //if there's an object of shredder initialized
        delete this->shredder; //delete the shredder object
    delete this->listViewModel; //delete the list view model
    delete this->signal; //delete the signal object
}


/// <summary>
/// Method for adding elements to the FileListView in GUI
/// </summary>
/// <param name="QString item"></param>
void FileShredder::addItemToListView(const QString& item) {
    QStringList itemList = this->listViewModel->stringList(); //get current item list in the listView
    itemList.append(item); //add the item to item list 
    this->listViewModel->setStringList(itemList); //add item list to the GUI FileListView
}


/// <summary>
/// Method to update the listView when thread finishes its work
/// </summary>
/// <param name="QString item"></param>
void FileShredder::updateListView(const QString& item, const QString& tag) {
    this->GUIMutex.lock(); //lock the GUI mutex to prevent race conditions with threads
    if (this->shredder) { //update only if shredder object is initialized
        int fileIndex = this->fileDictionary[item.toStdString()]; //get the index of the file we need to update in fileDictionary
        QModelIndex index = this->listViewModel->index(fileIndex, 0); //convert the integer index to QModelIndex for Qt list model
        this->listViewModel->setData(index, item + tag, Qt::EditRole); //set the tag in the specific file to indicate that its finished
        this->checkThreads(); //call checkThreads method to determine if we can delete allocated memory
    }
    this->GUIMutex.unlock(); //unlock mutex
}


/// <summary>
/// Method to check if wiping task has finished so we can delete unnecessary memory
/// </summary>
void FileShredder::checkThreads() {
    this->GUIMutex.lock();  //lock the GUI mutex to prevent race conditions with threads
    if (this->shredder) { //check only if shredder object is initialized
        this->shredder->setNumOfThreads(this->shredder->getNumOfThreads() - 1); //we decrease the amount of runnning threads with getter and setter methods
        int incrementAmount = 100 / this->filePathList.size(); //increment the progress bar with an amount that represents the division of num of files
        ui.progressBar->setValue(ui.progressBar->value() + incrementAmount); //increment progress bar
        if (this->shredder->getNumOfThreads() == 0) { //means that all threads finished their work 
            this->shredder->setThreadsRunning(false); //set threadsRunning flag to false indicating all threads finished work
            ui.progressBar->setValue(100); //we set the progress bar to 100 when we finish the wipe
        }
        if (!(this->shredder->areThreadsRunning())) { //if true the wipe finished so we clear the necessary elements
            this->filePathList.clear(); //clear filePathList
            this->fileDictionary.clear(); //clear file dictionary
            delete this->shredder; //delete shredder object
            this->shredder = NULL; //set pointer of shredder to NULL for next wipe
            if (File::getIsCanceled()) { //if true user canceled the wipe
                File::setIsCanceled(false); //set the isCanceled flag back to false
                this->showMessageBox("Wipe Canceled", "Secure wiping for selected files has been canceled, files that were in process were canceled.", "information"); //show messagebox
            }
            else //else we finished the wipe
                this->showMessageBox("Wipe Finished", "Secure wiping for selected files completed successfully.", "information"); //show messagebox with success message
            ui.progressBar->setTextVisible(false); //set the progressBar text to false not showing the text
            ui.progressBar->setValue(0); //set the progress bar back to zero
        }
    }
    this->GUIMutex.unlock(); //unlock mutex
}


/// <summary>
/// Method to set tags for files in the listView in GUI
/// </summary>
/// <param name="QString tag"></param>
/// <param name="QString currentTag"></param>
void FileShredder::setListViewTags(const QString& tag, const QString& currentTag) {
    QStringList items = this->listViewModel->stringList(); //get item list from the listView model
    if (!items.isEmpty()) { //check if the listView is not empty
        if (currentTag == NULL) { //if NULL we need to add the tags to items that in the fileDictionary
            for (const auto& [fileName, index] : this->fileDictionary) { //iterate over fileDictionary items
                items[index] += tag; //add the tag to the items
            }
        }
        else { //if not NULL we need to add the tag to all items
            for (int i = 0; i < items.size(); i++) { //iterate over listView items
                int lastPosition = items[i].lastIndexOf(currentTag); //get the last index position of the file name without wiped tag
                if (lastPosition != -1) //if true we found the position we need to add new tag from
                    items[i] = items[i].left(lastPosition) + tag; //add the new tag to the file name
            }
        }
        this->listViewModel->setStringList(items); //set the list with updated tags
    }
}


/// <summary>
/// Method we use to show messagebox with given parameters, also for signals with proxy object
/// </summary>
/// <param name="QString title"></param>
/// <param name="QString text"></param>
/// <param name="QString type"></param>
QMessageBox::StandardButton FileShredder::showMessageBox(const QString& title, const QString& text, const QString& type) {
    if (type == "critical")
        return QMessageBox::critical(nullptr, title, text);
    else if (type == "warning")
        return QMessageBox::warning(nullptr, title, text);
    else if (type == "information")
        return QMessageBox::information(nullptr, title, text);
    else if (type == "question")
        return QMessageBox::question(nullptr, title, text);
    /*else if (type == "NoIcon")
        return QMessageBox::NoIcon(nullptr, title, text);*/
}


/// <summary>
/// Method for initiating file wipe 
/// </summary>
void FileShredder::wipeFiles() {
    if (this->shredder == NULL && !(this->fileDictionary.empty())) { //if true we can start the wipe
        QMessageBox::StandardButton choice = showMessageBox("Starting File Wiping", "Please be aware that all data in the selected files will be permanently erased and cannot be recovered. Are you sure you want to proceed?", "question");
        if (choice == QMessageBox::No) //if user chose "No" we do not start the wiping process
            return; //finish the method's work
        int numOfPasses = ui.PassesSpinBox->value(); //get number of passes from GUI
        bool toRemove = ui.RemoveFilesCheckBox->isChecked(); //check if user wants to delete files after wipe
        this->shredder = new Shredder(this->filePathList, numOfPasses, toRemove, this->signal); //initialize the shredder with the required parameters
        if (this->shredder) //check if we successfully initialized the shredder
            ui.progressBar->setTextVisible(true); //set the progressBar text to be visible
            this->setListViewTags(" - Processing.."); //set tags to the items in listView indicating that wiping started
            this->shredder->initShred(); //start the shredder and create the threads
    }
    else if (this->shredder == NULL && this->fileDictionary.empty()){ //if user didn't choose files we show messagebox with error
        this->showMessageBox("No Files Chosen Error", "Error, Please choose files before starting wipe.", "critical");
    }
    else if (this->shredder != NULL) { //if there's a wipe in progress we show messagebox with error
        this->showMessageBox("Wipe In Progress Error", "Error, Please wait for current wipe to finish.", "warning");
    }
    else { //else unknown error occurred
        this->showMessageBox("Unknown Error", "Unknown error occurred, please try again.", "critical");
    }
}


/// <summary>
/// Method to open the file dialog for choosing files for wipe
/// </summary>
void FileShredder::openFileDialog() {
    if (this->shredder == NULL) {
        QString desktopPath = QDir::homePath() + "/Desktop"; //we set a QString to point to the desktop directory 
        //set the file dialog with appropriate settings for our wiping
        QStringList selectedFiles = QFileDialog::getOpenFileNames(nullptr, "Select Files", desktopPath, "All Files (*);;Text Files (*.txt);;Image Files (*.png *.jpg)");

        // Check if files were selected
        if (!selectedFiles.isEmpty()) {
            this->setListViewTags(" - Finished", " - Wiped Successfully"); //set previous files tags to finished 
            bool isMaxSize = false; //flag for indicating files exceeds maximum size allowed
            const qint64 maxSize = 600LL * 1024LL * 1024LL;; // set max size to 600MB
            for (const QString& filePath : selectedFiles) { //we process the selected file paths
                QFileInfo fileInfo(filePath); //get info about the file
                if (fileInfo.size() <= maxSize) { //if file is below maximum size we proccess it
                    string FilePath = filePath.toStdString(); //save file path in a string variable
                    filesystem::path p(FilePath); //call filesystem path method to get name of file
                    QString fileName = QString::fromStdString(p.stem().string() + p.extension().string()); //save name of file in QString variable for GUI
                    auto dictIterator = this->fileDictionary.find(fileName.toStdString()); //we check if the file already exists inn our file dictionary
                    if (dictIterator == this->fileDictionary.end()) { //if true and our dictIterator points to the end of dictionary it means the file is not in dictionary
                        this->addItemToListView(fileName); //add the file name to our FileListView in GUI
                        this->fileDictionary[fileName.toStdString()] = this->listViewCounter; //add the file name as the key and the counter representing its index in the listView as value
                        this->filePathList.push_back(filePath.toStdString()); //add each file path to out filePathList
                        this->listViewCounter++; //increase counter
                    }
                }
                else //else file is exceeding maximum size
                    isMaxSize = true; //set isMaxSize flag to true
            }
            if(isMaxSize) //if isMaxSize flag is set we show messagebox that notifies the user for a file that exceeded allowed file size
                this->showMessageBox("Max File Size", "Files larger then 600MB detected, files that exceed the limit were not added.", "information");
        }
        else { //else we show messagebox indicating that user didn't choose files
            this->showMessageBox("No Files Chosen Error", "Error, please choose files for wiping.", "critical");
        }
    }
    else
        this->showMessageBox("Wipe In Progress Error", "Error, cannot add files while wipe in progress.", "warning");
}


/// <summary>
/// Method to clear the listView items
/// </summary>
void FileShredder::clearContents() {
    if (this->shredder == NULL) { //means there's no wipe in progress
        this->filePathList.clear(); //clear filePathList
        this->fileDictionary.clear(); //clear file dictionary
        this->listViewCounter = 0; //set the listViewCounter back to zero
        this->listViewModel->setStringList(QStringList()); //clear current listView items
    }
    else { //else we can't clear the screen
        this->showMessageBox("Unable To Clear Screen", "Error, cannot clear the screen while wipe in progress.", "warning"); //we show messagebox with error
    }
}


 //<summary>
 //Method to stop the wiping process
 //</summary>
void FileShredder::cancelWipe() {
    if (this->shredder) {
        QMessageBox::StandardButton choice = showMessageBox("Cancel Wipe", "Stopping the wiping process will result in irretrievable loss of data for the files currently undergoing wiping. Are you certain you want to proceed?", "question");
        if (choice == QMessageBox::No) //if user chose "No" we do not cancel the wiping process
            return; //finish the method's work
        this->shredder->cancelWipe(); //else we call cancelWipe method to indicate for cancelation
    }
}