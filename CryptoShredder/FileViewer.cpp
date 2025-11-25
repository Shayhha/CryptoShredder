#include "FileViewer.h"


bool FileViewer::isInstance = false; //initialize the static instance flag 


/**
 * @brief Constructor of class.
 * @param QWidget* parent
 * @param QString filePath
 * @param QString fileName
 */
FileViewer::FileViewer(QWidget* parent, const QString& filePath, const QString& fileName) : QDialog(parent) {
    ui.setupUi(this); //set ui elements
    this->file.setFileName(filePath); //set file name for QFile object
    ui.FileNameLabel->setText(QString(QChar(0x200E)) + fileName); //set the file name in FileViewer
    //set the tooltip for the window
    this->setWhatsThis("This window serves as a viewer for the selected file in one of three formats: HEX, BINARY, or UTF-8. You can choose the format using the dropdown menu. The viewer allows you to inspect the file's contents in the selected format.");
    ui.FileTextEdit->viewport()->setCursor(Qt::ArrowCursor); //set cursor for FileTextEdit
    this->setAttribute(Qt::WA_DeleteOnClose); //ensure that object gets deleted when window closes
    this->setModal(true); //set the dialog model to block interactions with main GUI 
    
    //this will make the text inside textEdit from left to right regardless of language 
    //QTextDocument* doc = ui.FileTextEdit->document();
    //QTextOption textOption = doc->defaultTextOption();
    //textOption.setTextDirection(Qt::LeftToRight);
    //doc->setDefaultTextOption(textOption);
    //ui.FileTextEdit->setDocument(doc);
    
    connect(ui.FileTextEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &FileViewer::loadChunk); //connect signal for adding text when scrolling
    connect(ui.FormatComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FileViewer::updateFileContent); //connect signal for combo box index changed event

    if (this->openFile(filePath)) { //if true we show the window
        this->show(); //show window
    }
    else { //else we couldn't open the file so we show error messagebox and delete object
        QMessageBox::critical(nullptr, "Error Opening File", "Failed to open the file. This could be due to the file not existing or being securely deleted as part of the wiping process."); //show messagebox with error
        this->deleteLater(); //ensure that allocated memory gets deleted 
    }
}


/**
 * @brief Destructor of class.
 */
FileViewer::~FileViewer() {
    if (isInstance)
        isInstance = false; //ensure that we return the instance flag back to false when object deleted
}


/**
 * @brief Method for getting instance for singleton class.
 * @param QWidget* parent
 * @param QString filePath
 * @param QString fileName
 * @return FileViewer* viewer
 */
FileViewer* FileViewer::getInstance(QWidget* parent, const QString& filePath, const QString& fileName) {
    if (!isInstance) { //if true we can give the user an instance for FileViewer
        isInstance = true; //indicating that we're creating an instance for class
        return new FileViewer(parent, filePath, fileName); //return the new instance
    }
    return NULL; //else we return NULL indicating that there's a FileViewer already open
}


/**
 * @brief Method for opening a file, return true if opened file successfully.
 * @param QString filePath
 * @return FileViewer* viewer
 */
bool FileViewer::openFile(const QString& filePath) {
    if (this->file.open(QIODevice::ReadOnly)) { //if true we opened the file successfully
        this->fileSize = this->file.size(); //set size file 
        this->bytesRead = 0; //set starting bytesRead to zero
        this->loadChunk();  //load the initial chunk
        return true; //return true indicating for successful file opening
    }
    else //else we couldn't open file
        return false; //we indicate that we failed opening file
}


/**
 * @brief Method to set the position of scroll bar to the start position when show() method is called.
 * @param QShowEvent event
 */
void FileViewer::showEvent(QShowEvent* event) {
    QDialog::showEvent(event); //call the base class implementation
    ui.FileTextEdit->verticalScrollBar()->setValue(ui.FileTextEdit->verticalScrollBar()->minimum()); //set the scrollbar value to the minimum position
}


/**
 * @brief Method for updating file text edit content with chosen format.
 * @param int index
 */
void FileViewer::updateFileContent(int index) {
    string oldFormat = this->format; //save old format for checking later if it has been changed
    if(index == 0)  //read and display the file as HEX
        this->format = "HEX";
    else if(index == 1) //read and display the file as BINARY
        this->format = "BINARY";
    else if (index == 2) //read and display the file as UTF-8
        this->format = "UTF-8";
    if (oldFormat != this->format) { //means that format was changed
        //disconnect the scrollbar signal before changing format
        disconnect(ui.FileTextEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &FileViewer::loadChunk);
        //we reset the file's reading progress to the beginning
        this->bytesRead = 0; //set bytesRead back to zero
        ui.FileTextEdit->clear(); //clear the FileTextEdit
        this->file.seek(0);  //reset file pointer to beginning
        this->loadChunk();  //load the initial chunk again with current format
        ui.FileTextEdit->verticalScrollBar()->setValue(ui.FileTextEdit->verticalScrollBar()->minimum()); //ser the scroll bar back to the beginning
        //reconnect the scrollbar signal after changing format
        connect(ui.FileTextEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &FileViewer::loadChunk);
    }
}


/**
 * @brief Method to load chunks of data to the FileTextEdit when scroll bar emits signal for value change.
 */
void FileViewer::loadChunk() {
    const qint64 DefaultChunkSize = 2048 * 3;  //setting DefaultChuckSize that will add each time

    if (!this->file.isOpen() || this->bytesRead >= this->fileSize) { //if true we loaded all the file contents
        return;  //no more data to load, finish the task
    }

    qint64 remainingBytes = this->fileSize - this->bytesRead; //set the remaining bytes parameter
    qint64 chunkSize = qMin(DefaultChunkSize, remainingBytes); //we set chunkSize to be the minimum between remaining bytes and a default parameter

    //read and append data only if the scrollbar position is near the bottom
    QScrollBar* scrollbar = ui.FileTextEdit->verticalScrollBar(); //get a pointer to the scroll bar
    int triggerPoint = scrollbar->maximum() - 5 * scrollbar->pageStep(); //set the trigger point for scroll bar for the signal for next chunk to be loaded

    if (scrollbar->value() >= triggerPoint) { //if true we need to load more data to FileTextEdit
        QByteArray buffer = this->file.read(chunkSize); //read the chunkSize bytes from file and store it in a byte array
        QString data = ""; //represents the data of buffer 
        if (this->format == "HEX") //if format is HEX
            data = buffer.toHex(' ').toUpper(); //we add data as HEX 
        else if (this->format == "UTF-8") //else if format is UTF-8
            data = QString::fromStdString(buffer.toStdString()); //add the data as UTF-8 
        else if (this->format == "BINARY") { //else format is BINARY
            vector<QString> binaryData(buffer.size()); //set the binary data array with buffer size
            //we use transform method to generate byte format for each value in buffer using lambda function
            transform(buffer.begin(), buffer.end(), binaryData.begin(), [](char c) { 
                return QString::number(static_cast<unsigned char>(c), 2).rightJustified(8, '0'); });
            QStringList binaryDataList = QStringList::fromVector(QVector<QString>::fromStdVector(binaryData)); //we creat a QStringList for saving the binary data 
            data = binaryDataList.join(" ").trimmed();  //join the strings with spaces and remove the trailing space
        }
        //disconnect the scrollbar signal before appending text
        disconnect(scrollbar, &QScrollBar::valueChanged, this, &FileViewer::loadChunk);
        ui.FileTextEdit->append(data); //append the data to FileTextEdit
        this->bytesRead += chunkSize; // add the appended amount of data to our bytesRead parameter
        //reconnect the scrollbar signal after appending text
        connect(scrollbar, &QScrollBar::valueChanged, this, &FileViewer::loadChunk);
    }
}