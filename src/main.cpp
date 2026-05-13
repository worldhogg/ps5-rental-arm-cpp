#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setStyle("Fusion"); // современный стиль Qt

    MainWindow window;
    window.show();

    return app.exec();
}
