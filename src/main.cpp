#pragma once

#include <iostream>
#include "plotting.cpp"
#include "conicity.h"
#include "contact.h"
using std::vector;
using std::string;
using std::ifstream;

int main() {
    //#####################################################
    const auto std_cal = "../data/dr_ce_E9.dat";
    const auto bench = "../data/dr_ce_E9_bench.dat";
    const auto wd = "../data/RUIC519A.dat";
    const auto wi = "../data/RUIC519B.dat";
    const auto r = "../data/SUIC519A.dat";

    const DataReader reader(std_cal);
    const auto &drs = reader.getColumn(0);

    const DataReader wheelod(wd), wheeloi(wi), railo(r);
    const auto &y1 = railo.getColumn(0);
    const auto &zp1 = railo.getColumn(1);
    const auto &y2 = wheelod.getColumn(0);
    const auto &zp2d = wheelod.getColumn(1);
    const auto &zp2i = wheeloi.getColumn(1);
    // #***************************************************
    // type=c; % (-1) asymmetric case - 5 / 6 / 7 / 8 / 9
    //         %  (1)  symmetric case - 1 / 2 / 3 / 4
    constexpr int c{-1};
    // #***************************************************
    // ## yii=(-6.8:0.1:6.8); % 1 / 2 / 3 / 5 / 9  || n=137
    // ## yii=(-7.1:0.1:7.1); % 4                  || n=143
    // ## yii=(-6.9:0.1:6.8); % 6                  || n=138
    // ## yii=(-6.8:0.1:6.7); % 7                  || n=136
    // ## yii=(-7.1:0.1:7.0); % 8                  || n=142
    // #***************************************************
    constexpr double min = -6.8, max = 6.8;
    // constexpr double min = -7.1, max = 7.1;
    // constexpr double min = -6.9, max = 6.8;
    // constexpr double min = -6.8, max = 6.7;
    // constexpr double min = -7.1, max = 7.0;
    constexpr double dy = 0.1,
            e = 1435.16, s = e * 0.5,
            rr0 = 920, rl0 = 920,
            ce = 0, cer = 0.025; // 1/40
    const auto ys = Linspace(min, max, dy).linspacec();
    //#####################################################
    std::vector<double> zp_2d;
    std::ranges::transform(cbegin(zp2d), cend(zp2d), std::back_insert_iterator(zp_2d),
                           [](const double &yi_i) {
                               return yi_i + 10;
                           });
    std::vector<double> zp_2i;
    std::ranges::transform(cbegin(zp2i), cend(zp2i), std::back_insert_iterator(zp_2i),
                           [](const double &yi_i) {
                               return yi_i + 10;
                           });

    ConPon rail_sys(y1, y2, zp1, zp2d, zp2i, ce, s);

    for (auto &d_l: ys) {
        rail_sys.transformCoordinates(rr0, rl0, d_l, cer, s);
    }
    //#####################################################
    auto &rrr_final = rail_sys.get_rrr();
    auto &rrl_final = rail_sys.get_rrl();
    auto &linr = rail_sys.get_linr();
    auto &linl = rail_sys.get_linl();
    const auto &y_1 = rail_sys.get_y_1();
    auto &y_2 = rail_sys.get_y_2();
    auto &zp_1 = rail_sys.get_zp_1();

    constexpr double dr0 = rr0 - rl0;
    int sign = (dr0 > 0) ? -1 : 1;
    vector<double> dr(rrr_final.size());
    for (int i = 0; i < rrr_final.size(); ++i) {
        dr[i] = (rrr_final[i] - rrl_final[i]) + sign * dr0 / 2;
    }
    // ---------------
    Plot(y_1, y_2, zp_1, zp_2d, zp_2i, linr, linl, wd, wi, r);
    // ---------------
    Plot(ys, dr, drs);
    //#####################################################
    const Output out(ys, dr, e, rr0, min, max, c);

    const auto &ycp = out.getYcp();
    const auto &ang = out.getAng();
    // ---------------
    Plot(bench, c, ycp, ang);
    //#####################################################
    Plot::show();
    //#####################################################
    return 0;
}
