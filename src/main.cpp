#include <QApplication>
#include <QMetaType>
#include <QtOpenGL>

#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont f=QApplication::font();
    f.setFamily("Helvetica");
    QApplication::setFont(f);
    
    
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);

    MainWindow w;
    w.show();

    return a.exec();
}
