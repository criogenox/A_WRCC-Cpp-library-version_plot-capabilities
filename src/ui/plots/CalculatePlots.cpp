#include "CalculatePlots.h"

#include <algorithm>
#include <cmath>
#include <QBrush>
#include <QLinearGradient>
#include <QPen>
#include <ranges>

#include "pchip.h"
#include "input.h"

namespace WRCC::UI {
    namespace config {
        static void computePaddedRange(const double dataMin, const double dataMax,
                                       double &outMin, double &outMax,
                                       const double defaultMin, const double defaultMax) {
            if (std::isnan(dataMin) || std::isnan(dataMax) || dataMin >= dataMax) {
                outMin = defaultMin;
                outMax = defaultMax;
                return;
            }
            double span = dataMax - dataMin;
            if (span <= 1e-9) {
                span = (std::abs(dataMax) > 1e-9) ? std::abs(dataMax) * 0.2 : 1.0;
            }
            const double pad = span * 0.10;
            outMin = dataMin - pad;
            outMax = dataMax + pad;
        }

        static void configureChartAxes(ZoomAndScroll *view, const QString &xTitle, const QString &yTitle) {
            if (!view || !view->chart()) return;

            for (auto *axis: view->chart()->axes()) {
                view->chart()->removeAxis(axis);
                delete axis;
            }

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

            view->xMin = view->minX;
            view->xMax = view->maxX;
            view->yMin = view->minY;
            view->yMax = view->maxY;

            QLinearGradient gradient(0, 0, 0, 700);
            gradient.setColorAt(0.0, QColor(255, 255, 255));
            gradient.setColorAt(0.5, QColor(235, 250, 255));
            gradient.setColorAt(1.0, QColor(162, 210, 232));
            view->chart()->setBackgroundBrush(QBrush(gradient));
        }
    } // namespace config

    void CalculatePlotHelper::initEmptyPlot(ZoomAndScroll *view,
                                            const QString &xTitle,
                                            const QString &yTitle,
                                            const double minX,
                                            const double maxX,
                                            const double minY,
                                            const double maxY) {
        if (!view || !view->chart()) return;
        view->chart()->removeAllSeries();
        view->minX = minX;
        view->maxX = maxX;
        view->minY = minY;
        view->maxY = maxY;
        view->xMin = minX;
        view->xMax = maxX;
        view->yMin = minY;
        view->yMax = maxY;
        config::configureChartAxes(view, xTitle, yTitle);
        view->chart()->legend()->setVisible(false);
    }

    void ContactPointsPlot::applyPlot(ZoomAndScroll *view,
                                      const std::vector<double> &y1,
                                      const std::vector<double> &y2,
                                      const std::vector<double> &zp1,
                                      const std::vector<double> &zp2,
                                      const std::vector<std::pair<double, double> > &contactLines,
                                      const int sideSign,
                                      const QString &chartTitle) {
        if (!view || !view->chart()) return;
        view->chart()->removeAllSeries();

        std::vector<double> y_1n(y1.size());
        std::vector<double> y_2n(y2.size());
        for (size_t i = 0; i < y1.size(); ++i) y_1n[i] = -1.0 * y1[i];
        for (size_t i = 0; i < y2.size(); ++i) y_2n[i] = -1.0 * y2[i];

        const std::vector<double> &y1p = (sideSign < 0) ? y_1n : y1;
        const std::vector<double> &y2p = (sideSign < 0) ? y_2n : y2;

        // Contact vector lines (without tracking label)
        for (size_t i = 0; i + 1 < contactLines.size(); i += 2) {
            auto *contactLineSeries = new QLineSeries();
            const double x1 = sideSign * contactLines[i].first;
            const double y1_val = contactLines[i].second;
            const double x2 = contactLines[i + 1].first;
            const double y2_val = contactLines[i + 1].second;

            QList<QPointF> linePoints;
            linePoints.reserve(2);
            linePoints.append(QPointF(x1, y1_val));
            linePoints.append(QPointF(x2, y2_val));
            contactLineSeries->replace(linePoints);

            QPen linePen(Qt::blue);
            linePen.setWidth(1);
            contactLineSeries->setPen(linePen);
            view->chart()->addSeries(contactLineSeries);
        }

        // Rail profile (Red curve)
        auto *railSeries = new QLineSeries();
        railSeries->setName("Rail Profile");
        const size_t railCount = std::min(y1p.size(), zp1.size());
        QList<QPointF> railPoints;
        railPoints.reserve(static_cast<qsizetype>(railCount));
        for (size_t i = 0; i < railCount; ++i) {
            railPoints.append(QPointF(y1p[i], zp1[i]));
        }
        railSeries->replace(railPoints);
        QPen railPen(Qt::red);
        railPen.setWidth(2);
        railPen.setStyle(Qt::SolidLine);
        railSeries->setPen(railPen);
        view->chart()->addSeries(railSeries);

        // Wheel profile (Black curve, shifted up by +10 mm to match physical contact location)
        auto *wheelSeries = new QLineSeries();
        wheelSeries->setName("Wheel Profile");
        const size_t wheelCount = std::min(y2p.size(), zp2.size());
        QList<QPointF> wheelPoints;
        wheelPoints.reserve(static_cast<qsizetype>(wheelCount));
        for (size_t i = 0; i < wheelCount; ++i) {
            wheelPoints.append(QPointF(y2p[i], zp2[i] + 10.0));
        }
        wheelSeries->replace(wheelPoints);
        QPen wheelPen(Qt::black);
        wheelPen.setWidth(2);
        wheelPen.setStyle(Qt::SolidLine);
        wheelSeries->setPen(wheelPen);
        view->chart()->addSeries(wheelSeries);

        view->updateXLimits(view->chart());
        // Auto-range with 10% padding
        const double dataMinX = view->minX;
        const double dataMaxX = view->maxX;
        const double dataMinY = view->minY;
        const double dataMaxY = view->maxY;
        config::computePaddedRange(dataMinX, dataMaxX, view->minX, view->maxX, -750.0, 750.0);
        config::computePaddedRange(dataMinY, dataMaxY, view->minY, view->maxY, -50.0, 50.0);

        config::configureChartAxes(view, "Transverse Direction [mm]", "Vertical Distance (Height) [mm]");
        view->chart()->legend()->setVisible(false);
    }

    void CalculatedEqConPlot::applyPlot(ZoomAndScroll *view,
                                        const std::vector<double> &ycp,
                                        const std::vector<double> &ang,
                                        const bool isSymmetric) {
        if (!view || !view->chart()) return;
        view->chart()->removeAllSeries();

        // Filter input data strictly to [0, 8] mm
        std::vector<double> filtX;
        std::vector<double> filtY;
        for (size_t i = 0; i < ycp.size() && i < ang.size(); ++i) {
            if (ycp[i] >= 0.0 && ycp[i] <= 8.0 + 1e-6) {
                filtX.push_back(ycp[i]);
                filtY.push_back(ang[i]);
            }
        }
        if (filtX.empty()) {
            filtX = ycp;
            filtY = ang;
        }

        std::vector<double> finalX = filtX;
        std::vector<double> finalY = filtY;

        if (isSymmetric && filtX.size() >= 2) {
            try {
                const Pchip spline(filtX, filtY);
                const double maxInterp = std::min(8.0, filtX.back());
                const auto filtX_ = static_cast<long double>(filtX.front());
                const auto maxInterp_ = static_cast<long double>(maxInterp);
                finalX = Linspace(filtX_, maxInterp_, 0.01).linspacec();
                finalY.clear();
                finalY.reserve(finalX.size());
                for (double x: finalX) {
                    finalY.push_back(spline.interpolate(x));
                }
            } catch (...) {
                finalX = filtX;
                finalY = filtY;
            }
        }

        auto *calcSeries = new ScatterSeries(view);
        calcSeries->setName("Nominal Equivalent Conicity");
        QList<QPointF> points;
        points.reserve(static_cast<qsizetype>(std::min(finalX.size(), finalY.size())));
        for (size_t i = 0; i < finalX.size() && i < finalY.size(); ++i) {
            if (finalX[i] >= 0.0 && finalX[i] <= 8.0 + 1e-6) {
                points.append(QPointF(finalX[i], finalY[i]));
            }
        }
        calcSeries->replace(points);

        calcSeries->setPen(QPen(Qt::blue));
        calcSeries->setMarkerShape(QScatterSeries::MarkerShapeStar);
        calcSeries->setMarkerSize(8);
        calcSeries->setPointLabelsFormat("@xPoint, @yPoint");
        view->chart()->addSeries(calcSeries);

        view->updateXLimits(view->chart());
        const double dataMinY = view->minY;
        const double dataMaxY = view->maxY;

        view->minX = 0.0;
        view->maxX = 8.0;
        config::computePaddedRange(dataMinY, dataMaxY, view->minY, view->maxY, 0.0, 0.6);

        config::configureChartAxes(view, "Lateral Wheelset Displacement [mm]",
                                   "Nominal Equivalent Conicity [tan(\u03bb)]");
        view->chart()->legend()->setVisible(false);
    }

    void CalculatedDeltaRPlot::applyPlot(ZoomAndScroll *view,
                                         const std::vector<double> &ys,
                                         const std::vector<double> &drf) {
        if (!view || !view->chart()) return;
        view->chart()->removeAllSeries();

        const size_t count = std::min(ys.size(), drf.size());
        QList<QPointF> points;
        points.reserve(static_cast<qsizetype>(count));
        for (size_t i = 0; i < count; ++i) {
            if (ys[i] >= -8.0 - 1e-6 && ys[i] <= 8.0 + 1e-6) {
                points.append(QPointF(ys[i], drf[i]));
            }
        }

        auto *calcScatter = new ScatterSeries(view);
        calcScatter->setName("Calculated \u0394r Points");
        calcScatter->replace(points);
        calcScatter->setColor(Qt::darkGreen);
        calcScatter->setMarkerShape(QScatterSeries::MarkerShapeStar);
        calcScatter->setMarkerSize(12);
        calcScatter->setPointLabelsFormat("@xPoint, @yPoint");
        view->chart()->addSeries(calcScatter);

        auto *calcLine = new LineSeries(view);
        calcLine->setName("Calculated \u0394r Function");
        calcLine->replace(points);
        QPen pen(Qt::red);
        pen.setWidth(2);
        pen.setStyle(Qt::DashLine);
        calcLine->setPen(pen);
        calcLine->setPointLabelsFormat("@xPoint, @yPoint");
        view->chart()->addSeries(calcLine);

        view->updateXLimits(view->chart());
        const double dataMinY = view->minY;
        const double dataMaxY = view->maxY;

        view->minX = -8.0;
        view->maxX = 8.0;
        config::computePaddedRange(dataMinY, dataMaxY, view->minY, view->maxY, -8.0, 8.0);

        config::configureChartAxes(view, "Lateral Wheelset Displacement y [mm]", "Right-Left Wheelset \u0394r [mm]");
        view->chart()->legend()->setVisible(false);
    }

    void CalculatedAnglePlot::applyPlot(ZoomAndScroll *view,
                                        const std::vector<double> &ys,
                                        const std::vector<double> &rad) {
        if (!view || !view->chart()) return;
        view->chart()->removeAllSeries();

        const size_t count = std::min(ys.size(), rad.size());
        QList<QPointF> points;
        points.reserve(static_cast<qsizetype>(count));
        for (size_t i = 0; i < count; ++i) {
            if (ys[i] >= -8.0 - 1e-6 && ys[i] <= 8.0 + 1e-6) {
                points.append(QPointF(ys[i], rad[i]));
            }
        }

        auto *calcScatter = new ScatterSeries(view);
        calcScatter->setName("Calculated Angle Points");
        calcScatter->replace(points);
        calcScatter->setColor(Qt::darkBlue);
        calcScatter->setMarkerShape(QScatterSeries::MarkerShapeStar);
        calcScatter->setMarkerSize(12);
        calcScatter->setPointLabelsFormat("@xPoint, @yPoint");
        view->chart()->addSeries(calcScatter);

        auto *calcLine = new LineSeries(view);
        calcLine->setName("Calculated Angle Function");
        calcLine->replace(points);
        QPen pen(Qt::magenta);
        pen.setWidth(2);
        pen.setStyle(Qt::DashLine);
        calcLine->setPen(pen);
        calcLine->setPointLabelsFormat("@xPoint, @yPoint");
        view->chart()->addSeries(calcLine);

        view->updateXLimits(view->chart());
        const double dataMinY = view->minY;
        const double dataMaxY = view->maxY;

        view->minX = -8.0;
        view->maxX = 8.0;
        config::computePaddedRange(dataMinY, dataMaxY, view->minY, view->maxY, -0.5, 0.5);

        config::configureChartAxes(view, "Lateral Wheelset Displacement y [mm]",
                                   "Effective Contact Angle [tan(\u03b3)]");
        view->chart()->legend()->setVisible(false);
    }
} // namespace WRCC::UI
