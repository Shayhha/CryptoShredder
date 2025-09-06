#ifndef _SignalProxy_H
#define _SignalProxy_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <QObject.h>

using namespace std;

/**
 * @brief Class that we use to connect foreigner classes to main GUI class
 * @brief with signal and slots that serves as a proxy between them.
 */
class SignalProxy : public QObject {
	Q_OBJECT

signals: //here we declare the signals for GUI
	void signalUpdateListView(const QString& fileFullName, const QString& fileName, const QString& tag); //signal to update GUI listView 
	void signalMessageBox(const QString& title, const QString& message, const QString& type); //signal to show GUI messagebox
	void signalSetListViewTags(const QString& tag, const QString& currentTag); //signal for set tags in GUI listView

public:
	SignalProxy() {}; //ctor
	~SignalProxy() {}; //dtor

public slots: //here we declare the slot methods
	/**
	 * @brief Method to emit a signal to GUI to update listView.
	 * @param u16string fileFullName
	 * @param u16string fileName
	 * @param string tag
	 */
	void sendSignalUpdateListView(const u16string& fileFullName, const u16string& fileName, const string& tag) {
		emit signalUpdateListView(QString::fromStdU16String(fileFullName), QString::fromStdU16String(fileName), QString::fromStdString(tag));
	}

	/**
	 * @brief Method to emit signal to GUI to show messagebox.
	 * @param string title
	 * @param string message
	 * @param string type
	 */
	void sendSignalMessageBox(const string& title, const string& message, const string& type) {
		emit signalMessageBox(QString::fromStdString(title), QString::fromStdString(message), QString::fromStdString(type));
	}

	/**
	 * @brief Method to emit signal to GUI to set tags in listView.
	 * @param string tag
	 * @param string currentTag
	 */
	void sendSignalSetListViewTags(const string& tag, const string& currentTag) {
		emit signalSetListViewTags(QString::fromStdString(tag), QString::fromStdString(currentTag));
	}
};
#endif