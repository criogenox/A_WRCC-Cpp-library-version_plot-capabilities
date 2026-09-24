#include <iostream>

#include <QApplication>

#include "CalculateWindow.h"
#include "LauncherWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "--generate-figures") {
            WRCC::UI::CalculateWindow calcWin;
            QString err;
            if (calcWin.performCalculation(&err)) {
                calcWin.exportReportToMarkdown("docs/test_report.md", "Standard Analysis Case", &err);
                calcWin.exportReportToHtml("docs/test_report.html", "Standard Analysis Case", &err);
                std::cout << "Successfully generated calculation figures into docs/figures/\n";
                return 0;
            } else {
                std::cerr << "Calculation error: " << err.toStdString() << "\n";
                return 1;
            }
        }
    }

    WRCC::UI::LauncherWindow launcher;
    launcher.show();

    return QApplication::exec();
}
