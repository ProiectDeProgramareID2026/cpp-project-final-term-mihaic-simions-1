#include "Reservation.h"
#include <sstream>

std::string Reservation::toDisplay() const {
    return "[" + reservationId + "] Customer: " + customerId +
           " | Car: " + car.getBrand() + " (" + car.getId() + ")" +
           " | " + startDate.toString() + " -> " + endDate.toString();
}

std::string Reservation::toFileLine() const {
    std::ostringstream o;
    o << reservationId << " " << customerId << " " << car.getId() << " "
      << startDate.day << " " << startDate.month << " " << startDate.year << " "
      << endDate.day   << " " << endDate.month   << " " << endDate.year;
    return o.str();
}

Reservation Reservation::fromFileLine(const std::string& line, const Car& car) {
    std::istringstream iss(line);
    std::string resId, custId, carId;
    Date s, e;
    iss >> resId >> custId >> carId
        >> s.day >> s.month >> s.year
        >> e.day >> e.month >> e.year;
    return Reservation(resId, car, s, e, custId);
}
