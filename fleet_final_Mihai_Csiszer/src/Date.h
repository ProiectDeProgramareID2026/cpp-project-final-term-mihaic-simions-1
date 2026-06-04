#pragma once
#include <string>

struct Date {
    int day = 1, month = 1, year = 2025;
    Date() = default;
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
    std::string toString() const;
    bool isValid() const;
    bool operator<(const Date& o) const;
    bool operator<=(const Date& o) const;
};
