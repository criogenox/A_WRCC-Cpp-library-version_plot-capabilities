#pragma once

#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>

#include "CalculatePlots.h"
#include "customviewgroup.h"
#include "wrcc/services/ReportExporter.h"

namespace WRCC::UI {
    class LauncherWindow;
    class BenchmarkWindow;

    class CalculateWindow : public QMainWindow {
        Q_OBJECT

    public:
        explicit CalculateWindow(LauncherWindow *launcher = nullptr, QWidget *parent = nullptr);

        ~CalculateWindow() override;

        bool performCalculation(QString *errorMessage = nullptr);

        bool exportReportToMarkdown(const QString &filePath, const QString &operatingConditions = "",
                                    QString *errorMessage = nullptr) const;

        bool exportReportToHtml(const QString &filePath, const QString &operatingConditions = "",
                                QString *errorMessage = nullptr) const;

        bool eventFilter(QObject *obj, QEvent *event) override;

    protected:
        void closeEvent(QCloseEvent *event) override;

    private slots:
        void onCalculate();

        void onCheckData();

        void onSaveHtml();

        void onSaveMarkdown();

        void onBrowseRightWheel();

        void onBrowseRightRail();

        void onBrowseLeftWheel();

        void onBrowseLeftRail();

        void onAsymmetricToggled(bool checked) const;

        void onOpenBenchmarkWindow();

        void onReturnToLauncher();

        void showAbout();

    private:
        void initUI();

        void createMenuBar();

        void setupDefaultParameters() const;

        void setupEventFilters(QWidget *widget);

        LauncherWindow *m_launcher{nullptr};

        QWidget *leftPanel{nullptr};
        QAction *togglePanelAction{nullptr};

        // Parameter inputs
        QDoubleSpinBox *rightWheelRadiusSpin{nullptr};
        QDoubleSpinBox *leftWheelRadiusSpin{nullptr};
        QCheckBox *asymmetricCheckbox{nullptr};
        QDoubleSpinBox *rightRailInclinSpin{nullptr};
        QDoubleSpinBox *leftRailInclinSpin{nullptr};
        QDoubleSpinBox *backToBackSpin{nullptr};
        QDoubleSpinBox *trackGaugeSpin{nullptr};

        // Profile file selectors
        QLineEdit *rightWheelPathEdit{nullptr};
        QLineEdit *rightRailPathEdit{nullptr};
        QLineEdit *leftWheelPathEdit{nullptr};
        QLineEdit *leftRailPathEdit{nullptr};

        QPushButton *calculateButton{nullptr};
        QPushButton *checkDataButton{nullptr};
        QPushButton *saveHtmlButton{nullptr};
        QPushButton *saveMarkdownButton{nullptr};

        // Tabs & Chart Views
        QTabWidget *tabWidget{nullptr};
        CustomViewGroup *rightPairViewGroup{nullptr};
        CustomViewGroup *leftPairViewGroup{nullptr};
        CustomViewGroup *eqconViewGroup{nullptr};
        CustomViewGroup *deltaRViewGroup{nullptr};
        CustomViewGroup *angleViewGroup{nullptr};

        ZoomAndScroll *rightPairView{nullptr};
        ZoomAndScroll *leftPairView{nullptr};
        ZoomAndScroll *eqconView{nullptr};
        ZoomAndScroll *deltaRView{nullptr};
        ZoomAndScroll *angleView{nullptr};

        // Last calculation state for report generation
        bool m_hasCalculated{false};
        Core::CalculationParameters m_lastParams;
        std::vector<double> m_lastYcp;
        std::vector<double> m_lastAng;
    };
} // namespace WRCC::UI
