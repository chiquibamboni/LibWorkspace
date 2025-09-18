#include "LibWorkspace.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    //QApplication::setStyle(QStyleFactory::create("Windows"));
    QApplication app(argc, argv);
    LibWorkspace window;
    window.setWindowTitle(QStringLiteral(u"�������� ���������"));
    window.show();
    window.resize(600, 700);
    return app.exec();
}
