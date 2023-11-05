#ifndef _ImageLabel_H
#define _ImageLabel_H
#define _CRT_SECURE_NO_WARNINGS
#include <QtWidgets/QMainWindow>
#include <iostream>
#include <string.h>
#include <vector>
#include <QMouseEvent>
#include <QLabel>

using namespace std;

/// <summary>
/// Class for image label for GUI that supports click events
/// </summary>
class ImageLabel : public QLabel {
    Q_OBJECT

public:
    ImageLabel(const QString& imagePath, const QPoint& position, const QSize& size, QWidget* parent = nullptr) : QLabel(parent) {
        setPixmap(QPixmap(imagePath)); //set the icon file path 
        setGeometry(position.x(), position.y(), size.width(), size.height()); //set the size of icon and its position in GUI
        this->setStyleSheet("background: transparent;"); //make background transparent
    }

    void setNewImage(const QString& imagePath, const QString& tooltipText = "") {
        setPixmap(QPixmap(imagePath)); //set the new icon file path
        if (tooltipText != "")
            this->setToolTip(tooltipText);
    }

signals:
    void clicked(); //signal for clicked event

protected:
    /// <summary>
    /// Method for emitting clicked signal to GUI
    /// </summary>
    /// <param name="QMouseEvent event"></param>
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) //if left button clicked
            emit clicked(); //we emit the clicked signal
    }

    /// <summary>
    /// Method for setting cursor icon for label when hovering on it
    /// </summary>
    /// <param name="QEvent event"></param>
    void enterEvent(QEvent* event) override {
        Q_UNUSED(event);
        setCursor(Qt::PointingHandCursor); //set the cursor to pointing hand
    }

    /// <summary>
    /// Method for setting cursor icon back to default when leaving the icon
    /// </summary>
    /// <param name="event"></param>
    void leaveEvent(QEvent* event) override {
        Q_UNUSED(event);
        unsetCursor(); //set the cursor to default 
    }
};
#endif
