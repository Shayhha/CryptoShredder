#include "FileShredder.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("images/fileShredder.ico")); //add icon
    FileShredder w;
    w.show();
    return a.exec();
}
