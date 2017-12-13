#include "calc.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Calc *c = new Calc();
    c->show();

    return app.exec();
}
