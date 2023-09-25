#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <QStringListModel>
#include <unordered_map>
#include "ui_FileShredder.h"
#include "Shredder.h"

using namespace std;

class FileShredder : public QMainWindow {
    Q_OBJECT

private:
    Ui::FileShredderClass ui; //ui element for GUI
    QStringListModel* listViewModel = NULL; //model for listView
    Shredder* shredder = NULL; //shredder object for wipe
    unordered_map<string, int> fileDictionary; //fileDictionary that represents each file with its corresponding index in listView
    vector<string> filePathList; //vector that represents all the files path
    SignalProxy* signal = NULL; //signal object for foreigner classes to communicate with GUI
    recursive_mutex GUIMutex; //mutex for thread-safe operations
    size_t listViewCounter = 0; //counter for number of items in listView
    size_t fileCounter = 0; //counter for number of files

public:
    FileShredder(QWidget *parent = nullptr);
    ~FileShredder();

private slots: //here we declare the slot methods
    void wipeFiles();
    void cancelWipe();
    void openFileDialog();
    void clearContents();
    void addItemToListView(const QString& item);
    void updateListView(const QString& item, const QString& tag);
    void checkThreads();
    void setListViewTags(const QString& tag, const QString& currentTag = NULL);
    QMessageBox::StandardButton showMessageBox(const QString& title, const QString& text, const QString& type);
};
