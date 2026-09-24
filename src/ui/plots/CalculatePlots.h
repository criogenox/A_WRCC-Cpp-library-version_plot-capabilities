#pragma once

#include <QObject>
#include <string>
#include <utility>
#include <vector>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>

#include "customEvents.h"

namespace WRCC::UI {
    /**
     * @brief Contact Profile Points Plot for Wheel-Rail pair in Track Coordinate System
     * Range -13:0.1:13 mm, tracking label disabled, zoom/pan/clipping enabled.
     */
    class ContactPointsPlot : public QObject {
        Q_OBJECT

    public:
        static void applyPlot(ZoomAndScroll *view,
                              const std::vector<double> &y1,
                              const std::vector<double> &y2,
                              const std::vector<double> &zp1,
                              const std::vector<double> &zp2,
                              const std::vector<std::pair<double, double> > &contactLines,
                              int sideSign, // +1 for Right, -1 for Left
                              const QString &chartTitle);
    };

    /**
     * @brief Equivalent Conicity Plot for generic calculation
     */
    class CalculatedEqConPlot : public QObject {
        Q_OBJECT

    public:
        static void applyPlot(ZoomAndScroll *view,
                              const std::vector<double> &ycp,
                              const std::vector<double> &ang,
                              bool isSymmetric = false);
    };

    /**
     * @brief Delta R (Rolling Radii Difference) Plot for generic calculation
     */
    class CalculatedDeltaRPlot : public QObject {
        Q_OBJECT

    public:
        static void applyPlot(ZoomAndScroll *view,
                              const std::vector<double> &ys,
                              const std::vector<double> &drf);
    };

    /**
     * @brief Contact Inclination Angle Plot for generic calculation
     */
    class CalculatedAnglePlot : public QObject {
        Q_OBJECT

    public:
        static void applyPlot(ZoomAndScroll *view,
                              const std::vector<double> &ys,
                              const std::vector<double> &rad);
    };

    /**
     * @brief Helper to initialize background chart with styled grid and axes before calculation
     */
    class CalculatePlotHelper : public QObject {
        Q_OBJECT

    public:
        static void initEmptyPlot(ZoomAndScroll *view,
                                  const QString &xTitle,
                                  const QString &yTitle,
                                  double minX,
                                  double maxX,
                                  double minY,
                                  double maxY);
    };
} // namespace WRCC::UI
