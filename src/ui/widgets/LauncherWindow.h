#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace WRCC::UI {
    class BenchmarkWindow;
    class CalculateWindow;

    class LauncherWindow : public QWidget {
        Q_OBJECT

    public:
        explicit LauncherWindow(QWidget *parent = nullptr);

        ~LauncherWindow() override;

    private slots:
        void openBenchmark();

        void openCalculate();

        void showInstructions();

        void showAbout();

    private:
        void initUI();

        QPushButton *instructionsButton{nullptr};
        QPushButton *benchmarkButton{nullptr};
        QPushButton *calculateButton{nullptr};
        QPushButton *aboutButton{nullptr};
        QPushButton *exitButton{nullptr};

        QPointer<BenchmarkWindow> benchmarkWindow;
        QPointer<CalculateWindow> calculateWindow;
    };
} // namespace WRCC::UI
