#include "InfoWindow.h"


InfoWindow* InfoWindow::currentInstance = NULL; //initialize the static InfoWindow instance to null


/**
 * @brief Constructor of class.
 * @param QWidget* parent
 */
InfoWindow::InfoWindow(QWidget* parent) : QDialog(parent) {
    this->ui.setupUi(this); //set ui elements

    this->setWindowIcon(QIcon("images/CryptoShredder.ico")); //set window icon
    this->setAttribute(Qt::WA_DeleteOnClose); //ensure that object gets deleted when window closes
    connect(this->ui.OKButton, &QPushButton::clicked, this, &QDialog::accept); //connect OK button to close the window
    this->setModal(true); //set the dialog model to block interactions with main GUI
    this->show(); //show window
}


/**
 * @brief Destructor of class.
 */
InfoWindow::~InfoWindow() {
    if (InfoWindow::currentInstance) //if there's an instance of InfoWindow
        InfoWindow::currentInstance = NULL; //set InfoWindow instance back to null
}


/**
 * @brief Method for getting instance for singleton class.
 * @param QWidget* parent
 * @return InfoWindow* instance
 */
InfoWindow* InfoWindow::getInstance(QWidget* parent) {
    if (!InfoWindow::currentInstance) { //if true we can give the user an instance for InfoWindow
        InfoWindow::currentInstance = new InfoWindow(parent); //create a new instance of InfoWindow
    }
    else {
        InfoWindow::currentInstance->raise(); //bring the window to the front
        InfoWindow::currentInstance->activateWindow(); //activate the window
    }
    return InfoWindow::currentInstance; //return the instance
}