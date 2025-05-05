#include "Mulberry.hpp"

#include <QApplication>

int main(int argc, char*argv[])
{
    QApplication app{ argc, argv };
    Mulberry xmulberry;
    xmulberry.show();
    return app.exec();
}
