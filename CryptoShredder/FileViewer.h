#ifndef _FileViewer_H
#define _FileViewer_H
#define _CRT_SECURE_NO_WARNINGS
#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QScrollBar>
#include <iostream>
#include <string>
#include <vector>
#include <QTextEdit>
#include <QFile>
#include "ui_FileViewer.h"  

using namespace std;

/**
 * @brief Class for file viewing in various formats.
 */
class FileViewer : public QDialog {
    Q_OBJECT

private:
    Ui::FileViewer ui; //ui element for window
    QFile file; //QFile object for opening file
    qint64 fileSize = 0; //qint64 for fileSize
    qint64 bytesRead = 0; //qint64 for amount of bytes read
    string format = "HEX"; //format string for text
    FileViewer(QWidget* parent, const QString& filePath, const QString& fileName); //private constructor for class
    ~FileViewer(); //destructor for class
    FileViewer(const FileViewer&) = delete; //prevent copy
    void operator=(const FileViewer&) = delete; //prevent assignment
    static bool isInstance; //static bool instance flag for indication 

public:
    static FileViewer* getInstance(QWidget* parent, const QString& filePath, const QString& fileName);
    bool openFile(const QString& filePath);
    void showEvent(QShowEvent* event) override;
    void updateFileContent(int index);
    void loadChunk();
};
#endif 