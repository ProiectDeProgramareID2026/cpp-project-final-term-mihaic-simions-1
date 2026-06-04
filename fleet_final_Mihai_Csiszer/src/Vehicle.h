#pragma once
#include <string>

class Vehicle {
protected:
    std::string id, brand, vehicleType;
    bool available = true;
    int  seats     = 5;
public:
    Vehicle() = default;
    Vehicle(std::string id, std::string brand, std::string type, bool avail, int seats)
        : id(std::move(id)), brand(std::move(brand)), vehicleType(std::move(type)),
          available(avail), seats(seats) {}
    virtual ~Vehicle() = default;

    const std::string& getId()          const { return id; }
    const std::string& getBrand()       const { return brand; }
    const std::string& getVehicleType() const { return vehicleType; }
    bool               isAvailable()    const { return available; }
    int                getSeats()       const { return seats; }

    void setAvailable(bool a)                  { available = a; }
    void setSeats(int s)                       { seats = s; }
    void setBrand(const std::string& b)        { brand = b; }
    void setVehicleType(const std::string& t)  { vehicleType = t; }

    virtual std::string toFileLine() const = 0;
};
