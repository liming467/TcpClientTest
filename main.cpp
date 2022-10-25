#include "TcpClientTest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClientTest w;
    w.show();
    return a.exec();
}
