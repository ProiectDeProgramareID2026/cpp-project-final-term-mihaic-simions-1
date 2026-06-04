#pragma once
#include "Car.h"
#include "Date.h"
#include <string>

class Reservation {
    std::string reservationId, customerId;
    Car  car;
    Date startDate, endDate;
public:
    Reservation() = default;
    Reservation(std::string resId, Car car, Date start, Date end, std::string custId)
        : reservationId(std::move(resId)), customerId(std::move(custId)),
          car(std::move(car)), startDate(start), endDate(end) {}

    const std::string& getReservationId() const { return reservationId; }
    const std::string& getCustomerId()    const { return customerId; }
    const std::string& getCarId()         const { return car.getId(); }
    const Car&         getCar()           const { return car; }
    Date               getStartDate()     const { return startDate; }
    Date               getEndDate()       const { return endDate; }

    std::string toDisplay()  const;
    std::string toFileLine() const;
    static Reservation fromFileLine(const std::string& line, const Car& car);
};
