#include "BenchmarkWindow.h"
#include "CalculateWindow.h"
#include "LauncherWindow.h"

#include <filesystem>
#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QFrame>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>

#include "conicity.h"
#include "contact.h"
#include "input.h"

namespace WRCC::UI {
    namespace {
        std::string resolvePath(const std::string &fn) {
            namespace fs = std::filesystem;
            const fs::path p(fn);
            if (fs::exists(p)) return fn;

            const fs::path parentP = fs::path("..") / p;
            if (fs::exists(parentP)) return parentP.string();

            static const fs::path appDir = [] {
                const QString appDirPath = QCoreApplication::applicationDirPath();
                return fs::path(appDirPath.toStdString());
            }();

            const fs::path appP = appDir / p;
            if (fs::exists(appP)) return appP.string();

            const fs::path appParentP = appDir / ".." / p;
            if (fs::exists(appParentP)) return appParentP.string();

            return fn;
        }
    } // anonymous namespace

    BenchmarkWindow::BenchmarkWindow(LauncherWindow *launcher, QWidget *parent)
        : QMainWindow(parent), m_launcher(launcher) {
        initUI();
        setupPresetCase(currentCase);
    }

    BenchmarkWindow::~BenchmarkWindow() = default;

    void BenchmarkWindow::closeEvent(QCloseEvent *event) {
        if (m_launcher) {
            m_launcher->show();
        }
        QMainWindow::closeEvent(event);
    }

    void BenchmarkWindow::initUI() {
        setWindowTitle("Railway applications \u2014 Equivalent conicity (EN 15302)");
        setMinimumSize(1000, 700);

        createMenuBar();

        auto *centralWidget = new QWidget(this);
        auto *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);

        auto *splitter = new QSplitter(Qt::Horizontal, centralWidget);

        // ==========================================
        // Left Control Panel (Scrollable)
        // ==========================================
        auto *scrollArea = new QScrollArea(splitter);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
        scrollArea->setMaximumWidth(360);
        scrollArea->setMinimumWidth(310);

        auto *leftPanel = new QWidget();
        auto *leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setContentsMargins(6, 6, 6, 6);
        leftLayout->setSpacing(8);

        // Title Card
        auto *headerCard = new QFrame(leftPanel);
        headerCard->setStyleSheet(
            "QFrame {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #bae6fd, stop:1 #99ccff);"
            "  border: 1px solid #7dd3fc;"
            "  border-radius: 10px;"
            "  padding: 8px;"
            "}"
        );
        auto *headerLayout = new QVBoxLayout(headerCard);
        headerLayout->setContentsMargins(4, 4, 4, 4);
        auto *titleLabel = new QLabel("EN15302:2008+A1:2010\n****** Testing Cases ******", headerCard);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 11pt; color: #0f172a; line-height: 1.3;");
        headerLayout->addWidget(titleLabel);
        leftLayout->addWidget(headerCard);

        // General Data Info Card
        auto *infoCard = new QFrame(leftPanel);
        infoCard->setStyleSheet(
            "QFrame {"
            "  background-color: #f1f5f9;"
            "  border: 1px solid #cbd5e1;"
            "  border-radius: 10px;"
            "  padding: 8px;"
            "}"
        );
        auto *infoLayout = new QVBoxLayout(infoCard);
        infoLayout->setContentsMargins(4, 4, 4, 4);
        auto *infoLabel = new QLabel(
            "Wheel profiles: R-UIC 519-A/B/H/I\n"
            "Rail profile: S-UIC 519-A\n"
            "Gauge: 1435.16 mm\n"
            "Wheels diameter: 920 mm",
            infoCard
        );
        infoLabel->setAlignment(Qt::AlignCenter);
        infoLabel->setStyleSheet("font-weight: bold; font-size: 9pt; color: #334155; line-height: 1.25;");
        infoLayout->addWidget(infoLabel);
        leftLayout->addWidget(infoCard);

        // Subtitle Section
        auto *subtitleLabel = new QLabel("Annex E \u2014 Calculation Reference", leftPanel);
        subtitleLabel->setAlignment(Qt::AlignCenter);
        subtitleLabel->setStyleSheet(
            "font-weight: bold; font-size: 9.5pt; color: #0284c7; "
            "background-color: #f0f9ff; border: 1px solid #bae6fd; "
            "border-radius: 6px; padding: 4px; margin-top: 2px; margin-bottom: 2px;"
        );
        leftLayout->addWidget(subtitleLabel);

        // Structured Cases List
        caseButtonGroup = new QButtonGroup(this);

        struct CaseInfo {
            QString wheelDesc;
            QString typeDesc;
            QString noteDesc;
            bool isAsymmetric;
        };

        const CaseInfo casesData[9] = {
            {"Wheel A / Rail A", "Symmetric case", "", false},
            {"Wheel B / Rail A", "Symmetric case", "", false},
            {"Wheel H / Rail A", "Symmetric case", "", false},
            {"Wheel I / Rail A", "Symmetric case", "", false},
            {"Wheel A / Rail A", "Asymmetric case", "Left WD: 918 mm", true},
            {"Wheel B / Rail A", "Asymmetric case", "Left WD: 918 mm", true},
            {"Wheel H / Rail A", "Asymmetric case", "Left WD: 918 mm", true},
            {"Wheel I / Rail A", "Asymmetric case", "Left WD: 918 mm", true},
            {"Wheel A(R) - B(L)", "Asymmetric case", "", true}
        };

        for (int i = 0; i < 9; ++i) {
            caseCards[i] = new QFrame(leftPanel);
            caseCards[i]->installEventFilter(this);

            auto *cardLayout = new QHBoxLayout(caseCards[i]);
            cardLayout->setContentsMargins(10, 6, 10, 6);
            cardLayout->setSpacing(8);

            // Left text description block
            auto *textLayout = new QVBoxLayout();
            textLayout->setContentsMargins(0, 0, 0, 0);
            textLayout->setSpacing(2);

            auto *wheelLabel = new QLabel(casesData[i].wheelDesc, caseCards[i]);
            wheelLabel->setStyleSheet("font-weight: bold; font-size: 9.5pt; color: #0f172a; background: transparent;");
            textLayout->addWidget(wheelLabel);

            auto *typeLabel = new QLabel(casesData[i].typeDesc, caseCards[i]);
            if (casesData[i].isAsymmetric) {
                typeLabel->setStyleSheet(
                    "font-size: 8.5pt; font-weight: 600; color: #b45309; background: transparent;");
            } else {
                typeLabel->setStyleSheet(
                    "font-size: 8.5pt; font-weight: 600; color: #0284c7; background: transparent;");
            }
            textLayout->addWidget(typeLabel);

            if (!casesData[i].noteDesc.isEmpty()) {
                auto *noteLabel = new QLabel(casesData[i].noteDesc, caseCards[i]);
                noteLabel->setStyleSheet(
                    "font-size: 8pt; color: #64748b; font-style: italic; background: transparent;");
                textLayout->addWidget(noteLabel);
            }

            cardLayout->addLayout(textLayout, 1);

            // Right Radio Button
            caseRadios[i] = new QRadioButton(QString("E%1").arg(i + 1), caseCards[i]);
            caseRadios[i]->setCursor(Qt::PointingHandCursor);
            caseButtonGroup->addButton(caseRadios[i], i + 1);
            cardLayout->addWidget(caseRadios[i], 0, Qt::AlignRight | Qt::AlignVCenter);

            leftLayout->addWidget(caseCards[i]);
        }

        caseRadios[currentCase - 1]->setChecked(true);
        updateCaseCardStyles(currentCase);
        connect(caseButtonGroup, &QButtonGroup::idClicked, this, &BenchmarkWindow::onCaseSelected);

        scrollArea->setWidget(leftPanel);

        // ==========================================
        // Right Chart Tabs
        // ==========================================
        tabWidget = new QTabWidget(splitter);

        eqconViewGroup = new CustomViewGroup(tabWidget);
        eqconView = eqconViewGroup->getView();
        tabWidget->addTab(eqconViewGroup->getGroupBox(), "Standard Benchmark Comparison (w/max & min tolerances)");

        deltaRViewGroup = new CustomViewGroup(tabWidget);
        deltaRView = deltaRViewGroup->getView();
        tabWidget->addTab(deltaRViewGroup->getGroupBox(), "Wheels' Rolling Radii Difference");

        splitter->addWidget(scrollArea);
        splitter->addWidget(tabWidget);
        splitter->setStretchFactor(0, 0);
        splitter->setStretchFactor(1, 1);

        mainLayout->addWidget(splitter);
        setCentralWidget(centralWidget);
    }

    void BenchmarkWindow::createMenuBar() {
        auto *menuBar = new QMenuBar(this);

        // File Menu
        auto *fileMenu = menuBar->addMenu(tr("&File"));
        auto *exitAction = new QAction(tr("E&xit"), this);
        connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
        fileMenu->addAction(exitAction);

        // View Menu
        auto *viewMenu = menuBar->addMenu(tr("&View"));
        auto *openCalcAction = new QAction(tr("Switch to &General Calculation..."), this);
        connect(openCalcAction, &QAction::triggered, this, &BenchmarkWindow::onOpenCalculateWindow);
        viewMenu->addAction(openCalcAction);

        auto *returnLauncherAction = new QAction(tr("Return to Main Launcher"), this);
        connect(returnLauncherAction, &QAction::triggered, this, &BenchmarkWindow::onReturnToLauncher);
        viewMenu->addAction(returnLauncherAction);

        // Help Menu
        auto *helpMenu = menuBar->addMenu(tr("&Help"));
        auto *aboutAction = new QAction(tr("About &Benchmark"), this);
        connect(aboutAction, &QAction::triggered, this, &BenchmarkWindow::showAbout);
        helpMenu->addAction(aboutAction);

        setMenuBar(menuBar);
    }

    bool BenchmarkWindow::eventFilter(QObject *watched, QEvent *event) {
        if (event->type() == QEvent::MouseButtonPress) {
            for (int i = 0; i < 9; ++i) {
                if (watched == caseCards[i]) {
                    caseRadios[i]->setChecked(true);
                    onCaseSelected(i + 1);
                    return true;
                }
            }
        }
        return QMainWindow::eventFilter(watched, event);
    }

    void BenchmarkWindow::updateCaseCardStyles(int activeId) const {
        for (int i = 0; i < 9; ++i) {
            if (!caseCards[i]) continue;
            if (i + 1 == activeId) {
                caseCards[i]->setStyleSheet(
                    "QFrame {"
                    "  background-color: #eff6ff;"
                    "  border: 2px solid #2563eb;"
                    "  border-radius: 8px;"
                    "}"
                );
                if (caseRadios[i]) {
                    caseRadios[i]->setStyleSheet(
                        "QRadioButton {"
                        "  font-size: 10pt;"
                        "  font-weight: bold;"
                        "  color: #1d4ed8;"
                        "  background: transparent;"
                        "}"
                        "QRadioButton::indicator {"
                        "  width: 15px;"
                        "  height: 15px;"
                        "  border-radius: 8px;"
                        "  border: 2px solid #1d4ed8;"
                        "  background-color: #2563eb;"
                        "}"
                    );
                }
            } else {
                caseCards[i]->setStyleSheet(
                    "QFrame {"
                    "  background-color: #ffffff;"
                    "  border: 1px solid #cbd5e1;"
                    "  border-radius: 8px;"
                    "}"
                );
                if (caseRadios[i]) {
                    caseRadios[i]->setStyleSheet(
                        "QRadioButton {"
                        "  font-size: 10pt;"
                        "  font-weight: bold;"
                        "  color: #334155;"
                        "  background: transparent;"
                        "}"
                        "QRadioButton::indicator {"
                        "  width: 15px;"
                        "  height: 15px;"
                        "  border-radius: 8px;"
                        "  border: 2px solid #94a3b8;"
                        "  background-color: #ffffff;"
                        "}"
                        "QRadioButton::indicator:hover {"
                        "  border: 2px solid #2563eb;"
                        "  background-color: #eff6ff;"
                        "}"
                    );
                }
            }
        }
    }

    void BenchmarkWindow::onCaseSelected(int id) {
        currentCase = id;
        updateCaseCardStyles(currentCase);
        setupPresetCase(currentCase);
    }

    void BenchmarkWindow::onOpenCalculateWindow() {
        this->hide();
        auto *calcWin = new CalculateWindow(m_launcher, nullptr);
        calcWin->setAttribute(Qt::WA_DeleteOnClose);
        calcWin->showMaximized();
    }

    void BenchmarkWindow::setupPresetCase(int caseIndex) {
        double min = -6.8, max = 6.8;
        switch (caseIndex) {
            case 1:
            case 2:
            case 3:
            case 5:
            case 9:
                min = -6.8;
                max = 6.8;
                break;
            case 4:
                min = -7.1;
                max = 7.1;
                break;
            case 6:
                min = -6.9;
                max = 6.8;
                break;
            case 7:
                min = -6.8;
                max = 6.7;
                break;
            case 8:
                min = -7.1;
                max = 7.0;
                break;
            default:
                min = -6.8;
                max = 6.8;
                break;
        }

        std::string wd = "data/RUIC519A.dat";
        std::string wi = "data/RUIC519A.dat";

        switch (caseIndex) {
            case 1:
            case 5:
                wd = "data/RUIC519A.dat";
                wi = "data/RUIC519A.dat";
                break;
            case 2:
            case 6:
                wd = "data/RUIC519B.dat";
                wi = "data/RUIC519B.dat";
                break;
            case 3:
            case 7:
                wd = "data/RUIC519H.dat";
                wi = "data/RUIC519H.dat";
                break;
            case 4:
            case 8:
                wd = "data/RUIC519I.dat";
                wi = "data/RUIC519I.dat";
                break;
            case 9:
                wd = "data/RUIC519A.dat";
                wi = "data/RUIC519B.dat";
                break;
            default:
                break;
        }

        double rr0 = 920.0;
        double rl0 = (caseIndex >= 5 && caseIndex <= 8) ? 918.0 : 920.0;
        bool isSymmetric = (caseIndex >= 1 && caseIndex <= 4);
        int c = (caseIndex >= 1 && caseIndex <= 4) ? 1 : -1;

        std::string benchFile = "data/dr_ce_E" + std::to_string(caseIndex) + "_bench.dat";
        std::string refDrFile = "data/dr_ce_E" + std::to_string(caseIndex) + ".dat";
        std::string railFile = "data/SUIC519A.dat";

        try {
            DataReader wheelRight(resolvePath(wd));
            DataReader wheelLeft(resolvePath(wi));
            DataReader rail(resolvePath(railFile));

            const auto &y1 = rail.getColumn(0);
            const auto &zp1 = rail.getColumn(1);
            const auto &y2 = wheelRight.getColumn(0);
            const auto &zp2d = wheelRight.getColumn(1);
            const auto &zp2i = wheelLeft.getColumn(1);

            constexpr double dy = 0.1, e = 1435.16, s = e * 0.5, ce = 0.0, cer = 0.025;

            auto min_ = static_cast<long double>(min), max_ = static_cast<long double>(max);
            const auto ys = Linspace(min_, max_, dy).linspacec();

            ConPon rail_sys(y1, y2, zp1, zp2d, zp2i, ce, s);
            for (double d_l: ys) {
                rail_sys.transformCoordinates(rr0, rl0, d_l, cer, s);
            }

            const auto &rrr_final = rail_sys.get_rrr();
            const auto &rrl_final = rail_sys.get_rrl();

            double dr0 = rr0 - rl0;
            int sign = (dr0 > 0) ? -1 : 1;
            std::vector<double> dr(rrr_final.size());
            for (size_t i = 0; i < rrr_final.size(); ++i) {
                dr[i] = (rrr_final[i] - rrl_final[i]) + sign * dr0 / 2.0;
            }

            std::vector<double> drf(dr.size());
            for (size_t i = 0; i < dr.size(); ++i) {
                drf[i] = (std::abs(dr[i]) < 1e-4) ? 0.0 : dr[i];
            }

            Output out(ys, drf, e, rr0, min, max, c);
            const auto &ycp = out.getYcp();
            const auto &ang = out.getAng();

            BenchmarkEqConPlot::applyPlot(eqconView, ycp, ang, benchFile, isSymmetric);
            BenchmarkDeltaRPlot::applyPlot(deltaRView, ys, drf, refDrFile);
        } catch (const std::exception &ex) {
            qWarning() << "Benchmark calculation error:" << ex.what();
            QMessageBox::critical(this, tr("Benchmark Error"),
                                  tr("An error occurred during benchmark calculation:\n%1").arg(ex.what()));
        }
    }

    void BenchmarkWindow::onReturnToLauncher() {
        this->hide();
        if (m_launcher) {
            m_launcher->show();
        }
    }

    void BenchmarkWindow::showAbout() {
        const QString aboutText =
                "<h2 style='color: lightblue; text-decoration: underline; text-align: center;'>"
                "Wheel / Rail Contact Characterization Program</h2>"

                "<p style='font-size: 14px; font-weight: bold; text-align: left;'>"
                "Description:</p>"

                "<p style='text-align: justify; text-indent: 85px;'>"
                "Railway application aimed to characterize the physical wheel-rail interaction "
                "by computing the full set of contact parameters.</p>"

                "<p style='font-size: 12px; font-style: italic; text-align: center;'>"
                "Version: 1.2-beta</p>"

                "<p style='font-size: 14px; font-weight: bold; text-align: left;'>"
                "Key Features:</p>"

                "<ul style='text-align: justify;'>"
                "<li>Potential wheel-rail contact points detection.</li>"
                "<li>Effective contact angle tan(γa) determination.</li>"
                "<li>Rolling-radius difference ∆r function calculation.</li>"
                "<li>Equivalent conicity f(λ) = tan(γc)  estimation.</li>"
                "</ul>"

                "<p style='text-align: right'>"
                "Developed by: Criogenox</p>";

        QMessageBox::about(this, "About", aboutText);
    }
} // namespace WRCC::UI
