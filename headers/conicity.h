#ifndef CONICITY_H
#define CONICITY_H

#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include "pchip.h"
#include <cassert>
#include <numeric>
#include <algorithm>
#include <complex>
#include <span>
#include "input.h"

class Con {
protected:
    Con(const std::vector<double> &yii, const std::vector<double> &r, double e,
        double r0, double min, double max, int c);

    [[nodiscard]] std::vector<double> cumsum(int sign, int idxc) const;

    void con() const;

    const std::vector<double> &r;
    const std::vector<double> &yii;
    const double &e, &r0, &min, &max;
    const int &c;
    std::unique_ptr<std::vector<double> > ang = std::make_unique<std::vector<double> >();
    std::unique_ptr<std::vector<double> > ycp = std::make_unique<std::vector<double> >();
};

class Output : protected Con {
public:
    Output(const std::vector<double> &yii, const std::vector<double> &r,
           const double &e, const double &r0, const double &min,
           const double &max, const int &c);

    [[nodiscard]] const std::vector<double> &getYcp() const;

    [[nodiscard]] const std::vector<double> &getAng() const;
};

#endif // CONICITY_H
