#pragma once
#include "Vehicle.h"

class Car : public Vehicle {
    std::string fuelType;
    double      pricePerDay = 0.0;
public:
    Car() = default;
    Car(std::string id, std::string brand, std::string type,
        bool avail, int seats, std::string fuel, double price)
        : Vehicle(std::move(id), std::move(brand), std::move(type), avail, seats),
          fuelType(std::move(fuel)), pricePerDay(price) {}

    const std::string& getFuelType()    const { return fuelType; }
    double             getPricePerDay() const { return pricePerDay; }

    void setPricePerDay(double p)          { pricePerDay = p; }
    void setFuelType(const std::string& f) { fuelType = f; }

    std::string toFileLine() const override;
    std::string toDisplay()  const;          // one-line summary for UI lists

    static Car fromFileLine(const std::string& line);
};
