#include "LibWorkspace.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Windows"));
    QApplication app(argc, argv);
    LibWorkspace window;
    window.setWindowTitle(QStringLiteral(u"Редактор библиотек"));
    window.show();
    window.resize(1200, 700);
    return app.exec();
}
