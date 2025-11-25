#ifndef _InfoWindow_H
#define _InfoWindow_H
#define _CRT_SECURE_NO_WARNINGS
#include <QtWidgets/QMainWindow>
#include <iostream>
#include <string>
#include <vector>
#include <QTextEdit>
#include <QFile>
#include "ui_InfoWindow.h"

using namespace std;

/**
 * @brief Class for application general information.
 */
class InfoWindow : public QDialog {
    Q_OBJECT

private:
    Ui::InfoWindow ui; //ui element for window
	static InfoWindow* currentInstance; //InfoWindow object for info window
    InfoWindow(QWidget* parent); //constructor
    ~InfoWindow(); //destructor
    InfoWindow(const InfoWindow&) = delete; //prevent copy
	void initUI(); //method for initializing the InfoWindow elements and slots
    void operator=(const InfoWindow&) = delete; //prevent assignment

public:
    static InfoWindow* GetInstance(QWidget* parent);
};
#endif