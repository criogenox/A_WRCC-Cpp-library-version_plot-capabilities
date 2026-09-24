#pragma once

#include <QImage>
#include <QString>
#include <vector>

namespace WRCC::Core {
    struct CalculationParameters {
        double rightRadius{920.0};
        double leftRadius{920.0};
        double rightRailInclin{40.0};
        double leftRailInclin{40.0};
        double trackGauge{1435.16};
        double backToBack{1360.0};
        bool isAsymmetric{false};
        QString rightWheelProfile;
        QString leftWheelProfile;
        QString rightRailProfile;
        QString leftRailProfile;
        QString surfaceCondition{"Dry surface"};
        QString additionalInfo{"New profiles"};
        QString operatingConditions; // Custom commentaries entered by user in print dialog
    };

    struct ConicityTableRow {
        double targetDisplacement{0.0}; // 1.0, 2.0, ..., 7.0
        double actualDisplacement{0.0}; // Closest matching actual displacement
        double equivalentConicity{0.0}; // tan(γ_c)
    };

    struct CalculationReportData {
        CalculationParameters params;
        std::vector<ConicityTableRow> conicityTable;

        // High-resolution chart snapshots
        QImage rightPairChart;
        QImage leftPairChart;
        QImage eqConChart;
        QImage deltaRChart;
        QImage contactAngleChart;
    };

    class ReportExporter {
    public:
        static std::vector<ConicityTableRow> extractIntegerConicityTable(
            const std::vector<double> &ycp,
            const std::vector<double> &conicity
        );

        static bool exportToHtml(
            const QString &filePath,
            const CalculationReportData &data,
            bool embedBase64 = true,
            QString *errorMessage = nullptr
        );

        static bool exportToMarkdown(
            const QString &filePath,
            const CalculationReportData &data,
            bool embedBase64 = true,
            QString *errorMessage = nullptr
        );
    };
} // namespace WRCC::Core
