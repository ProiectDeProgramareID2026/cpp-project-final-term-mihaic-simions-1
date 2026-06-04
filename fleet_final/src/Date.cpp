#include "Date.h"
#include <sstream>
#include <iomanip>

std::string Date::toString() const {
    std::ostringstream o;
    o << std::setw(2) << std::setfill('0') << day   << "/"
      << std::setw(2) << std::setfill('0') << month << "/" << year;
    return o.str();
}
bool Date::isValid() const {
    if (year < 2000 || year > 2100 || month < 1 || month > 12 || day < 1) return false;
    int mx[] = {0,31,29,31,30,31,30,31,31,30,31,30,31};
    return day <= mx[month];
}
bool Date::operator<(const Date& o) const {
    if (year  != o.year)  return year  < o.year;
    if (month != o.month) return month < o.month;
    return day < o.day;
}
bool Date::operator<=(const Date& o) const { return !(o < *this); }
