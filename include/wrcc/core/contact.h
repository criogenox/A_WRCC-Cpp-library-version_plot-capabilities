#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <ranges>
#include "wrcc/core/pchip.h"

class RailSystem {
public:
    RailSystem(const std::vector<double> &y1, const std::vector<double> &y2,
               const std::vector<double> &zp1, double beta, double s,
               unsigned long n);

protected:
    void rotateAndTransform(double beta, double s);

    const unsigned long n;
    const std::vector<double> &y1;
    const std::vector<double> &y2;
    const std::vector<double> &zp1;
    std::vector<double> y_1;
    std::vector<double> zp_1;
    std::vector<double> y_2;
};

class ConPon : protected RailSystem {
public:
    ConPon(const std::vector<double> &y1, const std::vector<double> &y2,
           const std::vector<double> &zp1, const std::vector<double> &zp2d,
           const std::vector<double> &zp2i, double beta, double s);

    void transformCoordinates(double rr0, double rl0,
                              double d_l, double cer, double s);

    [[nodiscard]] const std::vector<double> &get_rrr() const;

    [[nodiscard]] const std::vector<double> &get_rrl() const;

    [[nodiscard]] const std::vector<double> &get_radr() const;

    [[nodiscard]] const std::vector<double> &get_radl() const;

    [[nodiscard]] const std::vector<std::pair<double, double> > &get_linl() const;

    [[nodiscard]] const std::vector<std::pair<double, double> > &get_linr() const;

    [[nodiscard]] const std::vector<double> &get_y_1() const;

    [[nodiscard]] const std::vector<double> &get_y_2() const;

    [[nodiscard]] const std::vector<double> &get_zp_1() const;

private:
    [[nodiscard]] std::pair<double, long> rollRadious(const std::vector<double> &zp_c) const;

    static double dotProduct(const std::vector<double> &a, const std::vector<double> &b);

    void contZone(std::vector<std::pair<double, double> > &lin, const double R[2][2],
                  std::vector<double> &rrp, std::vector<double> &radp, const std::vector<double> &y2,
                  const std::vector<double> &zp2, double rr, int sign,
                  double d_l, double g) const;

    void rotation(std::vector<std::pair<double, double> > &lin,
                  std::vector<double> &rrp,
                  std::vector<double> &radp, double rr,
                  double d_l, double cer, double s, int sign) const;

    double s_val;
    const std::vector<double> &zp2d;
    const std::vector<double> &zp2i;
    std::vector<double> rrl;
    std::vector<double> rrr;
    std::vector<double> radl;
    std::vector<double> radr;
    std::vector<std::pair<double, double> > linl;
    std::vector<std::pair<double, double> > linr;
};
