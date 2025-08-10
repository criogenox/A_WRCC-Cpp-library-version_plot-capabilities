#include "input.h"

DataReader::DataReader(const std::string &filename) {
    readFile(filename);
}

std::vector<double> DataReader::getColumn(const int col) const {
    std::vector<double> columnData;

    for (const auto &row: data) {
        if (col >= 0 && col < row.size()) {
            columnData.push_back(row[col]);
        }
    }
    return columnData;
}


void DataReader::readFile(const std::string &filename) {
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

Opl::Opl(const long double &min, const double &dy)
    : linf{
        [&min, inc = 0, &dy]() mutable {
            return min + (dy * inc++);
        }
    } {
}

Linspace::Linspace(const long double &min, const long double &max, const double &dy)
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
