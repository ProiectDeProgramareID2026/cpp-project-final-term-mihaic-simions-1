#include "Car.h"
#include <sstream>
#include <iomanip>

std::string Car::toFileLine() const {
    std::ostringstream o;
    o << id << " " << brand << " " << vehicleType << " "
      << (available ? 1 : 0) << " " << seats << " "
      << fuelType << " " << std::fixed << std::setprecision(2) << pricePerDay;
    return o.str();
}

std::string Car::toDisplay() const {
    std::ostringstream o;
    o << "[" << id << "] " << brand << " | " << vehicleType
      << " | " << seats << " seats | " << fuelType
      << " | " << std::fixed << std::setprecision(2) << pricePerDay << " RON/day"
      << " | " << (available ? "AVAILABLE" : "RENTED");
    return o.str();
}

Car Car::fromFileLine(const std::string& line) {
    std::istringstream iss(line);
    std::string id, brand, type, fuel;
    int avail, seats;
    double price;
    iss >> id >> brand >> type >> avail >> seats >> fuel >> price;
    return Car(id, brand, type, avail == 1, seats, fuel, price);
}
