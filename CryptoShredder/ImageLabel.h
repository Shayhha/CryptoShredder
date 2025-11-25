#ifndef _ImageLabel_H
#define _ImageLabel_H
#define _CRT_SECURE_NO_WARNINGS
#include <QtWidgets/QMainWindow>
#include <iostream>
#include <string>
#include <vector>
#include <QMouseEvent>
#include <QLabel>

using namespace std;

/**
 * @brief Class for image label for GUI that supports click events.
 */
class ImageLabel : public QLabel {
    Q_OBJECT

public:
    ImageLabel(const QString& imagePath, const QPoint& position, const QSize& size, QWidget* parent = nullptr) : QLabel(parent) {
        setPixmap(QPixmap(imagePath)); //set the icon file path 
        setGeometry(position.x(), position.y(), size.width(), size.height()); //set the size of icon and its position in GUI
        this->setStyleSheet("QLabel { background: none; }"); //make background transparent
    }

    void setNewImage(const QString& imagePath, const QString& tooltipText = "") {
        setPixmap(QPixmap(imagePath)); //set the new icon file path
        if (tooltipText != "")
            this->setToolTip("<html><head/><body><p><span style='font-size:10pt;'>" + tooltipText + "</span></p></body></html>");
    }

signals:
    void clicked(); //signal for clicked event

protected:
    /**
     * @brief Method for emitting clicked signal to GUI.
     * @param QMouseEvent event
     */
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) //if left button clicked.
            emit clicked(); //we emit the clicked signal
    }

    /**
     * @brief Method for setting cursor icon for label when hovering on it.
     * @param QEvent event
     */
    void enterEvent(QEvent* event) override {
        Q_UNUSED(event);
        setCursor(Qt::PointingHandCursor); //set the cursor to pointing hand
    }

    /**
     * @brief Method for setting cursor icon back to default when leaving the icon.
     * @param QEvent event
     */
    void leaveEvent(QEvent* event) override {
        Q_UNUSED(event);
        unsetCursor(); //set the cursor to default 
    }
};
#endif