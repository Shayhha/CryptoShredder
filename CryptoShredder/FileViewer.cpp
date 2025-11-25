#include "FileViewer.h"


FileViewer* FileViewer::currentInstance = NULL; //initialize the static fileViewer instance to null


/**
 * @brief Constructor of class.
 * @param QWidget* parent
 * @param QString filePath
 * @param QString fileName
 */
FileViewer::FileViewer(QWidget* parent, const QString& filePath, const QString& fileName) : QDialog(parent) {
	QLoggingCategory::setFilterRules("qt.text.font.db=false"); //disable font database warnings
	this->ui.setupUi(this); //load the ui file of FileViewer
    this->initUI(filePath, fileName); //call init method
}


/**
 * @brief Destructor of class.
 */
FileViewer::~FileViewer() {
    if (FileViewer::currentInstance) //if there's an instance of FileViewer
		FileViewer::currentInstance = NULL; //set FileViewer instance back to null
}


/**
 * @brief Method for initializing the FileViewer elements and slots.
 * @param QString filePath
 * @param QString fileName
 */
void FileViewer::initUI(const QString& filePath, const QString& fileName) {
    this->setWindowTitle("File Viewer"); //set title of window
    this->setWindowIcon(QIcon("images/CryptoShredder.ico")); //set window icon
    this->file.setFileName(filePath); //set file name for QFile object
    this->ui.FileNameLabel->setText(QString(QChar(0x200E)) + fileName); //set the file name in FileViewer
    this->ui.FileTextEdit->viewport()->setCursor(Qt::ArrowCursor); //set cursor for FileTextEdit
    this->setAttribute(Qt::WA_DeleteOnClose); //ensure that object gets deleted when window closes
    this->setModal(true); //set the dialog model to block interactions with main GUI 

    //this will make the text inside textEdit from left to right regardless of language 
    //QTextDocument* doc = ui.FileTextEdit->document();
    //QTextOption textOption = doc->defaultTextOption();
    //textOption.setTextDirection(Qt::LeftToRight);
    //doc->setDefaultTextOption(textOption);
    //ui.FileTextEdit->setDocument(doc);

    connect(this->ui.FileTextEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &FileViewer::LoadChunk); //connect signal for adding text when scrolling
    connect(this->ui.FormatComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FileViewer::UpdateFileContent); //connect signal for combo box index changed event

    if (this->OpenFile(filePath)) //if true we show the window
        this->show(); //show window
    else //else we couldn't open the file so we delete object
        this->deleteLater(); //ensure that allocated memory gets deleted 
}


/**
 * @brief Method for getting instance for singleton class.
 * @param QWidget* parent
 * @param QString filePath
 * @param QString fileName
 * @return FileViewer* instance
 */
FileViewer* FileViewer::GetInstance(QWidget* parent, const QString& filePath, const QString& fileName) {
	if (!FileViewer::currentInstance) { //if true we can give the user an instance for FileViewer
        FileViewer::currentInstance = new FileViewer(parent, filePath, fileName); //create a new instance of FileViewer
    }
    else {
        FileViewer::currentInstance->raise(); //bring the window to the front
        FileViewer::currentInstance->activateWindow(); //activate the window
    }
	return FileViewer::currentInstance; //return the instance
}


/**
 * @brief Method for opening a file, return true if opened file successfully.
 * @param QString filePath
 * @return bool result
 */
bool FileViewer::OpenFile(const QString& filePath) {
    if (this->file.open(QIODevice::ReadOnly)) { //if true we opened the file successfully
        this->fileSize = this->file.size(); //set size file 
        this->bytesRead = 0; //set starting bytesRead to zero
        this->LoadChunk(); //load the initial chunk
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
    this->ui.FileTextEdit->verticalScrollBar()->setValue(this->ui.FileTextEdit->verticalScrollBar()->minimum()); //set the scrollbar value to the minimum position
}


/**
 * @brief Method for updating file text edit content with chosen format.
 * @param int index
 */
void FileViewer::UpdateFileContent(int index) {
    string oldFormat = this->format; //save old format for checking later if it has been changed
    if(index == 0)  //read and display the file as HEX
        this->format = "HEX";
    else if(index == 1) //read and display the file as BINARY
        this->format = "BINARY";
    else if (index == 2) //read and display the file as UTF-8
        this->format = "UTF-8";
    if (oldFormat != this->format) { //means that format was changed
        //disconnect the scrollbar signal before changing format
        disconnect(this->ui.FileTextEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &FileViewer::LoadChunk);
        //we reset the file's reading progress to the beginning
        this->bytesRead = 0; //set bytesRead back to zero
        this->ui.FileTextEdit->clear(); //clear the FileTextEdit
        this->file.seek(0);  //reset file pointer to beginning
        this->LoadChunk();  //load the initial chunk again with current format
		this->ui.FileTextEdit->verticalScrollBar()->setValue(this->ui.FileTextEdit->verticalScrollBar()->minimum()); //set the scroll bar back to the beginning
        //reconnect the scrollbar signal after changing format
        connect(this->ui.FileTextEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &FileViewer::LoadChunk);
    }
}


/**
 * @brief Method to load chunks of data to the FileTextEdit when scroll bar emits signal for value change.
 */
void FileViewer::LoadChunk() {
    const qint64 DefaultChunkSize = 2048 * 3;  //setting DefaultChuckSize that will add each time

    if (!this->file.isOpen() || this->bytesRead >= this->fileSize) { //if true we loaded all the file contents
        return;  //no more data to load, finish the task
    }

    qint64 remainingBytes = this->fileSize - this->bytesRead; //set the remaining bytes parameter
    qint64 chunkSize = qMin(DefaultChunkSize, remainingBytes); //we set chunkSize to be the minimum between remaining bytes and a default parameter

    //read and append data only if the scrollbar position is near the bottom
    QScrollBar* scrollbar = this->ui.FileTextEdit->verticalScrollBar(); //get a pointer to the scroll bar
    int triggerPoint = scrollbar->maximum() - 5 * scrollbar->pageStep(); //set the trigger point for scroll bar for the signal for next chunk to be loaded

    if (scrollbar->value() >= triggerPoint) { //if true we need to load more data to FileTextEdit
        QByteArray buffer = this->file.read(chunkSize); //read the chunkSize bytes from file and store it in a byte array
        QString data = ""; //represents the data of buffer 
        if (this->format == "HEX") //if format is HEX
            data = buffer.toHex(' ').toUpper(); //we add data as HEX 
        else if (this->format == "UTF-8") //else if format is UTF-8
            data = QString::fromUtf8(buffer); //add the data as UTF-8 
        else if (this->format == "BINARY") { //else format is BINARY
            vector<QString> binaryData(buffer.size()); //set the binary data array with buffer size
            //we use transform method to generate byte format for each value in buffer using lambda function
			transform(buffer.begin(), buffer.end(), binaryData.begin(), [](char c) { return QString::number((unsigned char)c, 2).rightJustified(8, '0'); }); //convert each byte to its binary representation
            QStringList binaryDataList = QStringList::fromVector(QVector<QString>(binaryData.begin(), binaryData.end())); //we create QStringList for saving the binary data 
            data = binaryDataList.join(" ").trimmed(); //join the strings with spaces and remove the trailing space
        }
        //disconnect the scrollbar signal before appending text
        disconnect(scrollbar, &QScrollBar::valueChanged, this, &FileViewer::LoadChunk);
        this->ui.FileTextEdit->append(data); //append the data to FileTextEdit
        this->bytesRead += chunkSize; //add the appended amount of data to our bytesRead parameter
        //reconnect the scrollbar signal after appending text
        connect(scrollbar, &QScrollBar::valueChanged, this, &FileViewer::LoadChunk);
    }
}