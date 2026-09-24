#pragma once

#include <QMainWindow>
#include <string>
#include <vector>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>

#include "customEvents.h"
#include "wrcc/core/conicity.h"
#include "wrcc/core/pchip.h"
#include "wrcc/models/input.h"

namespace WRCC::UI {
    /**
     * @brief Benchmark Equivalent Conicity Plot
     * Plots calculated equivalent conicity with tracking enabled, alongside standard tolerance curves (without tracking).
     */
    class BenchmarkEqConPlot : public QObject {
        Q_OBJECT

    public:
        explicit BenchmarkEqConPlot(ZoomAndScroll *view,
                                    const std::vector<double> &calcX,
                                    const std::vector<double> &calcY,
                                    const std::string &benchDataFile,
                                    bool isSymmetric = false,
                                    QObject *parent = nullptr);

        static void applyPlot(ZoomAndScroll *view,
                              const std::vector<double> &calcX,
                              const std::vector<double> &calcY,
                              const std::string &benchDataFile,
                              bool isSymmetric = false);
    };

    /**
     * @brief Benchmark Delta R Difference Plot
     * Plots calculated Delta R alongside standard reference curve.
     */
    class BenchmarkDeltaRPlot : public QObject {
        Q_OBJECT

    public:
        explicit BenchmarkDeltaRPlot(ZoomAndScroll *view,
                                     const std::vector<double> &ys,
                                     const std::vector<double> &calculatedDr,
                                     const std::string &referenceDrFile,
                                     QObject *parent = nullptr);

        static void applyPlot(ZoomAndScroll *view,
                              const std::vector<double> &ys,
                              const std::vector<double> &calculatedDr,
                              const std::string &referenceDrFile);
    };
} // namespace WRCC::UI
