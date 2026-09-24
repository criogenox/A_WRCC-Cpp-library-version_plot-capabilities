#pragma once

#include <QMainWindow>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>

#include "customviewgroup.h"
#include "BenchmarkPlots.h"

namespace WRCC::UI {
    class LauncherWindow;

    class BenchmarkWindow : public QMainWindow {
        Q_OBJECT

    public:
        explicit BenchmarkWindow(LauncherWindow *launcher = nullptr, QWidget *parent = nullptr);

        ~BenchmarkWindow() override;

        bool eventFilter(QObject *watched, QEvent *event) override;

    protected:
        void closeEvent(QCloseEvent *event) override;

    private slots:
        void onCaseSelected(int id);

        void onOpenCalculateWindow();

        void onReturnToLauncher();

        void showAbout();

    private:
        void initUI();

        void createMenuBar();

        void setupPresetCase(int caseIndex);

        LauncherWindow *m_launcher{nullptr};

        // Left panel widgets
        QButtonGroup *caseButtonGroup{nullptr};
        QRadioButton *caseRadios[9]{nullptr};
        QFrame *caseCards[9]{nullptr};

        void updateCaseCardStyles(int activeId) const;

        // Chart views & tabs
        QTabWidget *tabWidget{nullptr};
        CustomViewGroup *eqconViewGroup{nullptr};
        CustomViewGroup *deltaRViewGroup{nullptr};
        ZoomAndScroll *eqconView{nullptr};
        ZoomAndScroll *deltaRView{nullptr};

        int currentCase{6}; // Default to E6 (as in benchmark.png)
    };
} // namespace WRCC::UI
