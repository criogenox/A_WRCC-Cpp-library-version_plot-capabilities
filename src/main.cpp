#include <iostream>
#include <QApplication>

#include "LauncherWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WRCC::UI::LauncherWindow launcher;
    launcher.show();

    return QApplication::exec();
}
