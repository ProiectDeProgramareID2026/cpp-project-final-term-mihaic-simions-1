#pragma once
#include <vector>
#include <string>
#include <utility>
#include "Car.h"
#include "Reservation.h"

namespace FileManager {
    std::vector<Car>         loadFleet(const std::string& path = "flota.txt");
    void                     saveFleet(const std::vector<Car>& fleet, const std::string& path = "flota.txt");

    std::vector<Reservation> loadReservations(const std::vector<Car>& fleet, const std::string& path = "rezervari.txt");
    void                     saveReservations(const std::vector<Reservation>& r, const std::string& path = "rezervari.txt");

    std::string generateId(const std::string& prefix, int count);
}
