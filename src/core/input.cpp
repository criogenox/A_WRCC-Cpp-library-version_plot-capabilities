#include "input.h"

DataReader::DataReader(const std::string &filename) {
    readFile(filename);
}

std::vector<double> DataReader::getColumn(const int col) const {
    std::vector<double> columnData;

    for (const auto &row: data) {
        if (col >= 0 && static_cast<size_t>(col) < row.size()) {
            columnData.push_back(row[col]);
        }
    }
    return columnData;
}


#include <charconv>

void DataReader::readFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    std::vector<double> row;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        row.clear();

        const char *p = line.data();
        const char *end = p + line.size();
        while (p < end) {
            while (p < end && std::isspace(static_cast<unsigned char>(*p))) {
                ++p;
            }
            if (p >= end) break;
            double val = 0.0;
            auto [next, ec] = std::from_chars(p, end, val);
            if (ec != std::errc()) break;
            row.push_back(val);
            p = next;
        }

        if (!row.empty()) {
            data.push_back(row);
        }
    }
}

Opl::Opl(long double min, long double dy)
    : linf{
        [min, inc = 0, dy]() mutable {
            return min + (dy * inc++);
        }
    } {
}

Linspace::Linspace(long double min, long double max, double dy)
    : Opl(min, dy),
      min(min),
      max(max),
      dy(dy) {
}

vector<double> Linspace::linspacec() const {
    const int len = static_cast<int>(1 + (max - min) / dy);
    vector<double> yii(len + 1);
    generate(yii, linf);
    return yii;
}
