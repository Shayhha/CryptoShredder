/********************************************************************************
** Form generated from reading UI file 'FileShredder.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILESHREDDER_H
#define UI_FILESHREDDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileShredderClass
{
public:
    QWidget *centralWidget;
    QListView *FileListView;
    QFrame *OptionsFrame;
    QCheckBox *RemoveFilesCheckBox;
    QPushButton *WipeButton;
    QPushButton *ChooseFilesButton;
    QPushButton *ClearScreenButton;
    QSpinBox *PassesSpinBox;
    QLabel *NumberOfPassesLabel;
    QFrame *SpinBoxFrame;
    QPushButton *CancelWipeButton;
    QProgressBar *progressBar;
    QFrame *titleFrame;
    QLabel *titleLabel;
    QLabel *shredderLabel;
    QLabel *FilesToBeWipedLabel;

    void setupUi(QMainWindow *FileShredderClass)
    {
        if (FileShredderClass->objectName().isEmpty())
            FileShredderClass->setObjectName(QString::fromUtf8("FileShredderClass"));
        FileShredderClass->resize(1078, 700);
        FileShredderClass->setMinimumSize(QSize(1078, 700));
        FileShredderClass->setMaximumSize(QSize(1078, 700));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(14);
        FileShredderClass->setFont(font);
        FileShredderClass->setStyleSheet(QString::fromUtf8("QWidget {\n"
"	background-color: rgba(68,70,84,255);\n"
"}\n"
"\n"
"QScrollBar:vertical {\n"
"    background-color: rgba(247, 247, 247,150);\n"
"    border: 1px solid rgb(153, 153, 153);\n"
"    width:10px;    \n"
"    margin: 0px 0px 0px 0px;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {          \n"
"    background-color: black;   \n"
"    min-height: 100px;\n"
"    border: 0px solid black;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical {       \n"
"    height: 0px;\n"
"    subcontrol-position: bottom;\n"
"    subcontrol-origin: margin;\n"
"}\n"
"\n"
"QScrollBar::sub-line:vertical {\n"
"    height: 0px;\n"
"    subcontrol-position: top;\n"
"    subcontrol-origin: margin;\n"
"}\n"
"\n"
"QListView::item:hover { \n"
"   background-color: rgb(173, 174, 184); \n"
"   color:black;\n"
"   border: 1px solid black;\n"
"}\n"
"\n"
"QListView::item:selected { \n"
"   background-color: rgb(187, 188, 196);\n"
"   color:black;\n"
"   border: 1px solid black;\n"
"}\n"
""));
        centralWidget = new QWidget(FileShredderClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        FileListView = new QListView(centralWidget);
        FileListView->setObjectName(QString::fromUtf8("FileListView"));
        FileListView->setGeometry(QRect(60, 120, 961, 371));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(16);
        FileListView->setFont(font1);
        FileListView->setFocusPolicy(Qt::NoFocus);
        FileListView->setStyleSheet(QString::fromUtf8("background-color: rgba(198, 198, 198, 0.6);\n"
"border-radius: 15px;\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-color: black;\n"
"padding: 4px;"));
        OptionsFrame = new QFrame(centralWidget);
        OptionsFrame->setObjectName(QString::fromUtf8("OptionsFrame"));
        OptionsFrame->setGeometry(QRect(60, 510, 961, 161));
        OptionsFrame->setStyleSheet(QString::fromUtf8("#OptionsFrame {\n"
"background-color: rgba(53,54,64,0.8);\n"
"border-radius: 15px;\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-radius: 15px;\n"
"border-color: black;\n"
"padding: 4px;\n"
"}\n"
"\n"
"\n"
""));
        OptionsFrame->setFrameShape(QFrame::StyledPanel);
        OptionsFrame->setFrameShadow(QFrame::Raised);
        RemoveFilesCheckBox = new QCheckBox(OptionsFrame);
        RemoveFilesCheckBox->setObjectName(QString::fromUtf8("RemoveFilesCheckBox"));
        RemoveFilesCheckBox->setGeometry(QRect(180, 30, 261, 31));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(14);
        font2.setBold(true);
        font2.setWeight(75);
        RemoveFilesCheckBox->setFont(font2);
        RemoveFilesCheckBox->setCursor(QCursor(Qt::PointingHandCursor));
        RemoveFilesCheckBox->setFocusPolicy(Qt::ClickFocus);
        RemoveFilesCheckBox->setStyleSheet(QString::fromUtf8("background-color: none;\n"
"color: white;\n"
"border: none"));
        WipeButton = new QPushButton(OptionsFrame);
        WipeButton->setObjectName(QString::fromUtf8("WipeButton"));
        WipeButton->setGeometry(QRect(135, 90, 141, 51));
        WipeButton->setMinimumSize(QSize(141, 51));
        WipeButton->setMaximumSize(QSize(141, 51));
        WipeButton->setFont(font2);
        WipeButton->setCursor(QCursor(Qt::PointingHandCursor));
        WipeButton->setFocusPolicy(Qt::ClickFocus);
        WipeButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(32,33,35,255);\n"
"	color: rgb(245,245,245);\n"
"	border-radius: 15px;\n"
"	border-style: outset;\n"
"	border-width: 2px;\n"
"	border-radius: 15px;\n"
"	border-color: black;\n"
"	padding: 4px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"   background-color: #575965;\n"
"	color: white;\n"
"    border-radius: 15px;\n"
"	border-style: outset;\n"
"	border-width: 2px;\n"
"	border-radius: 15px;\n"
"	border-color: black;\n"
"	padding: 4px;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"   background-color:#b1b9bb;\n"
"	color: white;\n"
"}\n"
""));
        ChooseFilesButton = new QPushButton(OptionsFrame);
        ChooseFilesButton->setObjectName(QString::fromUtf8("ChooseFilesButton"));
        ChooseFilesButton->setGeometry(QRect(500, 90, 141, 51));
        ChooseFilesButton->setMinimumSize(QSize(141, 51));
        ChooseFilesButton->setMaximumSize(QSize(141, 51));
        ChooseFilesButton->setFont(font2);
        ChooseFilesButton->setCursor(QCursor(Qt::PointingHandCursor));
        ChooseFilesButton->setFocusPolicy(Qt::ClickFocus);
        ChooseFilesButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(32,33,35,255);\n"
"	color: rgb(245,245,245);\n"
"	border-radius: 15px;\n"
"	border-style: outset;\n"
"	border-width: 2px;\n"
"	border-radius: 15px;\n"
"	border-color: black;\n"
"	padding: 4px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"   background-color: #575965;\n"
"	color: white;\n"
"    border-radius: 15px;\n"
"	border-style: outset;\n"
"	border-width: 2px;\n"
"	border-radius: 15px;\n"
"	border-color: black;\n"
"	padding: 4px;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"   background-color:#b1b9bb;\n"
"	color: white;\n"
"}\n"
"\n"
""));
        ClearScreenButton = new QPushButton(OptionsFrame);
        ClearScreenButton->setObjectName(QString::fromUtf8("ClearScreenButton"));
        ClearScreenButton->setGeometry(QRect(685, 90, 141, 51));
        ClearScreenButton->setMinimumSize(QSize(141, 51));
        ClearScreenButton->setMaximumSize(QSize(141, 51));
        ClearScreenButton->setFont(font2);
        ClearScreenButton->setCursor(QCursor(Qt::PointingHandCursor));
        ClearScreenButton->setFocusPolicy(Qt::ClickFocus);
        ClearScreenButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(32,33,35,255);\n"
"	color: rgb(245,245,245);\n"
"	border-radius: 15px;\n"
"	border-style: outset;\n"
"	border-width: 2px;\n"
"	border-radius: 15px;\n"
"	border-color: black;\n"
"	padding: 4px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"   background-color: #575965;\n"
"	color: white;\n"
"    border-radius: 15px;\n"
"	border-style: outset;\n"
"	border-width: 2px;\n"
"	border-radius: 15px;\n"
"	border-color: black;\n"
"	padding: 4px;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"   background-color:#b1b9bb;\n"
"	color: white;\n"
"}\n"
""));
        PassesSpinBox = new QSpinBox(OptionsFrame);
        PassesSpinBox->setObjectName(QString::fromUtf8("PassesSpinBox"));
        PassesSpinBox->setGeometry(QRect(720, 30, 61, 31));
        PassesSpinBox->setFont(font2);
        PassesSpinBox->setCursor(QCursor(Qt::PointingHandCursor));
        PassesSpinBox->setFocusPolicy(Qt::ClickFocus);
        PassesSpinBox->setStyleSheet(QString::fromUtf8("background-color: transparent;\n"
"color: rgb(245,245,245);\n"
"border-radius: 15px;\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-radius: 15px;\n"
"border-color: black;\n"
"padding: 4px;\n"
"selection-background-color: transparent;"));
        PassesSpinBox->setReadOnly(false);
        PassesSpinBox->setMinimum(1);
        PassesSpinBox->setMaximum(10);
        NumberOfPassesLabel = new QLabel(OptionsFrame);
        NumberOfPassesLabel->setObjectName(QString::fromUtf8("NumberOfPassesLabel"));
        NumberOfPassesLabel->setGeometry(QRect(540, 30, 171, 31));
        NumberOfPassesLabel->setFont(font2);
        NumberOfPassesLabel->setStyleSheet(QString::fromUtf8("background-color: none;\n"
"color: rgb(245,245,245);\n"
"border: none"));
        SpinBoxFrame = new QFrame(OptionsFrame);
        SpinBoxFrame->setObjectName(QString::fromUtf8("SpinBoxFrame"));
        SpinBoxFrame->setGeometry(QRect(720, 30, 61, 31));
        SpinBoxFrame->setStyleSheet(QString::fromUtf8("background-color: rgba(32,33,35,255);\n"
"border-radius: 15px;\n"
"border-width: 2px;\n"
"border-radius: 15px;\n"
"padding: 4px;"));
        SpinBoxFrame->setFrameShape(QFrame::StyledPanel);
        SpinBoxFrame->setFrameShadow(QFrame::Raised);
        CancelWipeButton = new QPushButton(OptionsFrame);
        CancelWipeButton->setObjectName(QString::fromUtf8("CancelWipeButton"));
        CancelWipeButton->setGeometry(QRect(318, 90, 141, 51));
        CancelWipeButton->setMinimumSize(QSize(141, 51));
        CancelWipeButton->setMaximumSize(QSize(141, 51));
        CancelWipeButton->setFont(font2);
        CancelWipeButton->setCursor(QCursor(Qt::PointingHandCursor));
        CancelWipeButton->setFocusPolicy(Qt::ClickFocus);
        CancelWipeButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(32,33,35,255);\n"
"	color: rgb(245,245,245);\n"
"	border-radius: 15px;\n"
"	border-style: outset;\n"
"	border-width: 2px;\n"
"	border-radius: 15px;\n"
"	border-color: black;\n"
"	padding: 4px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"   background-color: #575965;\n"
"	color: white;\n"
"    border-radius: 15px;\n"
"	border-style: outset;\n"
"	border-width: 2px;\n"
"	border-radius: 15px;\n"
"	border-color: black;\n"
"	padding: 4px;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"   background-color:#b1b9bb;\n"
"	color: white;\n"
"}\n"
""));
        SpinBoxFrame->raise();
        RemoveFilesCheckBox->raise();
        WipeButton->raise();
        ChooseFilesButton->raise();
        ClearScreenButton->raise();
        PassesSpinBox->raise();
        NumberOfPassesLabel->raise();
        CancelWipeButton->raise();
        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(70, 490, 941, 21));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Arial"));
        font3.setPointSize(10);
        progressBar->setFont(font3);
        progressBar->setStyleSheet(QString::fromUtf8("background-color: rgba(32,33,35,255);\n"
"color: rgb(245,245,245);\n"
"border-radius: 15px;\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-radius: 15px;\n"
"border-color: black;\n"
"padding: 4px;\n"
"text-align: center;"));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);
        titleFrame = new QFrame(centralWidget);
        titleFrame->setObjectName(QString::fromUtf8("titleFrame"));
        titleFrame->setGeometry(QRect(0, 0, 1081, 61));
        titleFrame->setStyleSheet(QString::fromUtf8("background-color: rgba(32,33,35,255);\n"
"color: white;"));
        titleFrame->setFrameShape(QFrame::StyledPanel);
        titleFrame->setFrameShadow(QFrame::Raised);
        titleLabel = new QLabel(titleFrame);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        titleLabel->setGeometry(QRect(455, 10, 191, 41));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Arial"));
        font4.setPointSize(22);
        font4.setBold(true);
        font4.setItalic(true);
        font4.setWeight(75);
        titleLabel->setFont(font4);
        titleLabel->setStyleSheet(QString::fromUtf8("color: rgb(245,245,245);"));
        shredderLabel = new QLabel(titleFrame);
        shredderLabel->setObjectName(QString::fromUtf8("shredderLabel"));
        shredderLabel->setGeometry(QRect(405, 10, 40, 40));
        QFont font5;
        font5.setFamily(QString::fromUtf8("Arial"));
        shredderLabel->setFont(font5);
        shredderLabel->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
        shredderLabel->setPixmap(QPixmap(QString::fromUtf8("images/shredderIcon.png")));
        FilesToBeWipedLabel = new QLabel(centralWidget);
        FilesToBeWipedLabel->setObjectName(QString::fromUtf8("FilesToBeWipedLabel"));
        FilesToBeWipedLabel->setGeometry(QRect(80, 80, 431, 31));
        QFont font6;
        font6.setFamily(QString::fromUtf8("Arial"));
        font6.setPointSize(18);
        font6.setBold(true);
        font6.setItalic(false);
        font6.setUnderline(false);
        font6.setWeight(75);
        FilesToBeWipedLabel->setFont(font6);
        FilesToBeWipedLabel->setStyleSheet(QString::fromUtf8("background-color: none;\n"
"color: rgb(245,245,245);\n"
""));
        FileShredderClass->setCentralWidget(centralWidget);

        retranslateUi(FileShredderClass);

        QMetaObject::connectSlotsByName(FileShredderClass);
    } // setupUi

    void retranslateUi(QMainWindow *FileShredderClass)
    {
        FileShredderClass->setWindowTitle(QCoreApplication::translate("FileShredderClass", "File Shredder", nullptr));
        RemoveFilesCheckBox->setText(QCoreApplication::translate("FileShredderClass", "Remove Files After Wipe", nullptr));
        WipeButton->setText(QCoreApplication::translate("FileShredderClass", "Wipe Files", nullptr));
#if QT_CONFIG(tooltip)
        ChooseFilesButton->setToolTip(QCoreApplication::translate("FileShredderClass", "<html><head/><body><p>Maximum 20 files for wipe.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        ChooseFilesButton->setText(QCoreApplication::translate("FileShredderClass", "Choose Files", nullptr));
        ClearScreenButton->setText(QCoreApplication::translate("FileShredderClass", "Clear Screen", nullptr));
#if QT_CONFIG(tooltip)
        PassesSpinBox->setToolTip(QCoreApplication::translate("FileShredderClass", "<html><head/><body><p>Maximum 10 passes.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        NumberOfPassesLabel->setText(QCoreApplication::translate("FileShredderClass", "Number Of Passes", nullptr));
        CancelWipeButton->setText(QCoreApplication::translate("FileShredderClass", "Cancel Wipe", nullptr));
        titleLabel->setText(QCoreApplication::translate("FileShredderClass", "File Shredder", nullptr));
        shredderLabel->setText(QString());
        FilesToBeWipedLabel->setText(QCoreApplication::translate("FileShredderClass", "Files Scheduled For Wipe", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FileShredderClass: public Ui_FileShredderClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILESHREDDER_H
