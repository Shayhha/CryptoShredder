#include "CryptoShredder.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
    //check if listening server is running
    if (CryptoShredder::CheckServer()) {
        cout << "Another instance is already running." << endl;
        return 0;
    }

    //initalize listening server for application
    if (!CryptoShredder::InitServer()) {
        cout << "Failed to initialize listening server." << endl;
        return 1;
    }

    //start CryptoShredder application
    QApplication app(argc, argv);
    CryptoShredder cryptoShredder;
    cryptoShredder.show();

    //execute application and return execution code
    int ret = app.exec();
    cout << "Exiting." << endl;
    return ret;
}