#ifndef CONTACT_H
#define CONTACT_H

#pragma once

#include <memory>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <span>
#include "pchip.h"

using std::vector;
using std::string;
using std::ifstream;

class RailSystem {
public:
    RailSystem(const std::vector<double> &y1, const std::vector<double> &y2,
               const std::vector<double> &zp1, const double &beta, const double &s,
               unsigned long n);

private:
    void rotateAndTransform(double beta, double s) const;

protected:
    const unsigned long n;
    const std::vector<double> &y1;
    const std::vector<double> &y2;
    const std::vector<double> &zp1;
    std::unique_ptr<vector<double> > y_1 = std::make_unique<vector<double> >(n);
    std::unique_ptr<vector<double> > zp_1 = std::make_unique<vector<double> >(n);
    std::unique_ptr<vector<double> > y_2 = std::make_unique<vector<double> >(n);
};

class ConPon : protected RailSystem {
public:
    ConPon(const std::vector<double> &y1, const std::vector<double> &y2,
           const std::vector<double> &zp1, const std::vector<double> &zp2d,
           const std::vector<double> &zp2i, const double &beta, const double &s);

    void transformCoordinates(double rr0, double rl0,
                              double d_l, double cer, double s) const;

    [[nodiscard]] const std::vector<double> &get_rrr() const;

    [[nodiscard]] const std::vector<double> &get_rrl() const;

    [[nodiscard]] const std::vector<double> &get_radr() const;

    [[nodiscard]] const std::vector<double> &get_radl() const;

    [[nodiscard]] vector<std::pair<double, double> > &get_linl() const;

    [[nodiscard]] vector<std::pair<double, double> > &get_linr() const;

    [[nodiscard]] const std::vector<double> &get_y_1() const;

    [[nodiscard]] const std::vector<double> &get_y_2() const;

    [[nodiscard]] const std::vector<double> &get_zp_1() const;

private:
    [[nodiscard]] std::pair<double, const long> rollRadious(const vector<double> &zp_c) const;

    static double dotProduct(const std::vector<double> &a, const std::vector<double> &b);

    void contZone(std::vector<std::pair<double, double> > &lin, const double R[2][2],
                  vector<double> &rrp, vector<double> &radp, const vector<double> &y2,
                  const vector<double> &zp2, double rr, int sign,
                  double d_l, double g) const;

    void rotation(const std::unique_ptr<std::vector<std::pair<double, double> > > &lin,
                  const std::unique_ptr<vector<double> > &rrp,
                  const std::unique_ptr<vector<double> > &radp, double rr,
                  double d_l, double cer, double s, int sign) const;

    const double &s;
    const std::vector<double> &zp2d;
    const std::vector<double> &zp2i;
    std::unique_ptr<vector<double> > rrl = std::make_unique<vector<double> >();
    std::unique_ptr<vector<double> > rrr = std::make_unique<vector<double> >();
    std::unique_ptr<vector<double> > radl = std::make_unique<vector<double> >();
    std::unique_ptr<vector<double> > radr = std::make_unique<vector<double> >();
    std::unique_ptr<std::vector<std::pair<double, double> > > linl = std::make_unique<std::vector<std::pair<double,
        double> > >();
    std::unique_ptr<std::vector<std::pair<double, double> > > linr = std::make_unique<std::vector<std::pair<double,
        double> > >();
};

#endif //CONTACT_H
