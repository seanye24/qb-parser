#include <QApplication>
#include "window.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // create container window
    Window window;
    window.show();

    return app.exec();
}



