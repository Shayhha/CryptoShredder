#include "CryptoShredder.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("images/CryptoShredder.ico")); //add icon
    CryptoShredder w;
    w.show();
    return a.exec();
}
