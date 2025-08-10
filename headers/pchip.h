#ifndef PCHIP_H
#define PCHIP_H

#pragma once

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>

class Pchip {
public:
    enum class Extrapolation {
        Clamp, // hold the extreme values
        Linear, // extend using extreme points derivative
    };

    Pchip(const std::vector<double> &x,
          const std::vector<double> &y,
          const double dedup_epsilon = 1e-12,
          const Extrapolation extrap = Extrapolation::Clamp)
        : extrap_(extrap) {
        if (x.size() != y.size())
            throw std::invalid_argument("x and y must have the same size");
        if (x.size() < 2)
            throw std::invalid_argument("need at least 2 points");

        // Sort by x via index indirection
        std::vector<std::size_t> idx(x.size());
        std::iota(idx.begin(), idx.end(), 0);
        std::ranges::sort(idx, [&](std::size_t i, std::size_t j) {
            return x[i] < x[j];
        });

        // Build sorted and de-duplicated vectors (keep first occurrence)
        x_.reserve(idx.size());
        y_.reserve(idx.size());
        for (std::size_t k: idx) {
            if (!x_.empty() && std::abs(x[k] - x_.back()) < dedup_epsilon) {
                // skip near-duplicate x
                continue;
            }
            x_.push_back(x[k]);
            y_.push_back(y[k]);
        }

        if (x_.size() < 2)
            throw std::invalid_argument("need at least 2 unique x values after de-dup");

        const std::size_t n = x_.size();
        h_.resize(n - 1);
        inv_h_.resize(n - 1);
        delta_.resize(n - 1);
        for (std::size_t i = 0; i + 1 < n; ++i) {
            const double h = x_[i + 1] - x_[i];
            if (!(h > 0.0))
                throw std::runtime_error("x must be strictly increasing");
            h_[i] = h;
            inv_h_[i] = 1.0 / h;
            delta_[i] = (y_[i + 1] - y_[i]) * inv_h_[i];
        }

        // Compute PCHIP node derivatives m_ (size n)
        m_.assign(n, 0.0);

        if (n == 2) {
            // Two points: linear segment; derivatives equal the secant slope
            m_[0] = m_[1] = delta_[0];
        } else {
            // Internal nodes (Fritsch–Carlson weighted harmonic mean)
            for (std::size_t i = 1; i + 1 < n; ++i) {
                const double dkm1 = delta_[i - 1];
                const double dk = delta_[i];
                if (dkm1 == 0.0 || dk == 0.0 || dkm1 * dk < 0.0) {
                    m_[i] = 0.0;
                } else {
                    const double w1 = 2.0 * h_[i] + h_[i - 1];
                    const double w2 = h_[i] + 2.0 * h_[i - 1];
                    m_[i] = (w1 + w2) / (w1 / dkm1 + w2 / dk); // weighted harmonic mean
                }
            }

            // Endpoints (one-sided shape-preserving estimates, then clamped)
            {
                const double h0 = h_[0];
                const double h1 = h_[1];
                const double d0 = delta_[0];
                const double d1 = delta_[1];
                double m0 = ((2.0 * h0 + h1) * d0 - h0 * d1) / (h0 + h1);
                if (m0 * d0 <= 0.0) m0 = 0.0;
                else if (std::abs(m0) > 3.0 * std::abs(d0)) m0 = 3.0 * d0;
                m_[0] = m0;
            }
            // ----
            {
                const double hn_2 = h_[n - 2];
                const double hn_3 = h_[n - 3];
                const double dn_2 = delta_[n - 2];
                const double dn_3 = delta_[n - 3];
                double mn = ((2.0 * hn_2 + hn_3) * dn_2 - hn_2 * dn_3) / (hn_2 + hn_3);
                if (mn * dn_2 <= 0.0) mn = 0.0;
                else if (std::abs(mn) > 3.0 * std::abs(dn_2)) mn = 3.0 * dn_2;
                m_[n - 1] = mn;
            }

            // Flat segments: if a secant is zero, set adjacent node slopes to zero
            for (std::size_t i = 0; i + 1 < n; ++i) {
                if (delta_[i] == 0.0) {
                    m_[i] = 0.0;
                    m_[i + 1] = 0.0;
                }
            }
        }
    }

    [[nodiscard]] double interpolate(double xq) const noexcept {
        const std::size_t n = x_.size();

        // Extrapolation
        if (xq <= x_.front()) {
            switch (extrap_) {
                case Extrapolation::Clamp: return y_.front();
                case Extrapolation::Linear: return y_.front() + m_.front() * (xq - x_.front());
            }
        }
        if (xq >= x_.back()) {
            switch (extrap_) {
                case Extrapolation::Clamp: return y_.back();
                case Extrapolation::Linear: return y_.back() + m_.back() * (xq - x_.back());
            }
        }

        // Locate segment: first x >= xq
        const auto it = std::ranges::lower_bound(x_, xq);
        std::size_t j = static_cast<std::size_t>(it - x_.begin());
        if (j < n && x_[j] == xq) return y_[j]; // exact hit
        // Segment is [j-1, j]
        --j;

        const double h = h_[j];
        const double t = (xq - x_[j]) * inv_h_[j];

        // Cubic Hermite basis
        const double t2 = t * t;
        const double t3 = t2 * t;
        const double h00 = 2.0 * t3 - 3.0 * t2 + 1.0;
        const double h10 = t3 - 2.0 * t2 + t;
        const double h01 = -2.0 * t3 + 3.0 * t2;
        const double h11 = t3 - t2;

        return h00 * y_[j] + h10 * h * m_[j] + h01 * y_[j + 1] + h11 * h * m_[j + 1];
    }

    // Batch evaluate
    [[nodiscard]] std::vector<double> interpolate(const std::vector<double> &xq) const {
        std::vector<double> out;
        out.reserve(xq.size());
        for (const double v: xq) out.push_back(interpolate(v));
        return out;
    }

    // Accessors
    [[nodiscard]] std::pair<double, double> domain() const noexcept { return {x_.front(), x_.back()}; }
    [[nodiscard]] std::size_t size() const noexcept { return x_.size(); }
    [[nodiscard]] Extrapolation extrapolation() const noexcept { return extrap_; }

private:
    std::vector<double> x_, y_;
    std::vector<double> h_, inv_h_, delta_; // per-segment spacing, 1/h, secant slope
    std::vector<double> m_; // node derivatives (PCHIP)
    Extrapolation extrap_{Extrapolation::Clamp};
};

#endif //PCHIP_H
