#include "BenchmarkPlots.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <QBrush>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QLinearGradient>
#include <QPen>

namespace WRCC::UI {
    namespace config {
        static QString resolveDataPath(const std::string &filename) {
            namespace fs = std::filesystem;
            const fs::path p(filename);
            if (fs::exists(p)) return QString::fromStdString(filename);

            const fs::path parentP = fs::path("..") / p;
            if (fs::exists(parentP)) return QString::fromStdString(parentP.string());

            static const fs::path appDir = [] {
                const QString appDirPath = QCoreApplication::applicationDirPath();
                return fs::path(appDirPath.toStdString());
            }();

            const fs::path appP = appDir / p;
            if (fs::exists(appP)) return QString::fromStdString(appP.string());

            const fs::path appParentP = appDir / ".." / p;
            if (fs::exists(appParentP)) return QString::fromStdString(appParentP.string());

            return QString::fromStdString(filename);
        }

        static void configureChartAxes(const ZoomAndScroll *view, const QString &xTitle, const QString &yTitle) {
            view->chart()->createDefaultAxes();
            constexpr QColor axisColor(200, 200, 200);

            auto xAxes = view->chart()->axes(Qt::Horizontal);
            if (!xAxes.isEmpty()) {
                if (auto *axisX = qobject_cast<QValueAxis *>(xAxes[0])) {
                    const QPen axisPen(axisColor);
                    axisX->setLinePen(axisPen);
                    axisX->setTickCount(11);
                    axisX->setGridLinePen(axisColor);
                    axisX->setRange(view->minX, view->maxX);
                    axisX->setTitleText(xTitle);
                }
            }

            auto yAxes = view->chart()->axes(Qt::Vertical);
            if (!yAxes.isEmpty()) {
                if (auto *axisY = qobject_cast<QValueAxis *>(yAxes[0])) {
                    const QPen axisPen(axisColor);
                    axisY->setLinePen(axisPen);
                    axisY->setTickCount(11);
                    axisY->setGridLinePen(axisColor);
                    axisY->setRange(view->minY, view->maxY);
                    axisY->setTitleText(yTitle);
                }
            }

            QLinearGradient gradient(0, 0, 0, 700);
            gradient.setColorAt(0.0, QColor(255, 255, 255));
            gradient.setColorAt(0.5, QColor(235, 250, 255));
            gradient.setColorAt(1.0, QColor(162, 210, 232));
            view->chart()->setBackgroundBrush(QBrush(gradient));
            view->chart()->legend()->setVisible(true);
        }
    } // namespace config

    BenchmarkEqConPlot::BenchmarkEqConPlot(ZoomAndScroll *view,
                                           const std::vector<double> &calcX,
                                           const std::vector<double> &calcY,
                                           const std::string &benchDataFile,
                                           bool isSymmetric,
                                           QObject *parent)
        : QObject(parent) {
        applyPlot(view, calcX, calcY, benchDataFile, isSymmetric);
    }

    void BenchmarkEqConPlot::applyPlot(ZoomAndScroll *view,
                                       const std::vector<double> &calcX,
                                       const std::vector<double> &calcY,
                                       const std::string &benchDataFile,
                                       bool isSymmetric) {
        if (!view || !view->chart()) return;
        view->chart()->removeAllSeries();

        // 1. Prepare benchmark tolerance curves from file (columns 3 to 6 -> indices 2, 3, 4, 5)
        QString resolvedBench = config::resolveDataPath(benchDataFile);
        if (QFile::exists(resolvedBench)) {
            try {
                DataReader benchReader(resolvedBench.toStdString());
                const auto &benchX = benchReader.getColumn(0);

                // Tolerance curves (without tracking capability)
                // Column 2 (+0.05 / upper tolerance)
                const auto &col2 = benchReader.getColumn(2);
                if (!col2.empty()) {
                    auto *upperTolSeries = new QLineSeries();
                    upperTolSeries->setName("Upper Tolerance (+0.05)");
                    const size_t count = std::min(benchX.size(), col2.size());
                    QList<QPointF> points;
                    points.reserve(static_cast<qsizetype>(count));
                    for (size_t i = 0; i < count; ++i) {
                        points.append(QPointF(benchX[i], col2[i]));
                    }
                    upperTolSeries->replace(points);
                    QPen pen(QColor(0, 180, 0));
                    pen.setStyle(Qt::DashLine);
                    pen.setWidth(2);
                    upperTolSeries->setPen(pen);
                    view->chart()->addSeries(upperTolSeries);
                }

                // Column 3 (-0.05 / lower tolerance)
                const auto &col3 = benchReader.getColumn(3);
                if (!col3.empty()) {
                    auto *lowerTolSeries = new QLineSeries();
                    lowerTolSeries->setName("Lower Tolerance (-0.05)");
                    const size_t count = std::min(benchX.size(), col3.size());
                    QList<QPointF> points;
                    points.reserve(static_cast<qsizetype>(count));
                    for (size_t i = 0; i < count; ++i) {
                        points.append(QPointF(benchX[i], col3[i]));
                    }
                    lowerTolSeries->replace(points);
                    QPen pen(QColor(220, 0, 0));
                    pen.setStyle(Qt::DashLine);
                    pen.setWidth(2);
                    lowerTolSeries->setPen(pen);
                    view->chart()->addSeries(lowerTolSeries);
                }

                // Column 4 (upper limit)
                const auto &col4 = benchReader.getColumn(4);
                if (!col4.empty()) {
                    auto *maxTolSeries = new QLineSeries();
                    maxTolSeries->setName("Max Boundary");
                    const size_t count = std::min(benchX.size(), col4.size());
                    QList<QPointF> points;
                    points.reserve(static_cast<qsizetype>(count));
                    for (size_t i = 0; i < count; ++i) {
                        points.append(QPointF(benchX[i], col4[i]));
                    }
                    maxTolSeries->replace(points);
                    QPen pen(QColor(34, 139, 34));
                    pen.setStyle(Qt::DotLine);
                    pen.setWidth(1);
                    maxTolSeries->setPen(pen);
                    view->chart()->addSeries(maxTolSeries);
                }

                // Column 5 (lower limit)
                const auto &col5 = benchReader.getColumn(5);
                if (!col5.empty()) {
                    auto *minTolSeries = new QLineSeries();
                    minTolSeries->setName("Min Boundary");
                    const size_t count = std::min(benchX.size(), col5.size());
                    QList<QPointF> points;
                    points.reserve(static_cast<qsizetype>(count));
                    for (size_t i = 0; i < count; ++i) {
                        points.append(QPointF(benchX[i], col5[i]));
                    }
                    minTolSeries->replace(points);
                    QPen pen(QColor(178, 34, 34));
                    pen.setStyle(Qt::DotLine);
                    pen.setWidth(1);
                    minTolSeries->setPen(pen);
                    view->chart()->addSeries(minTolSeries);
                }
            } catch (const std::exception &e) {
                qWarning() << "Failed to read benchmark tolerance file:" << resolvedBench << "-" << e.what();
            }
        } else {
            qWarning() << "Benchmark tolerance file not found:" << QString::fromStdString(benchDataFile);
        }

        // 2. Add calculated conicity with tracking capabilities enabled
        if (!calcX.empty() && !calcY.empty()) {
            std::vector<double> finalX = calcX;
            std::vector<double> finalY = calcY;

            const auto calcF_ = static_cast<long double>(calcX.front());
            const auto calcB_ = static_cast<long double>(calcX.back());
            if (isSymmetric && calcX.size() >= 2) {
                try {
                    const Pchip spline(calcX, calcY);
                    finalX = Linspace(calcF_, calcB_,
                                      0.01).linspacec();
                    finalY.clear();
                    finalY.reserve(finalX.size());
                    for (const double x: finalX) {
                        finalY.push_back(spline.interpolate(x));
                    }
                } catch (const std::exception &e) {
                    qWarning() << "Spline interpolation failed for conicity plot:" << e.what();
                    finalX = calcX;
                    finalY = calcY;
                } catch (...) {
                    qWarning() << "Unknown error during spline interpolation for conicity plot.";
                    finalX = calcX;
                    finalY = calcY;
                }
            }

            auto *calcSeries = new ScatterSeries(view);
            calcSeries->setName("Calculated Conicity (EN 15302)");
            const size_t count = std::min(finalX.size(), finalY.size());
            QList<QPointF> points;
            points.reserve(static_cast<qsizetype>(count));
            for (size_t i = 0; i < count; ++i) {
                points.append(QPointF(finalX[i], finalY[i]));
            }
            calcSeries->replace(points);

            calcSeries->setPen(QPen(Qt::blue));
            calcSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
            calcSeries->setMarkerSize(7);
            calcSeries->setPointLabelsFormat("@xPoint, @yPoint");
            view->chart()->addSeries(calcSeries);
        }

        view->updateXLimits(view->chart());
        config::configureChartAxes(view, "Lateral Wheelset Displacement y [mm]",
                                   "Equivalent Conicity - tan(\u03b3) [\u00b0]");
    }

    BenchmarkDeltaRPlot::BenchmarkDeltaRPlot(ZoomAndScroll *view,
                                             const std::vector<double> &ys,
                                             const std::vector<double> &calculatedDr,
                                             const std::string &referenceDrFile,
                                             QObject *parent)
        : QObject(parent) {
        applyPlot(view, ys, calculatedDr, referenceDrFile);
    }

    void BenchmarkDeltaRPlot::applyPlot(ZoomAndScroll *view,
                                        const std::vector<double> &ys,
                                        const std::vector<double> &calculatedDr,
                                        const std::string &referenceDrFile) {
        if (!view || !view->chart()) return;
        view->chart()->removeAllSeries();

        // 1. Plot reference benchmark Delta R curve if available
        QString resolvedRef = config::resolveDataPath(referenceDrFile);
        if (QFile::exists(resolvedRef)) {
            try {
                const DataReader reader(resolvedRef.toStdString());
                const auto &refDrs = reader.getColumn(0);
                if (!refDrs.empty() && refDrs.size() == ys.size()) {
                    auto *refSeries = new QLineSeries();
                    refSeries->setName("Standard Benchmark \u0394r");
                    QList<QPointF> points;
                    points.reserve(static_cast<qsizetype>(ys.size()));
                    for (size_t i = 0; i < ys.size(); ++i) {
                        points.append(QPointF(ys[i], refDrs[i]));
                    }
                    refSeries->replace(points);
                    QPen pen(QColor(0, 102, 204));
                    pen.setStyle(Qt::DashLine);
                    pen.setWidth(2);
                    refSeries->setPen(pen);
                    view->chart()->addSeries(refSeries);
                }
            } catch (const std::exception &e) {
                qWarning() << "Failed to read reference Delta R file:" << resolvedRef << "-" << e.what();
            }
        } else {
            qWarning() << "Reference Delta R file not found:" << QString::fromStdString(referenceDrFile);
        }

        // 2. Plot calculated Delta R curve
        if (!ys.empty() && !calculatedDr.empty()) {
            const size_t count = std::min(ys.size(), calculatedDr.size());
            QList<QPointF> points;
            points.reserve(static_cast<qsizetype>(count));
            for (size_t i = 0; i < count; ++i) {
                points.append(QPointF(ys[i], calculatedDr[i]));
            }

            auto *calcScatter = new ScatterSeries(view);
            calcScatter->setName("Calculated \u0394r (Points)");
            calcScatter->replace(points);
            calcScatter->setColor(Qt::darkGreen);
            calcScatter->setMarkerShape(QScatterSeries::MarkerShapeStar);
            calcScatter->setMarkerSize(12);
            calcScatter->setPointLabelsFormat("@xPoint, @yPoint");
            view->chart()->addSeries(calcScatter);

            auto *calcLine = new LineSeries(view);
            calcLine->setName("Calculated \u0394r (Line)");
            calcLine->replace(points);
            QPen linePen(Qt::red);
            linePen.setStyle(Qt::SolidLine);
            linePen.setWidth(2);
            calcLine->setPen(linePen);
            view->chart()->addSeries(calcLine);
        }

        view->updateXLimits(view->chart());
        config::configureChartAxes(view, "Lateral Wheelset Displacement y [mm]", "Right-Left Wheelset \u0394r [mm]");
    }
} // namespace WRCC::UI
