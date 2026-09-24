#include "LauncherWindow.h"
#include "BenchmarkWindow.h"
#include "CalculateWindow.h"

#include <QApplication>
#include <QDialog>
#include <QFile>
#include <QFrame>
#include <QGuiApplication>
#include <QIcon>
#include <QMessageBox>
#include <QPixmap>
#include <QScreen>

namespace WRCC::UI {
    LauncherWindow::LauncherWindow(QWidget *parent)
        : QWidget(parent) {
        initUI();
    }

    LauncherWindow::~LauncherWindow() {
        delete benchmarkWindow;
        delete calculateWindow;
    }

    void LauncherWindow::initUI() {
        setWindowTitle("WRCC - Wheel-Rail Contact Characterization Program");
        setWindowFlags(
            Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint |
            Qt::WindowMinimizeButtonHint);
        setFixedSize(540, 480);

        // Center on screen
        if (const auto *screen = QGuiApplication::primaryScreen()) {
            const QRect sg = screen->availableGeometry();
            move(sg.x() + (sg.width() - 540) / 2, sg.y() + (sg.height() - 480) / 2);
        }

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(20, 18, 20, 18);
        mainLayout->setSpacing(14);

        auto *titleCard = new QFrame(this);
        titleCard->setStyleSheet(
            "QFrame {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1e3a8a, stop:1 #2563eb);"
            "  border-radius: 10px;"
            "  padding: 14px 10px;"
            "}"
        );
        auto *titleLayout = new QVBoxLayout(titleCard);
        titleLayout->setSpacing(0);
        titleLayout->setContentsMargins(6, 0, 6, 0);

        auto *mainTitle = new QLabel("Wheel / Rail Contact Characterization", titleCard);
        mainTitle->setStyleSheet("font-size: 13.5pt; font-weight: bold; color: #ffffff; background: transparent;");
        mainTitle->setAlignment(Qt::AlignCenter);

        auto *subTitle = new QLabel("Equivalent Conicity & Contact Geometry Analysis", titleCard);
        subTitle->setStyleSheet("font-size: 11pt; color: #e0f2fe; font-weight: 500; background: transparent;");
        subTitle->setAlignment(Qt::AlignCenter);

        auto *stdBadge = new QLabel("\u2014 Compliant with European Standard EN 15302:2008+A1:2010 (Annex E) \u2014",
                                    titleCard);
        stdBadge->setStyleSheet("font-size: 9pt; color: #bfdbfe; font-style: italic; background: transparent;");
        stdBadge->setAlignment(Qt::AlignCenter);

        titleLayout->addWidget(mainTitle);
        titleLayout->addWidget(subTitle);
        titleLayout->addWidget(stdBadge);
        mainLayout->addWidget(titleCard);

        // Instructions & Mode Info Card
        auto *modeCard = new QFrame(this);
        modeCard->setStyleSheet(
            "QFrame {"
            "  background-color: #1e293b;"
            "  border: 1px solid #334155;"
            "  border-radius: 8px;"
            "  padding: 10px 14px;"
            "}"
        );
        auto *modeLayout = new QVBoxLayout(modeCard);
        modeLayout->setSpacing(8);
        modeLayout->setContentsMargins(6, 6, 6, 6);

        auto *modeHeader = new QLabel("Calculation Modes & Information:", modeCard);
        modeHeader->setStyleSheet("font-size: 10pt; font-weight: bold; color: #f8fafc; background: transparent;");
        modeHeader->setAlignment(Qt::AlignCenter);

        instructionsButton = new QPushButton(" View Instructions / Calculation Modes Guide", modeCard);
        instructionsButton->setMinimumHeight(36);
        instructionsButton->setCursor(Qt::PointingHandCursor);
        instructionsButton->setStyleSheet(
            "QPushButton {"
            "  background-color: #334155;"
            "  color: #f8fafc;"
            "  font-size: 9.5pt;"
            "  font-weight: 600;"
            "  border: 1px solid #475569;"
            "  border-radius: 6px;"
            "  padding: 6px 12px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #475569;"
            "}"
            "QPushButton:pressed {"
            "  background-color: #1e293b;"
            "}"
        );
        connect(instructionsButton, &QPushButton::clicked, this, &LauncherWindow::showInstructions);

        modeLayout->addWidget(modeHeader);
        modeLayout->addWidget(instructionsButton);
        mainLayout->addWidget(modeCard);

        // Buttons Container
        auto *buttonsLayout = new QVBoxLayout();
        buttonsLayout->setSpacing(10);
        buttonsLayout->setAlignment(Qt::AlignCenter);

        benchmarkButton = new QPushButton(" \u2699  EN 15302 Benchmark Cases (E1 \u2014 E9)", this);
        benchmarkButton->setFixedSize(380, 44);
        benchmarkButton->setCursor(Qt::PointingHandCursor);
        benchmarkButton->setEnabled(false);
        benchmarkButton->setStyleSheet(
            "QPushButton {"
            "  background-color: #2563eb;"
            "  color: #ffffff;"
            "  font-size: 10.5pt;"
            "  font-weight: bold;"
            "  border-radius: 6px;"
            "  padding: 6px 14px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #1d4ed8;"
            "}"
            "QPushButton:pressed {"
            "  background-color: #1e40af;"
            "}"
            "QPushButton:disabled {"
            "  background-color: #64748b;"
            "  color: #cbd5e1;"
            "}"
        );

        calculateButton = new QPushButton(" \u2692  General Wheel-Rail Contact Calculation", this);
        calculateButton->setFixedSize(380, 44);
        calculateButton->setCursor(Qt::PointingHandCursor);
        calculateButton->setEnabled(false);
        calculateButton->setStyleSheet(
            "QPushButton {"
            "  background-color: #16a34a;"
            "  color: #ffffff;"
            "  font-size: 10.5pt;"
            "  font-weight: bold;"
            "  border-radius: 6px;"
            "  padding: 6px 14px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #15803d;"
            "}"
            "QPushButton:pressed {"
            "  background-color: #166534;"
            "}"
            "QPushButton:disabled {"
            "  background-color: #64748b;"
            "  color: #cbd5e1;"
            "}"
        );

        connect(benchmarkButton, &QPushButton::clicked, this, &LauncherWindow::openBenchmark);
        connect(calculateButton, &QPushButton::clicked, this, &LauncherWindow::openCalculate);

        buttonsLayout->addWidget(benchmarkButton);
        buttonsLayout->addWidget(calculateButton);
        mainLayout->addLayout(buttonsLayout);

        mainLayout->addStretch();

        // Footer actions
        auto *footerLayout = new QHBoxLayout();
        aboutButton = new QPushButton("About", this);
        aboutButton->setFixedSize(90, 32);
        aboutButton->setCursor(Qt::PointingHandCursor);

        exitButton = new QPushButton("Exit", this);
        exitButton->setFixedSize(90, 32);
        exitButton->setCursor(Qt::PointingHandCursor);

        connect(aboutButton, &QPushButton::clicked, this, &LauncherWindow::showAbout);
        connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

        footerLayout->addWidget(aboutButton);
        footerLayout->addStretch();
        footerLayout->addWidget(exitButton);
        mainLayout->addLayout(footerLayout);
    }

    void LauncherWindow::openBenchmark() {
        if (!benchmarkWindow) {
            benchmarkWindow = new BenchmarkWindow(this);
        }
        this->hide();
        benchmarkWindow->showMaximized();
    }

    void LauncherWindow::openCalculate() {
        if (!calculateWindow) {
            calculateWindow = new CalculateWindow(this);
        }
        this->hide();
        calculateWindow->showMaximized();
    }

    void LauncherWindow::showInstructions() {
        QDialog dialog(this);
        dialog.setWindowTitle("WRCC \u2014 Instructions & Calculation Modes");
        dialog.setMinimumWidth(480);
        dialog.setStyleSheet("background-color: #0f172a; color: #f8fafc;");

        auto *layout = new QVBoxLayout(&dialog);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(14);

        auto *infoLabel = new QLabel(
            "<h3 style='color: #60a5fa; margin-top: 0;'>Calculation Modes Guide</h3>"
            "<p style='line-height: 1.45; color: #e2e8f0; font-size: 10pt;'>"
            "<b>\u2022 EN 15302 Benchmark Cases (E1 \u2014 E9):</b><br/>"
            "Standardized validation against reference cases defined in Annex E of the EN 15302:2008+A1:2010 standard. "
            "Includes predefined symmetric and asymmetric combinations with upper and lower tolerance limits for equivalent conicity."
            "</p>"
            "<p style='line-height: 1.45; color: #e2e8f0; font-size: 10pt;'>"
            "<b>\u2022 General Wheel-Rail Contact Calculation:</b><br/>"
            "Comprehensive contact geometry characterization for arbitrary wheel and rail profiles, customizable rolling radii, "
            "rail inclinations (e.g., 1/20, 1/40), and track gauge configurations."
            "</p>"
            "<hr style='border: 1px solid #334155;'/>"
            "<p style='color: #94a3b8; font-size: 9pt; margin-bottom: 0;'>"
            "<i>Closing this instructions dialog enables the calculation mode buttons below.</i>"
            "</p>",
            &dialog
        );
        infoLabel->setTextFormat(Qt::RichText);
        infoLabel->setWordWrap(true);
        layout->addWidget(infoLabel);

        auto *closeBtn = new QPushButton("Got it \u2014 Enable Modes", &dialog);
        closeBtn->setMinimumHeight(36);
        closeBtn->setCursor(Qt::PointingHandCursor);
        closeBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #2563eb;"
            "  color: white;"
            "  font-weight: bold;"
            "  border-radius: 6px;"
            "  padding: 6px 18px;"
            "}"
            "QPushButton:hover { background-color: #1d4ed8; }"
        );
        connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
        layout->addWidget(closeBtn, 0, Qt::AlignCenter);

        dialog.exec();

        // Enable calculation mode buttons
        benchmarkButton->setEnabled(true);
        calculateButton->setEnabled(true);
    }

    void LauncherWindow::showAbout() {
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
