#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace FileManager {

std::vector<Car> loadFleet(const std::string& path) {
    std::vector<Car> fleet;
    std::ifstream f(path);
    if (!f) return fleet;
    int n = 0; f >> n; f.ignore();
    for (int i = 0; i < n; ++i) {
        std::string line;
        if (std::getline(f, line) && !line.empty())
            fleet.push_back(Car::fromFileLine(line));
    }
    return fleet;
}

void saveFleet(const std::vector<Car>& fleet, const std::string& path) {
    std::ofstream f(path);
    f << fleet.size() << "\n";
    for (auto& c : fleet) f << c.toFileLine() << "\n";
}

std::vector<Reservation> loadReservations(const std::vector<Car>& fleet,
                                           const std::string& path) {
    std::vector<Reservation> out;
    std::ifstream f(path);
    if (!f) return out;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string resId, custId, carId; iss >> resId >> custId >> carId;
        auto it = std::find_if(fleet.begin(), fleet.end(),
                               [&](const Car& c){ return c.getId() == carId; });
        if (it != fleet.end())
            out.push_back(Reservation::fromFileLine(line, *it));
    }
    return out;
}

void saveReservations(const std::vector<Reservation>& r, const std::string& path) {
    std::ofstream f(path);
    for (auto& res : r) f << res.toFileLine() << "\n";
}

std::string generateId(const std::string& prefix, int count) {
    return prefix + std::to_string(count + 1);
}

} // namespace FileManager
