#include <QApplication>
#include "Gui.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Gui window;
    window.resize(1200, 1200);
    window.show();
    return app.exec();
}
