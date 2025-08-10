#ifndef INPUT_H
#define INPUT_H

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
    explicit DataReader(const std::string &filename);

    [[nodiscard]] std::vector<double> getColumn(const int col) const;

private:
    std::vector<std::vector<double> > data;

    void readFile(const std::string &filename);
};


class Opl {
protected:
    function<double()> linf;

    explicit Opl(const long double &min, const double &dy);
};

class Linspace : protected Opl {
public:
    Linspace(const long double &min, const long double &max, const double &dy);

    [[nodiscard]] vector<double> linspacec() const;

private:
    const long double &min, &max;
    const double &dy;
};

#endif //INPUT_H
