#pragma once

#include <algorithm>
#include <fstream>
#include <functional>
#include <sstream>

using std::ranges::generate;
using std::vector;
using std::function;
using std::string;
using std::ifstream;

class DataReader {
public:
    explicit DataReader(const std::string &filename) {
        readFile(filename);
    }

    [[nodiscard]] std::vector<double> getColumn(const int col) const {
        std::vector<double> columnData;

        for (const auto &row: data) {
            if (col >= 0 && col < row.size()) {
                columnData.push_back(row[col]);
            }
        }
        return columnData;
    }

private:
    std::vector<std::vector<double> > data;

    void readFile(const std::string &filename) {
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            double value;
            std::vector<double> row;

            while (ss >> value) {
                row.push_back(value);
            }
            data.push_back(row);
        }
    }
};


class Opl {
protected:
    function<double()> linf;

    explicit Opl(const long double &min, const double &dy)
        : linf{
            [&min, inc = 0, &dy]() mutable {
                return min + (dy * inc++);
            }
        } {
    }
};

class Linspace : protected Opl {
public:
    Linspace(const long double &min, const long double &max, const double &dy)
        : Opl(min, dy),
          min(min),
          max(max),
          dy(dy) {
    }

    [[nodiscard]] vector<double> linspacec() const {
        const int len = static_cast<int>(1 + (max - min) / dy);
        vector<double> yii(len + 1);
        generate(yii, linf);
        return yii;
    };

private:
    const long double &min, &max;
    const double &dy;
};
