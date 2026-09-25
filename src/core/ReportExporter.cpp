#include "ReportExporter.h"

#include <cmath>
#include <limits>
#include <QBuffer>
#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

namespace WRCC::Core {
    static QString imageToBase64Src(const QImage &image) {
        if (image.isNull()) return "";
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        return QString("data:image/png;base64,%1").arg(QString::fromLatin1(byteArray.toBase64()));
    }

    std::vector<ConicityTableRow> ReportExporter::extractIntegerConicityTable(
        const std::vector<double> &ycp,
        const std::vector<double> &conicity) {
        std::vector<ConicityTableRow> table;
        if (ycp.empty() || conicity.empty()) return table;

        for (int targetY = 1; targetY <= 7; ++targetY) {
            const auto target = static_cast<double>(targetY);
            size_t bestIdx = 0;
            double minDiff = std::numeric_limits<double>::max();

            for (size_t i = 0; i < ycp.size() && i < conicity.size(); ++i) {
                double diff = std::abs(ycp[i] - target);
                if (diff < minDiff) {
                    minDiff = diff;
                    bestIdx = i;
                }
            }

            if (bestIdx < conicity.size()) {
                table.push_back({target, ycp[bestIdx], conicity[bestIdx]});
            }
        }
        return table;
    }

    static void saveFigureImages(const QString &filePath, const CalculationReportData &data) {
        const QFileInfo fi(filePath);
        const QString reportFiguresDir = fi.dir().filePath("figures");

        QList<QString> figureDirs;
        figureDirs.append(reportFiguresDir);

        if (QDir("docs/outputs/figures").exists()) {
            QString docsFigures = QDir("docs/outputs/figures").absolutePath();
            if (docsFigures != QDir(reportFiguresDir).absolutePath()) {
                figureDirs.append(docsFigures);
            }
        }

        for (const QString &dirPath: figureDirs) {
            QDir().mkpath(dirPath);
            if (!data.rightPairChart.isNull()) data.rightPairChart.save(dirPath + "/right_wheel_rail_pair.png", "PNG");
            if (!data.leftPairChart.isNull()) data.leftPairChart.save(dirPath + "/left_wheel_rail_pair.png", "PNG");
            if (!data.eqConChart.isNull()) data.eqConChart.save(dirPath + "/equivalent_conicity.png", "PNG");
            if (!data.deltaRChart.isNull()) data.deltaRChart.save(dirPath + "/rolling_radii_diff.png", "PNG");
            if (!data.contactAngleChart.isNull()) data.contactAngleChart.save(dirPath + "/contact_angles.png", "PNG");
        }
    }

    bool ReportExporter::exportToHtml(
        const QString &filePath,
        const CalculationReportData &data,
        const bool embedBase64,
        QString *errorMessage) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            if (errorMessage) *errorMessage = QString("Cannot write to file: %1").arg(filePath);
            return false;
        }

        const auto &p = data.params;
        const QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        const QString rightWheelName = p.rightWheelProfile.isEmpty()
                                           ? "R-UIC 519A"
                                           : QFileInfo(p.rightWheelProfile).fileName();
        const QString leftWheelName = p.leftWheelProfile.isEmpty()
                                          ? "R-UIC 519B"
                                          : QFileInfo(p.leftWheelProfile).fileName();
        const QString rightRailName = p.rightRailProfile.isEmpty()
                                          ? "S-UIC 519A"
                                          : QFileInfo(p.rightRailProfile).fileName();
        const QString leftRailName = p.leftRailProfile.isEmpty()
                                         ? "S-UIC 519A"
                                         : QFileInfo(p.leftRailProfile).fileName();

        QTextStream out(&file);

        out << "<!DOCTYPE html>\n"
                << "<html lang=\"en\">\n"
                << "<head>\n"
                << "<meta charset=\"UTF-8\">\n"
                << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                << "<title>Wheel / Rail Contact Characterization Program — Technical Report</title>\n"
                << "<style>\n"
                << "body {\n"
                << "  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Helvetica, Arial, sans-serif;\n"
                << "  font-size: 16px;\n"
                << "  line-height: 1.6;\n"
                << "  color: #24292e;\n"
                << "  background: #fff;\n"
                << "  padding: 45px;\n"
                << "  max-width: 900px;\n"
                << "  margin: 0 auto;\n"
                << "}\n"
                << "h1, h2, h3, h4, h5, h6 { margin-top: 24px; margin-bottom: 16px; font-weight: 600; line-height: 1.25; }\n"
                << "h1 { font-size: 2em; border-bottom: 1px solid #eaecef; padding-bottom: .3em; }\n"
                << "h2 { font-size: 1.5em; border-bottom: 1px solid #eaecef; padding-bottom: .3em; }\n"
                << "h3 { font-size: 1.25em; }\n"
                << "p { margin: 0 0 16px; }\n"
                << "a { color: #0366d6; text-decoration: none; }\n"
                << "a:hover { text-decoration: underline; }\n"
                << "code { padding: .2em .4em; font-size: 85%; background: rgba(27,31,35,.05); border-radius: 3px; font-family: 'SFMono-Regular', Consolas, 'Liberation Mono', Menlo, monospace; }\n"
                << "pre { padding: 16px; overflow: auto; font-size: 85%; line-height: 1.45; background: #f6f8fa; border-radius: 6px; }\n"
                << "pre code { display: inline; padding: 0; background: transparent; }\n"
                << "blockquote { padding: 0 1em; color: #6a737d; border-left: .25em solid #dfe2e5; margin: 0 0 16px; }\n"
                << "table { border-spacing: 0; border-collapse: collapse; margin: 0 0 16px; width: 100%; }\n"
                << "th, td { padding: 6px 13px; border: 1px solid #dfe2e5; }\n"
                << "th { font-weight: 600; background: #f6f8fa; }\n"
                << "tr:nth-child(2n) { background: #f6f8fa; }\n"
                << "ul, ol { margin: 0 0 16px; padding-left: 2em; }\n"
                << "li + li { margin-top: .25em; }\n"
                << "hr { height: .25em; padding: 0; margin: 24px 0; background: #e1e4e8; border: 0; }\n"
                << "img { max-width: 100%; }\n"
                << "input[type=\"checkbox\"] { margin-right: .5em; }\n"
                << "</style>\n"
                << "</head>\n"
                << "<body><h1>Wheel / Rail Contact Characterization Program</h1>\n"
                << "<blockquote>\n"
                << "<p><strong>Generic Contact Analysis Technical Report</strong><br />\n"
                << "<strong>Standard:</strong> EN 15302:2008+A1:2010<br />\n"
                << "<strong>Generated:</strong> " << currentDate << "</p>\n"
                << "</blockquote>\n"
                << "<hr />\n"
                << "<h2>1. Calculation &amp; Track Parameters</h2>\n";

        const QString opCondText = p.operatingConditions.trimmed().isEmpty()
                                       ? "-"
                                       : p.operatingConditions.trimmed().toHtmlEscaped();

        out << "<div align=\"center\">\n"
                << "<table style=\"border-collapse: collapse; margin: 0 auto;\">\n"
                << "  <thead>\n"
                << "    <tr>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\">Parameter</th>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">Right Side</th>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">Left Side</th>\n"
                << "    </tr>\n"
                << "  </thead>\n"
                << "  <tbody>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Wheel Profile Model</b></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\"><code>" <<
                rightWheelName << "</code></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\"><code>" <<
                leftWheelName << "</code></td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Wheel Rolling Radius (R₀)</b></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                QString::number(p.rightRadius, 'f', 1) << " mm</td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                QString::number(p.leftRadius, 'f', 1) << " mm</td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Rail Profile Model</b></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\"><code>" <<
                rightRailName << "</code></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\"><code>" <<
                leftRailName << "</code></td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Rail Initial Inclination</b></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">1 / " <<
                QString::number(p.rightRailInclin, 'f', 1) << "</td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">1 / " <<
                QString::number(p.leftRailInclin, 'f', 1) << "</td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Track Gauge</b></td>\n"
                << "      <td colspan=\"2\" style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                QString::number(p.trackGauge, 'f', 2) << " mm</td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Back-to-Back Distance</b></td>\n"
                << "      <td colspan=\"2\" style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                QString::number(p.backToBack, 'f', 1) << " mm</td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Operating Conditions</b></td>\n"
                << "      <td colspan=\"2\" style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                opCondText << "</td>\n"
                << "    </tr>\n"
                << "  </tbody>\n"
                << "</table>\n"
                << "</div>\n"
                << "<h2>2. Equivalent Conicity Analysis Results (EN 15302)</h2>\n"
                << "<p><em>Method: Harmonic linearization &amp; integration of non-linear differential equation.</em><br />\n"
                << "<em>Note: Standard nominal reference point assessed at <strong>y = 3.0 mm</strong>.</em></p>\n"
                << "<div align=\"center\">\n"
                << "<table style=\"border-collapse: collapse; margin: 0 auto; text-align: center;\">\n"
                << "  <thead>\n"
                << "    <tr>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px 16px; text-align: center;\">Lateral Displacement y [mm]</th>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px 16px; text-align: center;\">Equivalent Conicity tan(&gamma;<sub>c</sub>)</th>\n"
                << "    </tr>\n"
                << "  </thead>\n"
                << "  <tbody>\n";

        for (const auto &row: data.conicityTable) {
            bool is3mm = (std::abs(row.targetDisplacement - 3.0) < 1e-4);
            if (is3mm) {
                out << "    <tr style=\"font-weight: bold;\">\n"
                        << "      <td style=\"border: 3px solid #0284c7; padding: 8px 16px; text-align: center; font-weight: bold;\">"
                        << QString::number(row.targetDisplacement, 'f', 1) << " mm</td>\n"
                        << "      <td style=\"border: 3px solid #0284c7; padding: 8px 16px; text-align: center; font-weight: bold;\">"
                        << QString::number(row.equivalentConicity, 'f', 4) << "</td>\n"
                        << "    </tr>\n";
            } else {
                out << "    <tr>\n"
                        << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px 16px; text-align: center;\">"
                        << QString::number(row.targetDisplacement, 'f', 1) << " mm</td>\n"
                        << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px 16px; text-align: center;\">"
                        << QString::number(row.equivalentConicity, 'f', 4) << "</td>\n"
                        << "    </tr>\n";
            }
        }

        out << "  </tbody>\n"
                << "</table>\n"
                << "</div>\n"
                << "<h2>3. Characteristic Visualizations</h2>\n";

        // Also save PNG copies into docs/outputs/figures/ directory
        saveFigureImages(filePath, data);

        if (embedBase64) {
            if (!data.rightPairChart.isNull()) {
                out << "<div align=\"center\">\n"
                        << "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Right Wheel-Rail Pair Contact Points</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.rightPairChart) << "\" alt=\"" << rightWheelName <<
                        " - " << rightRailName <<
                        "\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n";
            }
            if (!data.leftPairChart.isNull()) {
                out << "<div align=\"center\">\n"
                        << "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Left Wheel-Rail Pair Contact Points</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.leftPairChart) << "\" alt=\"" << leftWheelName <<
                        " - " << leftRailName <<
                        "\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n";
            }
            if (!data.eqConChart.isNull()) {
                out << "<div align=\"center\">\n"
                        << "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Equivalent Conicity Function tan(&gamma;<sub>c</sub>)</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.eqConChart) <<
                        "\" alt=\"Equivalent Conicity\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n";
            }
            if (!data.deltaRChart.isNull()) {
                out << "<div align=\"center\">\n"
                        << "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Rolling Radii Difference Function &Delta;r(y)</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.deltaRChart) <<
                        "\" alt=\"Rolling Radii Difference\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n";
            }
            if (!data.contactAngleChart.isNull()) {
                out << "<div align=\"center\">\n"
                        <<
                        "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Effective Contact Angle Function tan(&gamma;<sub>a</sub>)</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.contactAngleChart) <<
                        "\" alt=\"Contact Angle\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n";
            }
        } else {
            out << "<p><em>Images omitted in compact mode.</em></p>\n";
        }

        out << "<hr />\n"
                << "<p><em>Generated automatically by WRCC (Wheel-Rail Contact Characterization Program) v2.0.</em></p>\n"
                << "</body>\n"
                << "</html>\n";

        file.close();
        return true;
    }

    bool ReportExporter::exportToMarkdown(
        const QString &filePath,
        const CalculationReportData &data,
        const bool embedBase64,
        QString *errorMessage) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            if (errorMessage) *errorMessage = QString("Cannot write to file: %1").arg(filePath);
            return false;
        }

        const auto &p = data.params;
        const QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        const QString rightWheelName = p.rightWheelProfile.isEmpty()
                                           ? "R-UIC 519A"
                                           : QFileInfo(p.rightWheelProfile).fileName();
        const QString leftWheelName = p.leftWheelProfile.isEmpty()
                                          ? "R-UIC 519B"
                                          : QFileInfo(p.leftWheelProfile).fileName();
        const QString rightRailName = p.rightRailProfile.isEmpty()
                                          ? "S-UIC 519A"
                                          : QFileInfo(p.rightRailProfile).fileName();
        const QString leftRailName = p.leftRailProfile.isEmpty()
                                         ? "S-UIC 519A"
                                         : QFileInfo(p.leftRailProfile).fileName();

        QTextStream out(&file);

        out << "# Wheel / Rail Contact Characterization Program\n\n";
        out << "> **Generic Contact Analysis Technical Report**  \n";
        out << "> **Standard:** EN 15302:2008+A1:2010  \n";
        out << "> **Generated:** " << currentDate << "  \n\n";

        out << "---\n\n";

        out << "## 1. Calculation & Track Parameters\n\n";

        const QString opCondText = p.operatingConditions.trimmed().isEmpty()
                                       ? "-"
                                       : p.operatingConditions.trimmed().toHtmlEscaped();

        out << "<div align=\"center\">\n"
                << "<table style=\"border-collapse: collapse; margin: 0 auto;\">\n"
                << "  <thead>\n"
                << "    <tr>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\">Parameter</th>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">Right Side</th>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">Left Side</th>\n"
                << "    </tr>\n"
                << "  </thead>\n"
                << "  <tbody>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Wheel Profile Model</b></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\"><code>" <<
                rightWheelName << "</code></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\"><code>" <<
                leftWheelName << "</code></td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Wheel Rolling Radius (R₀)</b></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                QString::number(p.rightRadius, 'f', 1) << " mm</td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                QString::number(p.leftRadius, 'f', 1) << " mm</td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Rail Profile Model</b></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\"><code>" <<
                rightRailName << "</code></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\"><code>" <<
                leftRailName << "</code></td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Rail Initial Inclination</b></td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">1 / " <<
                QString::number(p.rightRailInclin, 'f', 1) << "</td>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">1 / " <<
                QString::number(p.leftRailInclin, 'f', 1) << "</td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Track Gauge</b></td>\n"
                << "      <td colspan=\"2\" style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                QString::number(p.trackGauge, 'f', 2) << " mm</td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Back-to-Back Distance</b></td>\n"
                << "      <td colspan=\"2\" style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                QString::number(p.backToBack, 'f', 1) << " mm</td>\n"
                << "    </tr>\n"
                << "    <tr>\n"
                << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: left;\"><b>Operating Conditions</b></td>\n"
                << "      <td colspan=\"2\" style=\"border: 1px solid #cbd5e1; padding: 8px; text-align: center;\">" <<
                opCondText << "</td>\n"
                << "    </tr>\n"
                << "  </tbody>\n"
                << "</table>\n"
                << "</div>\n\n";

        out << "## 2. Equivalent Conicity Analysis Results (EN 15302)\n\n";
        out << "*Method: Harmonic linearization & integration of non-linear differential equation.*  \n";
        out << "*Note: Standard nominal reference point assessed at **y = 3.0 mm**.*\n\n";

        out << "<div align=\"center\">\n"
                << "<table style=\"border-collapse: collapse; margin: 0 auto; text-align: center;\">\n"
                << "  <thead>\n"
                << "    <tr>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px 16px; text-align: center;\">Lateral Displacement y [mm]</th>\n"
                << "      <th style=\"border: 1px solid #cbd5e1; padding: 8px 16px; text-align: center;\">Equivalent Conicity tan(&gamma;<sub>c</sub>)</th>\n"
                << "    </tr>\n"
                << "  </thead>\n"
                << "  <tbody>\n";

        for (const auto &row: data.conicityTable) {
            bool is3mm = (std::abs(row.targetDisplacement - 3.0) < 1e-4);
            if (is3mm) {
                out << "    <tr style=\"font-weight: bold;\">\n"
                        << "      <td style=\"border: 3px solid #0284c7; padding: 8px 16px; text-align: center; font-weight: bold;\">"
                        << QString::number(row.targetDisplacement, 'f', 1) << " mm</td>\n"
                        << "      <td style=\"border: 3px solid #0284c7; padding: 8px 16px; text-align: center; font-weight: bold;\">"
                        << QString::number(row.equivalentConicity, 'f', 4) << "</td>\n"
                        << "    </tr>\n";
            } else {
                out << "    <tr>\n"
                        << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px 16px; text-align: center;\">"
                        << QString::number(row.targetDisplacement, 'f', 1) << " mm</td>\n"
                        << "      <td style=\"border: 1px solid #cbd5e1; padding: 8px 16px; text-align: center;\">"
                        << QString::number(row.equivalentConicity, 'f', 4) << "</td>\n"
                        << "    </tr>\n";
            }
        }

        out << "  </tbody>\n"
                << "</table>\n"
                << "</div>\n\n";

        out << "## 3. Characteristic Visualizations\n\n";

        // Also save PNG copies into docs/figures/ directory
        saveFigureImages(filePath, data);

        if (embedBase64) {
            if (!data.rightPairChart.isNull()) {
                out << "<div align=\"center\">\n"
                        << "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Right Wheel-Rail Pair Contact Points</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.rightPairChart) << "\" alt=\"" << rightWheelName <<
                        " - " << rightRailName <<
                        "\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n\n";
            }
            if (!data.leftPairChart.isNull()) {
                out << "<div align=\"center\">\n"
                        << "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Left Wheel-Rail Pair Contact Points</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.leftPairChart) << "\" alt=\"" << leftWheelName <<
                        " - " << leftRailName <<
                        "\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n\n";
            }
            if (!data.eqConChart.isNull()) {
                out << "<div align=\"center\">\n"
                        << "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Equivalent Conicity Function tan(&gamma;<sub>c</sub>)</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.eqConChart) <<
                        "\" alt=\"Equivalent Conicity\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n\n";
            }
            if (!data.deltaRChart.isNull()) {
                out << "<div align=\"center\">\n"
                        << "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Rolling Radii Difference Function &Delta;r(y)</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.deltaRChart) <<
                        "\" alt=\"Rolling Radii Difference\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n\n";
            }
            if (!data.contactAngleChart.isNull()) {
                out << "<div align=\"center\">\n"
                        <<
                        "  <h3 style=\"text-align: center; color: #0f172a; margin-top: 16px; margin-bottom: 8px;\">Effective Contact Angle Function tan(&gamma;<sub>a</sub>)</h3>\n"
                        << "  <img src=\"" << imageToBase64Src(data.contactAngleChart) <<
                        "\" alt=\"Contact Angle\" width=\"90%\" style=\"max-width: 850px; height: auto; border: 1px solid #e2e8f0; border-radius: 6px;\"/>\n"
                        << "</div>\n\n";
            }
        } else {
            out << "*Images omitted in compact text mode.*\n\n";
        }

        out << "---\n";
        out << "*Generated automatically by WRCC (Wheel-Rail Contact Characterization Program) v2.0.*\n";

        file.close();
        return true;
    }
} // namespace WRCC::Core
