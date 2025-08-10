#include <regex>

#include "matplotlibcpp.h"
#include "input.h"
#include "pchip.h"

namespace plt = matplotlibcpp;
using std::vector;
using std::string;

class Plot {
public:
    Plot(const string &bench, const int &c, const vector<double> &ycp, const vector<double> &ang)
        : bench(&bench), c(&c), ycp(&ycp), ang(&ang) {
        if (c == 1) {
            Pchip splinep(ycp, ang);
            const vector<double> ycpc = Linspace(ycp[0], ycp[ycp.size() - 1], 0.01).linspacec();
            vector<double> angpc;
            angpc.reserve(ycpc.size());
            std::ranges::transform(cbegin(ycpc), cend(ycpc), std::back_insert_iterator(angpc),
                                   [&splinep](const double &yi_i) {
                                       return splinep.interpolate(yi_i);
                                   });
            plotc(ycpc, angpc);
        } else {
            plotc(ycp, ang);
        }
    }

    Plot(const vector<double> &y_1, const vector<double> &y_2,
         const vector<double> &zp_1, const vector<double> &zp2d, const vector<double> &zp2i,
         const vector<std::pair<double, double> > &linr, const vector<std::pair<double, double> > &linl,
         const string &wd, const string &wi, const string &r) {
        plotcp(y_1, zp_1, y_2, zp2d, linr, wd, r, 1);
        plotcp(y_1, zp_1, y_2, zp2i, linl, wi, r, -1);
    }

    Plot(const vector<double> &ys, const vector<double> &dr, const vector<double> &drs, const vector<double> &rad)
        : ys(&ys), dr(&dr), drs(&drs), rad(&rad) {
        plotdr();
        plotrad();
    }

    ~Plot() {
        bench = nullptr;
        c = nullptr;
        ycp = nullptr;
        ang = nullptr;
        ys = nullptr;
        dr = nullptr;
        drs = nullptr;
        rad = nullptr;
    }

    static void show() {
        plt::show();
    }

private:
    void plotcp(const vector<double> &y1, const vector<double> &zp1, const vector<double> &y2,
                const vector<double> &zp2, const vector<std::pair<double, double> > &lin, const string &w,
                const string &r, const int &c) const {
        //*******************  Plotting properties  ********************
        //##############################################################
        plt::figure_size(1366, 768);
        // -------------------------------------------------------------
        vector<double> y_1n;
        std::ranges::transform(cbegin(y1), cend(y1), std::back_insert_iterator(y_1n),
                               [](const double &yi_i) {
                                   return -1 * yi_i;
                               });
        vector<double> y_2n;
        std::ranges::transform(cbegin(y2), cend(y2), std::back_insert_iterator(y_2n),
                               [](const double &yi_i) {
                                   return -1 * yi_i;
                               });
        vector<double> y1p, y2p;
        string pos;
        if (c < 0) {
            y1p = y_1n;
            y2p = y_2n;
            pos.insert(0, "lower left");
        } else {
            y1p = y1;
            y2p = y2;
            pos.insert(0, "lower right");
        }
        // -------------------------------------------------------------
        bool first_line = true;
        for (size_t i = 0; i < lin.size(); i += 2) {
            const double x11 = c * lin[i].first; // x1
            const double y11 = lin[i].second; // y1
            const double x22 = lin[i + 1].first; // x2
            const double y22 = lin[i + 1].second; // y2

            std::vector<double> x = {x11, x22};
            std::vector<double> y = {y11, y22};

            if (first_line) {
                plt::plot(x, y, {
                              {"label", "Potential Contact Pairs"},
                              {"color", "blue"}, {"linestyle", "solid"}
                          });
                first_line = false; // Set the flag to false after the first iteration
            } else {
                plt::plot(x, y, "-b");
            }
        }
        // -----------------
        const std::regex pattern(R"(/([^/]+)\.)");
        std::smatch match;
        std::regex_search(r, match, pattern);
        string rail = "Rail Type: ";
        rail.insert(11, match[1]);
        // -----------------
        plt::plot(y1p, zp1, {
                      {"label", rail},
                      {"color", "black"}, {"linestyle", "solid"}
                  });
        // -----------------
        std::regex_search(w, match, pattern);
        string wheel = "Wheel Type: ";
        wheel.insert(12, match[1]);
        // -----------------
        plt::plot(y2p, zp2, {
                      {"label", wheel},
                      {"color", "red"}, {"linestyle", "solid"}
                  });
        // -------------------------------------------------------------
        plt::legend({{"loc", pos}});
        // -------------------------------------------------------------
        plt::ylim(-20, 20);
        // -------------------------------------------------------------
        // # Add graph title
        plt::title("Right Wheel-Rail / Track Coordinate System");
        // -------------------------------------------------------------
        plt::xlabel("Transverse Direction [mm]");
        plt::ylabel("Vertical Distance (Height) [mm]");
        // -------------------------------------------------------------
        plt::grid(true);
        // -------------------------------------------------------------
        plt::tight_layout();
        // -------------------------------------------------------------
        //##############################################################
    }

    void plotc(const vector<double> &con_x, const vector<double> &con_y) const {
        const DataReader reader_col(*bench);
        // -----------------
        const std::regex pattern(R"(E(\d+))");
        std::smatch match;
        std::regex_search(*bench, match, pattern);
        string calculated = "Calculated Conicity - Case E";
        calculated.insert(28, match[1].str());
        // -----------------
        const std::vector<double> &x = reader_col.getColumn(0);
        const std::vector<double> &con_ymax = reader_col.getColumn(2);
        const std::vector<double> &con_ymin = reader_col.getColumn(3);
        const std::vector<double> &con_ymaxex = reader_col.getColumn(4);
        const std::vector<double> &con_yminex = reader_col.getColumn(5);
        //*******************  Plotting properties  ********************
        //##############################################################
        plt::figure_size(1366, 768);
        // -------------------------------------------------------------
        plt::plot(con_x, con_y, {
                      {"label", calculated}, {"color", "red"},
                      {"linestyle", " "}, {"marker", "*"}, {"markersize", "3"}
                  });
        plt::plot(x, con_ymax, {
                      {"label", "Upper Level Tolerance"},
                      {"color", "blue"}, {"linestyle", "solid"}
                  });
        plt::plot(x, con_ymin, {
                      {"label", "Lower Level Tolerance"},
                      {"color", "green"}, {"linestyle", "solid"}
                  });
        plt::plot(x, con_ymaxex, {
                      {"label", "Max Supplementary Tolerance"},
                      {"color", "blue"}, {"linestyle", "dashed"}
                  });
        plt::plot(x, con_yminex, {
                      {"label", "Min Supplementary Tolerance"},
                      {"color", "green"}, {"linestyle", "dashed"}
                  });
        // -------------------------------------------------------------
        plt::legend({{"loc", "lower right"}});
        // -------------------------------------------------------------
        // plt::xlim(0, 10);
        // -------------------------------------------------------------
        plt::title("Equivalent Conicity Calculation "
            "(according to the EN15302 non-linear method)");
        // -------------------------------------------------------------
        plt::xlabel("Lateral Wheelset Displacement [mm]");
        plt::ylabel("Nominal Equivalent Conicity [tan(λ)]");
        // -------------------------------------------------------------
        plt::grid(true);
        // -------------------------------------------------------------
        plt::tight_layout();
        // -------------------------------------------------------------
        //##############################################################
    }

    void plotdr() const {
        // const ReadCol reader_col(bench);
        //*******************  Plotting properties  ********************
        //##############################################################
        plt::figure_size(1366, 768);
        // -------------------------------------------------------------
        plt::plot(*ys, *dr, {
                      {"label", "Calculated ∆r function"}, {"color", "red"},
                      {"linestyle", " "}, {"marker", "s"}, {"markersize", "5"}
                  });
        plt::plot(*ys, *drs, {
                      {"label", "EN15302 ∆r function"}, {"color", "blue"},
                      {"linestyle", " "}, {"marker", "*"}, {"markersize", "5"}
                  });
        // -------------------------------------------------------------
        plt::legend({{"loc", "lower right"}});
        // -------------------------------------------------------------
        // plt::ylim(-30, 20);
        // -------------------------------------------------------------
        plt::title("Wheel's Rolling Radii Difference");
        // -------------------------------------------------------------
        plt::xlabel("Lateral Wheelset Displacement y [mm]");
        plt::ylabel("Right-Left Wheelset ∆r [mm]");
        // -------------------------------------------------------------
        plt::grid(true);
        // -------------------------------------------------------------
        plt::tight_layout();
        // -------------------------------------------------------------
        //##############################################################
    }

    void plotrad() const {
        // const ReadCol reader_col(bench);
        //*******************  Plotting properties  ********************
        //##############################################################
        plt::figure_size(1366, 768);
        // -------------------------------------------------------------
        plt::plot(*ys, *rad, {
                      {"label", "Calculated angle"}, {"color", "blue"},
                      {"linestyle", "--"}, {"marker", "s"}, {"markersize", "5"}
                  });
        // -------------------------------------------------------------
        plt::legend({{"loc", "lower right"}});
        // -------------------------------------------------------------
        // plt::ylim(-30, 20);
        // -------------------------------------------------------------
        plt::title("Wheel's Rolling Radii Difference");
        // -------------------------------------------------------------
        plt::xlabel("Lateral Wheelset Displacement y [mm]");
        plt::ylabel("Efective contact angle [tan(γ)]");
        // -------------------------------------------------------------
        plt::grid(true);
        // -------------------------------------------------------------
        plt::tight_layout();
        // -------------------------------------------------------------
        //##############################################################
    }

    const string *bench{};
    const int *c{};
    const vector<double> *ycp{};
    const vector<double> *ang{};
    const vector<double> *ys{};
    const vector<double> *dr{};
    const vector<double> *drs{};
    const vector<double> *rad{};
};
