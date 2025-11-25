#include "CustomMessageBox.h"


bool CustomMessageBox::isMessageBox = false; //initialize flag for indicating if messagebox already exists


/**
 * @brief Constructor of custom message box class.
 * @param QString title
 * @param QString message
 * @param QString iconType
 * @param int width
 * @param int height
 * @param bool wordWrap
 * @param QWidget* parent
 */
CustomMessageBox::CustomMessageBox(const QString& title, const QString& message, const QString& iconType, int width, int height, bool wordWrap, QWidget* parent) : QDialog(parent) {
	// set window properties
    setWindowTitle(title); //set the title for message box
    setObjectName("customMessageBox"); //set object name for message box
    setWindowIcon(QIcon("images/CryptoShredder.ico")); //set the icon for message box

    //create the main vertical layout
    QVBoxLayout* layout = new QVBoxLayout(this);

    //create a horizontal layout for the icon and message
    QHBoxLayout* horizontalLayout = new QHBoxLayout();

    //we add an icon only if iconType is not "NoIcon"
    if (iconType != "NoIcon") { //if true it means we need to set an icon for message box
        QLabel* iconLabel = new QLabel();
        QIcon icon = GetMessageBoxIcon(iconType); //use the method to get message box icon

        //create pixmap for icon and set size and margin
        QPixmap pixmap = icon.pixmap(48, 48);
        iconLabel->setPixmap(pixmap);
        iconLabel->setContentsMargins(15, 0, 15, 0);
        iconLabel->setAlignment(Qt::AlignCenter); //center the icon vertically
        horizontalLayout->addWidget(iconLabel); //add icon to horizontal layout
    }

    //set the message
    QLabel* messageLabel = new QLabel(message);
    messageLabel->setWordWrap(wordWrap); //ensure long messages wrap properly
    messageLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter); //vertically center the text
    messageLabel->setOpenExternalLinks(true); //open links in an external web browser
    messageLabel->setContentsMargins(0, 0, 0, 0);
    messageLabel->setMinimumWidth(250);

    //add message to the horizontal layout
    horizontalLayout->addWidget(messageLabel);
    horizontalLayout->setAlignment(Qt::AlignCenter); //center the entire horizontalLayout

    //add stretchable space around the horizontalLayout to center it vertically in the dialog
    layout->addStretch(1); //add stretch before the content
    layout->addLayout(horizontalLayout);
    layout->addStretch(1); //add stretch before the content

    //create buttons layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter); //center the buttons

    //if question message box, we show "Yes" and "No" buttons
	if (iconType == "Question") {
        QPushButton* yesButton = new QPushButton("Yes");
        yesButton->setCursor(Qt::PointingHandCursor);
        connect(yesButton, &QPushButton::clicked, this, &QDialog::accept);

        QPushButton* noButton = new QPushButton("No");
        noButton->setCursor(Qt::PointingHandCursor);
        connect(noButton, &QPushButton::clicked, this, &QDialog::reject);

        buttonLayout->addWidget(yesButton);
        buttonLayout->addSpacing(15);
        buttonLayout->addWidget(noButton);
    }
    //else we show "OK" button
	else {
        QPushButton* okButton = new QPushButton("OK");
        okButton->setCursor(Qt::PointingHandCursor);
        connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
        buttonLayout->addWidget(okButton);
    }

    //apply layout to the dialog
    layout->addLayout(buttonLayout);
    setLayout(layout);
    
    //set custom stylesheet
    this->setStyleSheet(
        "#customMessageBox {"
        "    background-color: rgb(245, 245, 245);"
        "}"
        ""
        "#customMessageBox QLabel {"
        "    color: black;"
        "    font-family: Arial;"
        "    font-size: 18px;"
        "}"
        ""
        "#customMessageBox QPushButton {"
        "    background-color: rgba(32, 33, 35, 255);"
        "    color: rgb(245, 245, 245);"
        "    border: 2px solid black;"
        "    border-radius: 15px;"
        "    padding: 4px;"
        "    font-family: Arial;"
        "    font-size: 17px;"
        "    font-weight: bold;"
        "    min-width: 60px;"
        "    min-height: 20px;"
        "}"
        ""
        "#customMessageBox QPushButton:hover {"
        "    background-color: rgb(87, 89, 101);"
        "}"
        ""
        "#customMessageBox QPushButton:pressed {"
        "    background-color: rgb(177, 185, 187);"
        "}"
    );

    //set dialog properties
    setMinimumSize(width, height); //set a reasonable minimum size
    adjustSize(); //adjust the size based on content
    setFixedSize(size()); //lock the size to prevent resizing
}


/**
 * @brief Method for overriting the original accept function and setting isMessageBox flag.
 */
void CustomMessageBox::accept() {
	CustomMessageBox::isMessageBox = false;
	QDialog::accept();
}


/**
 * @brief Method for overriting the original reject function and setting isMessageBox flag.
 */
void CustomMessageBox::reject() {
	CustomMessageBox::isMessageBox = false;
    QDialog::reject();
}


/**
 * @brief Method for mapping the iconType to the appropriate StandardPixmap icon.
 * @param QString iconType
 * @return QIcon icon
 */
QIcon CustomMessageBox::GetMessageBoxIcon(const QString& iconType) {
	if (iconType == "Warning")
        return QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning);
	else if (iconType == "Critical")
        return QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical);
	else if (iconType == "Question")
        return QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion);
    return QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation);
}


/**
 * @brief Function for showing message box window.
 * @param QString title
 * @param QString message
 * @param QString iconType
 * @param int width
 * @param int height
 * @param bool wordWrap
 * @param QWidget* parent
 * @return bool result
 */
bool CustomMessageBox::ShowMessageBox(const QString& title, const QString& message, const QString& iconType, int width, int height, bool wordWrap, QWidget* parent) {
    //iconType options can be Information, Warning, Critical, Question, NoIcon
	if (!CustomMessageBox::isMessageBox) { //check if there's no message box already showing
        CustomMessageBox messageBox(title, message, iconType, width, height, wordWrap, parent);

        //set isMessageBox and show message box
		CustomMessageBox::isMessageBox = true;
		int result = messageBox.exec();

        //return result value for question message box, else true
		return (iconType == "Question") ? result == QDialog::Accepted : true;
    }
	return false; //if there's already a message box showing we return false
}