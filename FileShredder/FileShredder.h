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
    Ui::FileShredderClass ui;
    QStringListModel* listViewModel = NULL;
    Shredder* shredder = NULL;
    unordered_map<string, int> fileDictionary;
    vector<string> filePathList;
    SignalProxy* signal = NULL;
    recursive_mutex GUIMutex;
    int listViewCounter = 0;

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
