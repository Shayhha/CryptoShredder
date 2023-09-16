#include "FileShredder.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileShredder w;
    w.show();
    return a.exec();
}
