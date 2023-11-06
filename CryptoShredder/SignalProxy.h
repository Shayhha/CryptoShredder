#ifndef _SignalProxy_H
#define _SignalProxy_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <vector>
#include <QObject.h>

using namespace std;

/// <summary>
/// Class that we use to connect foreigner classes to main GUI class 
/// with signal and slots that serves as a proxy between them
/// </summary>
class SignalProxy : public QObject {
	Q_OBJECT

signals: //here we declare the signals for GUI
	void signalUpdateListView(const QString& fileDictionaryName, const QString& fileName, const QString& tag); //signal to update GUI listView 
	void signalMessageBox(const QString& title, const QString& text, const QString& type); //signal to show GUI messagebox
	void signalSetListViewTags(const QString& tag, const QString& currentTag); //signal for set tags in GUI listView

public:
	SignalProxy() {}; //ctor
	~SignalProxy() {}; //dtor

public slots: //here we declare the slot methods
	/// <summary>
	/// Method to emit a signal to GUI to update listView
	/// </summary>
	/// <param name="string item"></param>
	void sendSignalUpdateListView(const string& fileDictionaryName, const string& fileName, const string& tag) {
		emit signalUpdateListView(QString::fromStdString(fileDictionaryName), QString::fromStdString(fileName), QString::fromStdString(tag));
	}

	/// <summary>
	/// Method to emit signal to GUI to show messagebox
	/// </summary>
	/// <param name="string title"></param>
	/// <param name="string text"></param>
	/// <param name="string type"></param>
	void sendSignalMessageBox(const string& title, const string& text, const string& type) {
		emit signalMessageBox(QString::fromStdString(title), QString::fromStdString(text), QString::fromStdString(type));
	}

	/// <summary>
	/// Method to emit signal to GUI to set tags in listView
	/// </summary>
	/// <param name="string tag"></param>
	/// <param name="string currentTag"></param>
	void sendSignalSetListViewTags(const string& tag, const string& currentTag) {
		emit signalSetListViewTags(QString::fromStdString(tag), QString::fromStdString(currentTag));
	}
};
#endif