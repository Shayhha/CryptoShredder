#ifndef _InfoWindow_H
#define _InfoWindow_H
#define _CRT_SECURE_NO_WARNINGS
#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <iostream>
#include <string.h>
#include <vector>
#include <QTextEdit>
#include <QFile>
#include "ui_InfoWindow.h"

using namespace std;

/// <summary>
/// Class for application general information
/// </summary>
class InfoWindow : public QDialog {
    Q_OBJECT

private:
    Ui::InfoWindow ui; //ui element for window
    InfoWindow(QWidget* parent); //constructor
    ~InfoWindow(); //destructor
    InfoWindow(const InfoWindow&) = delete; //prevent copy
    void operator=(const InfoWindow&) = delete; //prevent assignment
    static bool isInstance; //static bool instance flag for indication 

public:
    static InfoWindow* getInstance(QWidget* parent);
};
#endif

