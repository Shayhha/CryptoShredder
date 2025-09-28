#include "InfoWindow.h"


bool InfoWindow::isInstance = false; //initialize the static isInstance flag 


/**
 * @brief Constructor of class.
 * @param QWidget* parent
 */
InfoWindow::InfoWindow(QWidget* parent) : QDialog(parent) {
    ui.setupUi(this); //set ui elements
    this->setAttribute(Qt::WA_DeleteOnClose); //ensure that object gets deleted when window closes
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); //remove hint from QDialog
    connect(ui.OKButton, &QPushButton::clicked, this, &QDialog::accept); //connect OK button to close the window
    this->setModal(true); //set the dialog model to block interactions with main GUI 
    this->show(); //show window
}


/**
 * @brief Destructor of class.
 * @param QWidget* parent
 */
InfoWindow::~InfoWindow() {
    if (isInstance) //if true we set back isInstance flag to false
        isInstance = false; //set the isInstance flag to false
}


/**
 * @brief Method for getting instance for singleton class.
 * @param QWidget* parent
 */
InfoWindow* InfoWindow::getInstance(QWidget* parent) {
    if (!isInstance) { //if true we can give the user an instance for InfoWindow
        isInstance = true; //indicating that we're creating an instance for class
        return new InfoWindow(parent); //return the new instance
    }
    return NULL; //else we return NULL indicating that there's a InfoWindow already open
}