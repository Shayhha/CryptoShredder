#define _CRT_SECURE_NO_WARNINGS
#include <QtWidgets/QMainWindow>
#include <QLocalServer>
#include <QLocalSocket>
#include <QStandardPaths>
#include <QFileDialog>
#include <QStringList>
#include <QLineEdit>
#include <QMouseEvent>
#include <QStringListModel>
#include <unordered_map>
#include "ui_CryptoShredder.h"
#include "CustomMessageBox.h"
#include "FileHandler.h"
#include "FileViewer.h"
#include "InfoWindow.h"

using namespace std;

/**
 * @brief Class that represents the main GUi application of CryptoShredder.
 */
class CryptoShredder : public QMainWindow {
    Q_OBJECT

private:
    Ui::CryptoShredder ui; //ui element for GUI
    static QLocalServer* server; //represents listening server for our app to make sure one instance is showing
    static QString serverName; //represents our listening server name
    static bool isWipe; //flag for indication operation mode, if true we wipe, else we encrypt/decrypt
    static bool isClosing; //flag for indicating that program is about to close
    FileViewer* fileViewer = NULL; //FileViewer object for file viewer
	InfoWindow* infoWindow = NULL; //InfoWindow object for info window
    QStringListModel* listViewModel = NULL; //model for listView
    QRegularExpressionValidator* keyValidator = NULL; //regular expression validator for KeyLineEdit
    FileHandler* fileHandler = NULL; //shredder object for wipe
    SignalProxy* signal = NULL; //signal object for foreigner classes to communicate with GUI
    unordered_map<u16string, size_t> fileDictionary; //fileDictionary that represents each file with its corresponding index in listView
    unordered_map<size_t, u16string> listViewFileDictionary; //listViewDictionary represents the current files that are showing in FileListView
    vector<File*> fileList; //vector that represents chosen files as objects
    recursive_mutex GUIMutex; //mutex for thread-safe operations
    size_t listViewCounter = 0; //counter for number of items in listView
    size_t fileCounter = 0; //counter for number of files

public:
    CryptoShredder(QWidget* parent = nullptr);
    ~CryptoShredder();
    static bool InitServer();
    static bool CheckServer();
    static void CloseServer();

private slots: //here we declare the slot methods
    void initUI();
    void closeEvent(QCloseEvent* event) override;
    void ProcessFiles();
    void CancelProcess();
    void OpenFileDialog();
    void ClearContents();
    void ChangeOperationMode();
    void ChangeCipherMode();
    void ShowInfoWindow();
    void AddItemToListView(const QString& item);
    void UpdateListView(const QString& fileFullName, const QString& fileName, const QString& tag);
    void CheckThreads();
    void SetListViewTags(const QString& tag, const QString& currentTag = NULL);
    void DoubleClickedFile(const QModelIndex& index);
    void CheckLineEditValidator();
};