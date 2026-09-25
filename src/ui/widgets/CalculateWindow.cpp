#include "CalculateWindow.h"
#include "BenchmarkWindow.h"
#include "LauncherWindow.h"

#include <filesystem>
#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>

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

    CalculateWindow::CalculateWindow(LauncherWindow *launcher, QWidget *parent)
        : QMainWindow(parent), m_launcher(launcher) {
        initUI();
        setupDefaultParameters();
    }

    CalculateWindow::~CalculateWindow() = default;

    void CalculateWindow::closeEvent(QCloseEvent *event) {
        if (m_launcher) {
            m_launcher->show();
        }
        QMainWindow::closeEvent(event);
    }

    bool CalculateWindow::eventFilter(QObject *obj, QEvent *event) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            if (!qobject_cast<QLineEdit *>(obj) && !qobject_cast<QAbstractSpinBox *>(obj)) {
                if (leftPanel && leftPanel->isVisible()) {
                    leftPanel->hide();
                    if (togglePanelAction) {
                        togglePanelAction->setChecked(false);
                    }
                    return true;
                }
            }
        }
        return QMainWindow::eventFilter(obj, event);
    }

    void CalculateWindow::setupEventFilters(QWidget *widget) {
        if (!widget) return;
        widget->installEventFilter(this);
        for (auto *child: widget->findChildren<QWidget *>()) {
            child->installEventFilter(this);
        }
    }

    void CalculateWindow::initUI() {
        setWindowTitle("Railway applications \u2014 Wheel / Rail Contact Characterization Program");
        setMinimumSize(750, 500);

        createMenuBar();

        auto *centralWidget = new QWidget(this);
        auto *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);

        auto *splitter = new QSplitter(Qt::Horizontal, centralWidget);

        // ==========================================
        // Left Parameter Panel
        // ==========================================
        leftPanel = new QWidget(splitter);
        leftPanel->setFixedWidth(280);
        auto *leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setContentsMargins(6, 6, 6, 6);
        leftLayout->setSpacing(6);

        // Header Card
        auto *headerCard = new QFrame(leftPanel);
        headerCard->setStyleSheet(
            "QFrame {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #153e75, stop:1 #2b6cb0);"
            "  border-radius: 8px;"
            "  padding: 8px 6px;"
            "}"
        );
        auto *headerLayout = new QVBoxLayout(headerCard);
        headerLayout->setContentsMargins(4, 4, 4, 4);
        auto *titleLabel = new QLabel("Generic Asymmetric Contact Analysis", headerCard);
        titleLabel->setWordWrap(true);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 10.5pt; color: #ffffff; background: transparent;");
        headerLayout->addWidget(titleLabel);
        leftLayout->addWidget(headerCard);

        const QString groupBoxStyle =
                "QGroupBox {"
                "  font-weight: bold;"
                "  border: 1px solid #475569;"
                "  border-radius: 6px;"
                "  margin-top: 14px;"
                "  padding-top: 12px;"
                "}"
                "QGroupBox::title {"
                "  subcontrol-origin: margin;"
                "  subcontrol-position: top left;"
                "  left: 8px;"
                "  padding: 0 4px 2px 4px;"
                "}";

        // General Parameters Group
        auto *paramsGroup = new QGroupBox("Rolling Parameters & Track Geometry", leftPanel);
        paramsGroup->setStyleSheet(groupBoxStyle);
        auto *formLayout = new QFormLayout(paramsGroup);
        formLayout->setContentsMargins(8, 10, 8, 8);
        formLayout->setVerticalSpacing(4);
        formLayout->setHorizontalSpacing(6);
        formLayout->setLabelAlignment(Qt::AlignLeft);
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

        rightWheelRadiusSpin = new QDoubleSpinBox(paramsGroup);
        rightWheelRadiusSpin->setRange(100.0, 3000.0);
        rightWheelRadiusSpin->setValue(920.0);
        rightWheelRadiusSpin->setSuffix(" mm");

        leftWheelRadiusSpin = new QDoubleSpinBox(paramsGroup);
        leftWheelRadiusSpin->setRange(100.0, 3000.0);
        leftWheelRadiusSpin->setValue(920.0);
        leftWheelRadiusSpin->setSuffix(" mm");

        asymmetricCheckbox = new QCheckBox("Asymmetric Case", paramsGroup);
        asymmetricCheckbox->setChecked(true);
        connect(asymmetricCheckbox, &QCheckBox::toggled, this, &CalculateWindow::onAsymmetricToggled);

        rightRailInclinSpin = new QDoubleSpinBox(paramsGroup);
        rightRailInclinSpin->setRange(1.0, 1000.0);
        rightRailInclinSpin->setValue(40.0);
        rightRailInclinSpin->setPrefix("1 / ");

        leftRailInclinSpin = new QDoubleSpinBox(paramsGroup);
        leftRailInclinSpin->setRange(1.0, 1000.0);
        leftRailInclinSpin->setValue(40.0);
        leftRailInclinSpin->setPrefix("1 / ");

        backToBackSpin = new QDoubleSpinBox(paramsGroup);
        backToBackSpin->setRange(500.0, 2500.0);
        backToBackSpin->setValue(1360.0);
        backToBackSpin->setSuffix(" mm");

        trackGaugeSpin = new QDoubleSpinBox(paramsGroup);
        trackGaugeSpin->setRange(500.0, 2500.0);
        trackGaugeSpin->setValue(1435.16);
        trackGaugeSpin->setSuffix(" mm");

        formLayout->addRow("Right Wheel Radius:", rightWheelRadiusSpin);
        formLayout->addRow("Left Wheel Radius:", leftWheelRadiusSpin);
        formLayout->addRow("", asymmetricCheckbox);
        formLayout->addRow("Right Rail Inclination:", rightRailInclinSpin);
        formLayout->addRow("Left Rail Inclination:", leftRailInclinSpin);
        formLayout->addRow("Back-to-Back Distance:", backToBackSpin);
        formLayout->addRow("Track Gauge:", trackGaugeSpin);

        leftLayout->addWidget(paramsGroup);

        // Profile Files Selection Group
        auto *filesGroup = new QGroupBox("Wheel & Rail Profiles", leftPanel);
        filesGroup->setStyleSheet(groupBoxStyle);
        auto *filesLayout = new QGridLayout(filesGroup);
        filesLayout->setContentsMargins(8, 10, 8, 8);
        filesLayout->setHorizontalSpacing(6);
        filesLayout->setVerticalSpacing(4);

        auto makeFileRow = [this, filesGroup, filesLayout](int row, const QString &label, QLineEdit *&edit,
                                                           QPushButton *&btn, const char *slot) {
            auto *lbl = new QLabel(label, filesGroup);
            edit = new QLineEdit(filesGroup);
            btn = new QPushButton("Browse...", filesGroup);
            btn->setFixedWidth(75);
            connect(btn, SIGNAL(clicked()), this, slot);

            filesLayout->addWidget(lbl, row * 2, 0, 1, 2);
            filesLayout->addWidget(edit, row * 2 + 1, 0);
            filesLayout->addWidget(btn, row * 2 + 1, 1);
        };

        QPushButton *btnRWheel, *btnRRail, *btnLWheel, *btnLRail;
        makeFileRow(0, "Right Wheel Profile:", rightWheelPathEdit, btnRWheel, SLOT(onBrowseRightWheel()));
        makeFileRow(1, "Right Rail Profile:", rightRailPathEdit, btnRRail, SLOT(onBrowseRightRail()));
        makeFileRow(2, "Left Wheel Profile:", leftWheelPathEdit, btnLWheel, SLOT(onBrowseLeftWheel()));
        makeFileRow(3, "Left Rail Profile:", leftRailPathEdit, btnLRail, SLOT(onBrowseLeftRail()));

        auto onParamChanged = [this]() {
            calculateButton->setEnabled(false);
            m_hasCalculated = false;
        };

        connect(rightWheelRadiusSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onParamChanged);
        connect(leftWheelRadiusSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onParamChanged);
        connect(asymmetricCheckbox, &QCheckBox::toggled, this, onParamChanged);
        connect(rightRailInclinSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onParamChanged);
        connect(leftRailInclinSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onParamChanged);
        connect(backToBackSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onParamChanged);
        connect(trackGaugeSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onParamChanged);

        connect(rightWheelPathEdit, &QLineEdit::textChanged, this, onParamChanged);
        connect(rightRailPathEdit, &QLineEdit::textChanged, this, onParamChanged);
        connect(leftWheelPathEdit, &QLineEdit::textChanged, this, onParamChanged);
        connect(leftRailPathEdit, &QLineEdit::textChanged, this, onParamChanged);

        leftLayout->addWidget(filesGroup);

        // Action Buttons
        auto *btnGrid = new QGridLayout();
        btnGrid->setSpacing(6);

        calculateButton = new QPushButton("Calculate", leftPanel);
        calculateButton->setStyleSheet(
            "QPushButton {"
            "  background-color: #2563eb;"
            "  color: #ffffff;"
            "  font-weight: bold;"
            "  padding: 8px;"
            "  border-radius: 6px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #1d4ed8;"
            "}"
            "QPushButton:disabled {"
            "  background-color: #475569;"
            "  color: #94a3b8;"
            "  border: 1px solid #334155;"
            "}"
        );
        calculateButton->setEnabled(false);
        connect(calculateButton, &QPushButton::clicked, this, &CalculateWindow::onCalculate);

        checkDataButton = new QPushButton("Check Data", leftPanel);
        checkDataButton->setStyleSheet(
            "QPushButton {"
            "  background-color: #ea580c;"
            "  color: #ffffff;"
            "  font-weight: bold;"
            "  padding: 8px;"
            "  border-radius: 6px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #c2410c;"
            "}"
        );
        connect(checkDataButton, &QPushButton::clicked, this, &CalculateWindow::onCheckData);

        saveHtmlButton = new QPushButton("Save as HTML", leftPanel);
        saveHtmlButton->setStyleSheet(
            "QPushButton {"
            "  background-color: #dc2626;"
            "  color: #ffffff;"
            "  font-weight: 500;"
            "  padding: 6px;"
            "  border-radius: 6px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #b91c1c;"
            "}"
        );
        connect(saveHtmlButton, &QPushButton::clicked, this, &CalculateWindow::onSaveHtml);

        saveMarkdownButton = new QPushButton("Save as Markdown", leftPanel);
        saveMarkdownButton->setStyleSheet(
            "QPushButton {"
            "  background-color: #dc2626;"
            "  color: #ffffff;"
            "  font-weight: 500;"
            "  padding: 6px;"
            "  border-radius: 6px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #b91c1c;"
            "}"
        );
        connect(saveMarkdownButton, &QPushButton::clicked, this, &CalculateWindow::onSaveMarkdown);

        btnGrid->addWidget(calculateButton, 0, 0);
        btnGrid->addWidget(checkDataButton, 0, 1);
        btnGrid->addWidget(saveHtmlButton, 1, 0);
        btnGrid->addWidget(saveMarkdownButton, 1, 1);
        leftLayout->addLayout(btnGrid);

        leftLayout->addStretch();

        setupEventFilters(leftPanel);

        // ==========================================
        // Right Tab Widget (5 Charts)
        // ==========================================
        tabWidget = new QTabWidget(splitter);

        rightPairViewGroup = new CustomViewGroup(tabWidget);
        rightPairView = rightPairViewGroup->getView();
        tabWidget->addTab(rightPairViewGroup->getGroupBox(), "Right Wheel-Rail / Track Coordinate System");

        leftPairViewGroup = new CustomViewGroup(tabWidget);
        leftPairView = leftPairViewGroup->getView();
        tabWidget->addTab(leftPairViewGroup->getGroupBox(), "Left Wheel-Rail / Track Coordinate System");

        eqconViewGroup = new CustomViewGroup(tabWidget);
        eqconView = eqconViewGroup->getView();
        tabWidget->addTab(eqconViewGroup->getGroupBox(), "Equivalent Conicity (EN15302 - Nonlinear Method)");

        deltaRViewGroup = new CustomViewGroup(tabWidget);
        deltaRView = deltaRViewGroup->getView();
        tabWidget->addTab(deltaRViewGroup->getGroupBox(), "Wheel's Rolling Radii Difference");

        angleViewGroup = new CustomViewGroup(tabWidget);
        angleView = angleViewGroup->getView();
        tabWidget->addTab(angleViewGroup->getGroupBox(), "Wheel-Rail Contact Inclination");

        splitter->addWidget(leftPanel);
        splitter->addWidget(tabWidget);
        splitter->setCollapsible(0, false);
        splitter->setStretchFactor(0, 0);
        splitter->setStretchFactor(1, 1);
        if (splitter->handle(1)) {
            splitter->handle(1)->setEnabled(false);
        }

        // Initialize stylized background charts without data (empty state)
        CalculatePlotHelper::initEmptyPlot(rightPairView, "Transverse Direction [mm]",
                                           "Vertical Distance (Height) [mm]", -750.0, 750.0, -50.0, 50.0);
        CalculatePlotHelper::initEmptyPlot(leftPairView, "Transverse Direction [mm]", "Vertical Distance (Height) [mm]",
                                           -750.0, 750.0, -50.0, 50.0);
        CalculatePlotHelper::initEmptyPlot(eqconView, "Lateral Wheelset Displacement [mm]",
                                           "Nominal Equivalent Conicity [tan(\u03bb)]", 0.0, 8.0, 0.0, 0.6);
        CalculatePlotHelper::initEmptyPlot(deltaRView, "Lateral Wheelset Displacement y [mm]",
                                           "Right-Left Wheelset \u0394r [mm]", -8.0, 8.0, -8.0, 8.0);
        CalculatePlotHelper::initEmptyPlot(angleView, "Lateral Wheelset Displacement y [mm]",
                                           "Effective Contact Angle [tan(\u03b3)]", -8.0, 8.0, -0.5, 0.5);

        mainLayout->addWidget(splitter);
        setCentralWidget(centralWidget);
    }

    void CalculateWindow::createMenuBar() {
        auto *menuBar = new QMenuBar(this);

        auto *fileMenu = menuBar->addMenu(tr("&File"));
        auto *exitAction = new QAction(tr("E&xit"), this);
        connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
        fileMenu->addAction(exitAction);

        auto *viewMenu = menuBar->addMenu(tr("&View"));
        auto *fullScreenAction = new QAction(tr("Toggle &Full Screen"), this);
        fullScreenAction->setShortcut(QKeySequence(Qt::Key_F11));
        connect(fullScreenAction, &QAction::triggered, this, [this]() {
            if (isFullScreen()) {
                showMaximized();
            } else {
                showFullScreen();
            }
        });
        viewMenu->addAction(fullScreenAction);

        togglePanelAction = new QAction(tr("Show Parameters Panel"), this);
        togglePanelAction->setCheckable(true);
        togglePanelAction->setChecked(true);
        connect(togglePanelAction, &QAction::toggled, this, [this](bool visible) {
            if (leftPanel) {
                leftPanel->setVisible(visible);
            }
        });
        viewMenu->addAction(togglePanelAction);

        auto *openBenchmarkAction = new QAction(tr("Switch to &Benchmark Window..."), this);
        connect(openBenchmarkAction, &QAction::triggered, this, &CalculateWindow::onOpenBenchmarkWindow);
        viewMenu->addAction(openBenchmarkAction);

        auto *returnLauncherAction = new QAction(tr("Return to Main Launcher"), this);
        connect(returnLauncherAction, &QAction::triggered, this, &CalculateWindow::onReturnToLauncher);
        viewMenu->addAction(returnLauncherAction);

        auto *helpMenu = menuBar->addMenu(tr("&Help"));
        auto *aboutAction = new QAction(tr("About &Characterization"), this);
        connect(aboutAction, &QAction::triggered, this, &CalculateWindow::showAbout);
        helpMenu->addAction(aboutAction);

        setMenuBar(menuBar);
    }

    void CalculateWindow::setupDefaultParameters() const {
        rightWheelPathEdit->setText(QString::fromStdString(resolvePath("data/RUIC519A.dat")));
        rightRailPathEdit->setText(QString::fromStdString(resolvePath("data/SUIC519A.dat")));
        leftWheelPathEdit->setText(QString::fromStdString(resolvePath("data/RUIC519B.dat")));
        leftRailPathEdit->setText(QString::fromStdString(resolvePath("data/SUIC519A.dat")));
    }

    void CalculateWindow::onAsymmetricToggled(bool checked) const {
        leftWheelRadiusSpin->setEnabled(checked);
        if (!checked) {
            leftWheelRadiusSpin->setValue(rightWheelRadiusSpin->value());
        }
    }

    void CalculateWindow::onBrowseRightWheel() {
        const QString fn = QFileDialog::getOpenFileName(this, "Select Right Wheel Profile", "",
                                                        "Profile Data (*.dat *.txt);;All Files (*)");
        if (!fn.isEmpty()) rightWheelPathEdit->setText(fn);
    }

    void CalculateWindow::onBrowseRightRail() {
        const QString fn = QFileDialog::getOpenFileName(this, "Select Right Rail Profile", "",
                                                        "Profile Data (*.dat *.txt);;All Files (*)");
        if (!fn.isEmpty()) rightRailPathEdit->setText(fn);
    }

    void CalculateWindow::onBrowseLeftWheel() {
        const QString fn = QFileDialog::getOpenFileName(this, "Select Left Wheel Profile", "",
                                                        "Profile Data (*.dat *.txt);;All Files (*)");
        if (!fn.isEmpty()) leftWheelPathEdit->setText(fn);
    }

    void CalculateWindow::onBrowseLeftRail() {
        const QString fn = QFileDialog::getOpenFileName(this, "Select Left Rail Profile", "",
                                                        "Profile Data (*.dat *.txt);;All Files (*)");
        if (!fn.isEmpty()) leftRailPathEdit->setText(fn);
    }

    void CalculateWindow::onCheckData() {
        try {
            const DataReader rWheel(rightWheelPathEdit->text().toStdString());
            const DataReader rRail(rightRailPathEdit->text().toStdString());
            const DataReader lWheel(leftWheelPathEdit->text().toStdString());
            const DataReader lRail(leftRailPathEdit->text().toStdString());

            const auto &c1 = rWheel.getColumn(0);
            const auto &c2 = rRail.getColumn(0);
            const auto &c3 = lWheel.getColumn(0);
            const auto &c4 = lRail.getColumn(0);

            if (c1.empty() || c2.empty() || c3.empty() || c4.empty()) {
                calculateButton->setEnabled(false);
                QMessageBox::warning(this, "Data Check Warning", "One or more profile files contain empty data.");
                return;
            }

            calculateButton->setEnabled(true);
            QMessageBox::information(
                this,
                "Data Check Success",
                QString("All profile files verified successfully:\n\n"
                    "\u2022 Right Wheel: %1 points\n"
                    "\u2022 Right Rail: %2 points\n"
                    "\u2022 Left Wheel: %3 points\n"
                    "\u2022 Left Rail: %4 points\n\n"
                    "Calculate button is now enabled.")
                .arg(c1.size())
                .arg(c2.size())
                .arg(c3.size())
                .arg(c4.size())
            );
        } catch (const std::exception &e) {
            calculateButton->setEnabled(false);
            QMessageBox::critical(this, "Data Check Error", QString("Error checking profiles: %1").arg(e.what()));
        }
    }

    static QImage captureChartImage(const QChartView *chartView, const int width = 1100, const int height = 550) {
        if (!chartView || !chartView->chart()) return {};

        const QSizeF oldSize = chartView->chart()->size();

        QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::white);

        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        chartView->chart()->resize(QSizeF(width, height));
        if (chartView->scene()) {
            chartView->scene()->render(&painter, QRectF(0, 0, width, height), QRectF(0, 0, width, height));
        } else {
            chartView->chart()->paint(&painter, nullptr, nullptr);
        }

        if (oldSize.isValid() && oldSize.width() > 0 && oldSize.height() > 0) {
            chartView->chart()->resize(oldSize);
        }

        return image;
    }

    bool CalculateWindow::exportReportToHtml(const QString &filePath, const QString &operatingConditions,
                                             QString *errorMessage) const {
        if (!m_hasCalculated) {
            if (errorMessage) *errorMessage = "Please perform calculation ('Calculate') before exporting a report.";
            return false;
        }

        Core::CalculationReportData reportData;
        reportData.params = m_lastParams;
        reportData.params.operatingConditions = operatingConditions.trimmed();
        reportData.conicityTable = WRCC::Core::ReportExporter::extractIntegerConicityTable(m_lastYcp, m_lastAng);

        // Grab high-resolution images from chart views
        reportData.rightPairChart = captureChartImage(rightPairView);
        reportData.leftPairChart = captureChartImage(leftPairView);
        reportData.eqConChart = captureChartImage(eqconView);
        reportData.deltaRChart = captureChartImage(deltaRView);
        reportData.contactAngleChart = captureChartImage(angleView);

        return WRCC::Core::ReportExporter::exportToHtml(filePath, reportData, true, errorMessage);
    }

    bool CalculateWindow::exportReportToMarkdown(const QString &filePath, const QString &operatingConditions,
                                                 QString *errorMessage) const {
        if (!m_hasCalculated) {
            if (errorMessage) *errorMessage = "Please perform calculation ('Calculate') before exporting a report.";
            return false;
        }

        Core::CalculationReportData reportData;
        reportData.params = m_lastParams;
        reportData.params.operatingConditions = operatingConditions.trimmed();
        reportData.conicityTable = Core::ReportExporter::extractIntegerConicityTable(m_lastYcp, m_lastAng);

        // Grab high-resolution images from chart views
        reportData.rightPairChart = captureChartImage(rightPairView);
        reportData.leftPairChart = captureChartImage(leftPairView);
        reportData.eqConChart = captureChartImage(eqconView);
        reportData.deltaRChart = captureChartImage(deltaRView);
        reportData.contactAngleChart = captureChartImage(angleView);

        return Core::ReportExporter::exportToMarkdown(filePath, reportData, true, errorMessage);
    }

    void CalculateWindow::onSaveHtml() {
        if (!m_hasCalculated) {
            QMessageBox::warning(this, "Save Report Warning",
                                 "Please perform calculation ('Calculate') before exporting a report.");
            return;
        }

        bool ok = false;
        const QString userComments = QInputDialog::getMultiLineText(
            this,
            "Report Operating Conditions",
            "Enter optional operating conditions / commentaries to include in the report:",
            m_lastParams.operatingConditions,
            &ok
        );
        if (!ok) {
            return;
        }

        const QString defaultName = "Final Technical Report " + QDateTime::currentDateTime().toString(
                                        "yyyy.MM.dd-HHmmss") +
                                    ".html";
        const QString docsPath = QString::fromStdString(resolvePath("docs/outputs"));
        QDir().mkpath(docsPath);
        const QString defaultFilePath = QDir(docsPath).filePath(defaultName);
        QString filePath = QFileDialog::getSaveFileName(this, "Save Report as HTML", defaultFilePath,
                                                        "HTML Documents (*.html *.htm);;All Files (*)");
        if (filePath.isEmpty()) return;
        if (!filePath.endsWith(".html", Qt::CaseInsensitive) && !filePath.endsWith(".htm", Qt::CaseInsensitive)) {
            filePath += ".html";
        }

        QString errorMsg;
        if (exportReportToHtml(filePath, userComments, &errorMsg)) {
            QMessageBox::information(this, "Save Success",
                                     QString("HTML report successfully saved to:\n%1").arg(filePath));
        } else {
            QMessageBox::critical(this, "Export Error", QString("Failed to generate HTML report:\n%1").arg(errorMsg));
        }
    }

    void CalculateWindow::onSaveMarkdown() {
        if (!m_hasCalculated) {
            QMessageBox::warning(this, "Save Report Warning",
                                 "Please perform calculation ('Calculate') before exporting a report.");
            return;
        }

        bool ok = false;
        const QString userComments = QInputDialog::getMultiLineText(
            this,
            "Report Operating Conditions",
            "Enter optional operating conditions / commentaries to include in the report:",
            m_lastParams.operatingConditions,
            &ok
        );
        if (!ok) {
            return;
        }

        const QString defaultName = "Final Technical Report " + QDateTime::currentDateTime().toString(
                                        "yyyy.MM.dd-HHmmss") +
                                    ".md";
        const QString docsPath = QString::fromStdString(resolvePath("docs/outputs"));
        QDir().mkpath(docsPath);
        const QString defaultFilePath = QDir(docsPath).filePath(defaultName);
        QString filePath = QFileDialog::getSaveFileName(this, "Save Report as Markdown", defaultFilePath,
                                                        "Markdown Documents (*.md);;Text Files (*.txt);;All Files (*)");
        if (filePath.isEmpty()) return;
        if (!filePath.endsWith(".md", Qt::CaseInsensitive) && !filePath.endsWith(".txt", Qt::CaseInsensitive)) {
            filePath += ".md";
        }

        QString errorMsg;
        if (exportReportToMarkdown(filePath, userComments, &errorMsg)) {
            QMessageBox::information(this, "Save Success", QString("Report successfully saved to:\n%1").arg(filePath));
        } else {
            QMessageBox::critical(this, "Export Error", QString("Failed to save report:\n%1").arg(errorMsg));
        }
    }

    bool CalculateWindow::performCalculation(QString *errorMessage) {
        try {
            double rr0 = rightWheelRadiusSpin->value();
            double rl0 = asymmetricCheckbox->isChecked() ? leftWheelRadiusSpin->value() : rr0;
            double trackGauge = trackGaugeSpin->value();
            double s = trackGauge * 0.5;
            double cer = 1.0 / rightRailInclinSpin->value();

            DataReader wheelRight(rightWheelPathEdit->text().toStdString());
            DataReader wheelLeft(leftWheelPathEdit->text().toStdString());
            DataReader railRight(rightRailPathEdit->text().toStdString());

            const auto &y1 = railRight.getColumn(0);
            const auto &zp1 = railRight.getColumn(1);
            const auto &y2 = wheelRight.getColumn(0);
            const auto &zp2d = wheelRight.getColumn(1);
            const auto &zp2i = wheelLeft.getColumn(1);

            if (y1.empty() || y2.empty() || zp1.empty() || zp2d.empty() || zp2i.empty()) {
                if (errorMessage)
                    *errorMessage = "One or more profile datasets are empty. Please check input file paths.";
                return false;
            }

            // Full range [-13, 13] mm with step 0.1 for contact points detection
            const auto ys_full = Linspace(-13, 13, 0.1).linspacec();

            ConPon rail_sys(y1, y2, zp1, zp2d, zp2i, 0.0, s);
            for (double d_l: ys_full) {
                rail_sys.transformCoordinates(rr0, rl0, d_l, cer, s);
            }

            const auto &rrr_final = rail_sys.get_rrr();
            const auto &rrl_final = rail_sys.get_rrl();
            const auto &radr_final = rail_sys.get_radr();
            const auto &radl_final = rail_sys.get_radl();
            const auto &linr = rail_sys.get_linr();
            const auto &linl = rail_sys.get_linl();
            const auto &y_1 = rail_sys.get_y_1();
            const auto &y_2 = rail_sys.get_y_2();
            const auto &zp_1 = rail_sys.get_zp_1();

            // Calculate Delta-r and contact angles for all points in ys_full
            double dr0 = rr0 - rl0;
            int sign = (dr0 > 0) ? -1 : 1;
            std::vector<double> dr(rrr_final.size());
            for (size_t i = 0; i < rrr_final.size(); ++i) {
                dr[i] = (rrr_final[i] - rrl_final[i]) + sign * dr0 / 2.0;
            }

            std::vector<double> rad(radr_final.size());
            for (size_t i = 0; i < radr_final.size(); ++i) {
                rad[i] = std::tan(radl_final[i] - radr_final[i]);
            }

            std::vector<double> drf(dr.size());
            for (size_t i = 0; i < dr.size(); ++i) {
                drf[i] = (std::abs(dr[i]) < 1e-4) ? 0.0 : dr[i];
            }

            // Extract calculation sub-range [-7.0, 7.0] mm to prevent flange contact
            // non-linearities from distorting Output / equivalent conicity calculation
            constexpr double min_calc = -7.0, max_calc = 7.0;
            std::vector<double> ys_calc;
            std::vector<double> drf_calc;
            std::vector<double> rad_calc;

            for (size_t i = 0; i < ys_full.size(); ++i) {
                if (ys_full[i] >= min_calc - 1e-6 && ys_full[i] <= max_calc + 1e-6) {
                    ys_calc.push_back(ys_full[i]);
                    drf_calc.push_back(drf[i]);
                    rad_calc.push_back(rad[i]);
                }
            }

            bool isSymmetric = (std::abs(rr0 - rl0) < 1e-4) &&
                               (rightWheelPathEdit->text().trimmed() == leftWheelPathEdit->text().trimmed()) &&
                               (!asymmetricCheckbox->isChecked() || std::abs(rr0 - rl0) < 1e-4);

            int c = isSymmetric ? 1 : -1;
            Output out(ys_calc, drf_calc, trackGauge, rr0, min_calc, max_calc, c);

            // Save last calculation state for report generation
            m_lastYcp = out.getYcp();
            m_lastAng = out.getAng();
            m_lastParams.rightRadius = rr0;
            m_lastParams.leftRadius = rl0;
            m_lastParams.rightRailInclin = rightRailInclinSpin->value();
            m_lastParams.leftRailInclin = leftRailInclinSpin->value();
            m_lastParams.trackGauge = trackGauge;
            m_lastParams.backToBack = backToBackSpin->value();
            m_lastParams.isAsymmetric = asymmetricCheckbox->isChecked();
            m_lastParams.rightWheelProfile = rightWheelPathEdit->text();
            m_lastParams.leftWheelProfile = leftWheelPathEdit->text();
            m_lastParams.rightRailProfile = rightRailPathEdit->text();
            m_lastParams.leftRailProfile = leftRailPathEdit->text();
            m_hasCalculated = true;

            // Apply all 5 plots
            ContactPointsPlot::applyPlot(rightPairView, y_1, y_2, zp_1, zp2d, linr, 1, "Right Wheel-Rail Pair");
            ContactPointsPlot::applyPlot(leftPairView, y_1, y_2, zp_1, zp2i, linl, -1, "Left Wheel-Rail Pair");
            CalculatedEqConPlot::applyPlot(eqconView, out.getYcp(), out.getAng(), isSymmetric);
            CalculatedDeltaRPlot::applyPlot(deltaRView, ys_calc, drf_calc);
            CalculatedAnglePlot::applyPlot(angleView, ys_calc, rad_calc);

            return true;
        } catch (const std::exception &ex) {
            m_hasCalculated = false;
            if (errorMessage) *errorMessage = QString("Error during contact computation: %1").arg(ex.what());
            return false;
        }
    }

    void CalculateWindow::onCalculate() {
        QString errorMsg;
        if (!performCalculation(&errorMsg)) {
            QMessageBox::critical(this, "Calculation Error", errorMsg);
        }
    }

    void CalculateWindow::onOpenBenchmarkWindow() {
        this->hide();
        auto *benchWin = new BenchmarkWindow(m_launcher, nullptr);
        benchWin->setAttribute(Qt::WA_DeleteOnClose);
        benchWin->showMaximized();
    }

    void CalculateWindow::onReturnToLauncher() {
        this->hide();
        if (m_launcher) {
            m_launcher->show();
        }
    }

    void CalculateWindow::showAbout() {
        const QString aboutText =
                "<h2 style='color: lightblue; text-decoration: underline; text-align: center;'>"
                "Wheel / Rail Contact Characterization Program</h2>"

                "<p style='font-size: 14px; font-weight: bold; text-align: left;'>"
                "Description:</p>"

                "<p style='text-align: justify; text-indent: 85px;'>"
                "Railway application aimed to characterize the physical wheel-rail interaction "
                "by computing the full set of contact parameters.</p>"

                "<p style='font-size: 12px; font-style: italic; text-align: center;'>"
                "Version: 2.0-beta</p>"

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
