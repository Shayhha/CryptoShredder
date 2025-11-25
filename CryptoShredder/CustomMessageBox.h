#ifndef _CustomMessageBox_H
#define _CustomMessageBox_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QApplication>
#include <QIcon>
#include <QCursor>

/**
 * @brief Class for custom message box.
 */
class CustomMessageBox : public QDialog {
    Q_OBJECT

private:
	static bool isMessageBox; //represents flag for indicating if messagebox already exists
	CustomMessageBox(const QString& title, const QString& message, const QString& iconType = "Information", int width = 400, int height = 150, bool wordWrap = true, QWidget* parent = nullptr); //constructor
	~CustomMessageBox() override = default; //destructor
	QIcon GetMessageBoxIcon(const QString& iconType); //method for mapping the iconType to the appropriate StandardPixmap icon

protected:
	void accept() override; //override accept method
	void reject() override; //override reject method

public:
	static bool ShowMessageBox(const QString& title, const QString& message, const QString& iconType = "Information", int width = 400, int height = 150, bool wordWrap = true, QWidget* parent = nullptr); //function for showing message box window
};
#endif