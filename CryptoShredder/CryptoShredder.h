#define _CRT_SECURE_NO_WARNINGS
#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <QLineEdit>
#include <QStringListModel>
#include <unordered_map>
#include "ui_CryptoShredder.h"
#include "FileHandler.h"
#include "FileViewer.h"
#include "ImageLabel.h"
#include "InfoWindow.h"

using namespace std;

/**
 * @brief Class that represents the main GUi application of CryptoShredder.
 */
class CryptoShredder : public QMainWindow {
    Q_OBJECT

private:
    Ui::CryptoShredder ui; //ui element for GUI
    static bool wipe; //flag for indication operation mode, if true we wipe, else we encrypt/decrypt
    static bool isClosing; //flag for indicating that program is about to close
    QStringListModel* listViewModel = NULL; //model for listView
    QRegExpValidator* keyValidator = NULL; //regular expression validator for KeyLineEdit
    FileHandler* fileHandler = NULL; //shredder object for wipe
    unordered_map<string, int> fileDictionary; //fileDictionary that represents each file with its corresponding index in listView
    unordered_map<int, string> listViewFileDictionary; //listViewDictionary represents the current files that are showing in FileListView
    vector<string> filePathList; //vector that represents all the files path
    SignalProxy* signal = NULL; //signal object for foreigner classes to communicate with GUI
    recursive_mutex GUIMutex; //mutex for thread-safe operations
    size_t listViewCounter = 0; //counter for number of items in listView
    size_t fileCounter = 0; //counter for number of files
    FileViewer* fileViewer = NULL; //FileViewer object for file viewer 
    ImageLabel* infoImageLabel = NULL; //ImageLabel for info icon
    ImageLabel* optionsImageLabel = NULL; //ImageLabel for options icon

public:
    CryptoShredder(QWidget* parent = nullptr);
    ~CryptoShredder();

private slots: //here we declare the slot methods
    void closeEvent(QCloseEvent* event) override;
    void processFiles();
    void cancelProcess();
    void openFileDialog();
    void clearContents();
    void addItemToListView(const QString& item);
    void updateListView(const QString& fileDictionaryName, const QString& fileName, const QString& tag);
    void checkThreads();
    void setListViewTags(const QString& tag, const QString& currentTag = NULL);
    void doubleClickedFile(const QModelIndex& index);
    void checkLineEditValidator();
    QMessageBox::StandardButton showMessageBox(const QString& title, const QString& text, const QString& type);
    void infoLabelClicked();
    void optionsLabelClicked();
    void cipherCheckBoxClicked();
};