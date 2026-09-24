#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include "wrcc/core/pchip.h"
#include <cassert>
#include <numeric>
#include <algorithm>
#include <complex>
#include <span>
#include "wrcc/models/input.h"

#include <ranges>
#include <stdexcept>

struct ConicityResult {
    double yp;
    double y_left;
    double y_right;
    int idx_left;
    int idx_right;
    double s_value;
};

class Con {
protected:
    Con(const std::vector<double> &yii, const std::vector<double> &r, double e,
        double r0, double min, double max, int c);

    [[nodiscard]] std::vector<double> cumsum(int sign, int idxc) const;

    void con();

    const std::vector<double> &r;
    const std::vector<double> &yii;
    const double e, r0, min, max;
    const int c;
    std::vector<double> ang;
    std::vector<double> ycp;
};

class Output : public Con {
public:
    Output(const std::vector<double> &yii, const std::vector<double> &r,
           double e, double r0, double min,
           double max, int c);

    [[nodiscard]] const std::vector<double> &getYcp() const;

    [[nodiscard]] const std::vector<double> &getAng() const;
};
