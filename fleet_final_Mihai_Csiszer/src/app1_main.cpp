#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "Car.h"
#include "FileManager.h"
#include "Reservation.h"

using namespace ftxui;

// Afiseaza o lista de elemente intr-un ecran cu scroll
static ScreenInteractive& globalScreen() {
    static ScreenInteractive scr = ScreenInteractive::Fullscreen();
    return scr;
}

static void showList(const std::string& title,
                     const std::vector<std::string>& items) {
    auto screen = ScreenInteractive::Fullscreen();
    int  selected = 0;
    auto menu     = Menu(&items, &selected);
    auto renderer = Renderer(menu, [&] {
        return vbox({
                   text(title) | bold | color(Color::Cyan) | hcenter,
                   separator(),
                   menu->Render() | frame | flex,
                   separator(),
                   text("  Press Enter or Esc to go back") | color(Color::GrayDark),
               }) |
               border;
    });
    auto catchEsc = CatchEvent(renderer, [&](Event e) {
        if (e == Event::Return || e == Event::Escape ||
            e == Event::Character('q') || e == Event::Character('Q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });
    screen.Loop(catchEsc);
}

// Afiseaza un formular cu campuri de intrare si returneaza true daca utilizatorul apasa OK
static bool showForm(const std::string& title,
                     std::vector<std::pair<std::string, std::string*>> fields) {
    auto screen  = ScreenInteractive::Fullscreen();
    bool confirm = false;

    Components inputs;
    for (size_t i = 0; i < fields.size(); ++i) {
        InputOption opt;
        opt.multiline = false;
        inputs.push_back(Input(fields[i].second, fields[i].first, opt));
    }

    auto btnOk  = Button("  OK  ", [&] { confirm = true;  screen.ExitLoopClosure()(); });
    auto btnCnl = Button("Cancel", [&] { confirm = false; screen.ExitLoopClosure()(); });
    auto buttons = Container::Horizontal({btnOk, btnCnl});
    auto all     = Container::Vertical({});
    for (auto& inp : inputs) all->Add(inp);
    all->Add(buttons);

    auto renderer = Renderer(all, [&] {
        Elements rows;
        for (size_t i = 0; i < fields.size(); ++i) {
            rows.push_back(hbox({
                text(fields[i].first + ": ") | size(WIDTH, EQUAL, 28),
                inputs[i]->Render() | flex,
            }));
        }
        rows.push_back(separator());
        rows.push_back(hbox({btnOk->Render(), text("   "), btnCnl->Render()}) | hcenter);
        return vbox({
                   text(title) | bold | color(Color::Cyan) | hcenter,
                   separator(),
                   vbox(rows) | flex,
               }) |
               border | size(WIDTH, EQUAL, 60);
    });
    screen.Loop(renderer);
    return confirm;
}

// Afiseaza un mesaj de informare sau eroare si asteapta confirmare
static void msgBox(const std::string& msg, bool isError = false) {
    auto screen = ScreenInteractive::Fullscreen();
    auto btn    = Button("  OK  ", screen.ExitLoopClosure());
    auto renderer = Renderer(btn, [&] {
        return vbox({
                   text(isError ? " ERROR " : " INFO ") |
                       (isError ? color(Color::Red) : color(Color::Green)) |
                       bold | hcenter,
                   separator(),
                   text(msg) | hcenter,
                   separator(),
                   btn->Render() | hcenter,
               }) |
               border | size(WIDTH, EQUAL, (int)msg.size() + 10);
    });
    screen.Loop(renderer);
}

// comenzi/actiuni

// Afiseaza toate masinile din flota
static void viewVehicles(const std::vector<Car>& fleet) {
    std::vector<std::string> lines;
    if (fleet.empty()) lines.push_back("  No vehicles in the fleet.");
    for (auto& c : fleet) lines.push_back("  " + c.toDisplay());
    showList(" All Vehicles ", lines);
}

// Adauga o masina noua in flota pe baza datelor introduse de utilizator
static void addVehicle(std::vector<Car>& fleet) {
    std::string brand, type, seatsStr, fuel, priceStr;
    bool ok = showForm(" Add Vehicle ", {
        {"Brand",               &brand},
        {"Type (car/suv/van)",  &type},
        {"Seats",               &seatsStr},
        {"Fuel (petrol/diesel/electric)", &fuel},
        {"Price/day (RON)",     &priceStr},
    });
    if (!ok) return;
    int    seats = 0;   try { seats = std::stoi(seatsStr); } catch(...) {}
    double price = 0.0; try { price = std::stod(priceStr); } catch(...) {}
    std::string id = FileManager::generateId("CAR", (int)fleet.size());
    fleet.emplace_back(id, brand, type, true, seats, fuel, price);
    FileManager::saveFleet(fleet);
    msgBox("Vehicle " + id + " added successfully.");
}

// Sterge o masina din flota dupa ID
static void deleteVehicle(std::vector<Car>& fleet) {
    std::string id;
    if (!showForm(" Delete Vehicle ", {{"Vehicle ID", &id}})) return;
    auto it = std::find_if(fleet.begin(), fleet.end(),
                           [&](const Car& c){ return c.getId() == id; });
    if (it == fleet.end()) { msgBox("Vehicle '" + id + "' not found.", true); return; }
    fleet.erase(it);
    FileManager::saveFleet(fleet);
    msgBox("Vehicle " + id + " deleted.");
}

// Modifica un camp al unei masini selectate din flota
static void modifyVehicle(std::vector<Car>& fleet) {
    std::string id;
    if (!showForm(" Modify Vehicle ", {{"Vehicle ID", &id}})) return;
    auto it = std::find_if(fleet.begin(), fleet.end(),
                           [&](const Car& c){ return c.getId() == id; });
    if (it == fleet.end()) { msgBox("Vehicle '" + id + "' not found.", true); return; }

    std::vector<std::string> fields = {
        "Price per day", "Availability (0=rented / 1=available)",
        "Seats", "Fuel type", "Brand", "Back"
    };
    auto screen  = ScreenInteractive::Fullscreen();
    int  sel     = 0;
    bool chosen  = false;
    auto menu    = Menu(&fields, &sel);
    auto renderer = Renderer(menu, [&] {
        return vbox({
                   text(" Modify Field – " + id) | bold | color(Color::Cyan) | hcenter,
                   separator(),
                   menu->Render() | frame | flex,
                   separator(),
                   text("  Enter to select  |  Esc to cancel") | color(Color::GrayDark),
               }) | border;
    });
    auto ev = CatchEvent(renderer, [&](Event e) {
        if (e == Event::Return)  { chosen = true;  screen.ExitLoopClosure()(); return true; }
        if (e == Event::Escape || e == Event::Character('q')) { screen.ExitLoopClosure()(); return true; }
        return false;
    });
    screen.Loop(ev);
    if (!chosen || sel == 5) return;

    std::string val;
    std::string label = fields[sel];
    if (!showForm(" New Value ", {{label, &val}})) return;
    switch (sel) {
        case 0: try { it->setPricePerDay(std::stod(val)); } catch(...) {} break;
        case 1: it->setAvailable(val == "1"); break;
        case 2: try { it->setSeats(std::stoi(val)); } catch(...) {} break;
        case 3: it->setFuelType(val); break;
        case 4: it->setBrand(val); break;
    }
    FileManager::saveFleet(fleet);
    msgBox("Vehicle " + id + " updated.");
}

// Cauta masini dupa marca folosind un cuvant cheie
static void searchVehicle(const std::vector<Car>& fleet) {
    std::string query;
    if (!showForm(" Search Vehicle ", {{"Brand keyword", &query}})) return;
    std::string q = query;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);
    std::vector<std::string> results;
    for (auto& c : fleet) {
        std::string b = c.getBrand();
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
        if (b.find(q) != std::string::npos)
            results.push_back("  " + c.toDisplay());
    }
    if (results.empty()) results.push_back("  No vehicles found for: " + query);
    showList(" Search Results ", results);
}

// Afiseaza toate rezervarile existente
static void viewReservations(const std::vector<Car>& fleet) {
    auto resv = FileManager::loadReservations(fleet);
    std::vector<std::string> lines;
    if (resv.empty()) lines.push_back("  No reservations found.");
    for (auto& r : resv) lines.push_back("  " + r.toDisplay());
    showList(" All Reservations ", lines);
}

// functia main

int main() {
    std::vector<std::string> entries = {
        "  View all vehicles",
        "  Add vehicle",
        "  Delete vehicle",
        "  Modify vehicle",
        "  Search vehicle by brand",
        "  View all reservations",
        "  Quit",
    };

    auto screen = ScreenInteractive::Fullscreen();
    int  sel    = 0;
    auto menu   = Menu(&entries, &sel);

    auto renderer = Renderer(menu, [&] {
        return vbox({
                   text("") ,
                   text(" FLEET MANAGER – Student 1 ") | bold | color(Color::Cyan) | hcenter,
                   text("") ,
                   separator(),
                   menu->Render() | frame | flex,
                   separator(),
                   text("  ↑↓ Navigate   Enter Select   Q Quit") | color(Color::GrayDark),
               }) | border;
    });

    auto ev = CatchEvent(renderer, [&](Event e) {
        if (e == Event::Return) {
            if (sel == 6) { screen.ExitLoopClosure()(); return true; }
            auto fleet = FileManager::loadFleet();
            switch (sel) {
                case 0: viewVehicles(fleet);     break;
                case 1: addVehicle(fleet);       break;
                case 2: deleteVehicle(fleet);    break;
                case 3: modifyVehicle(fleet);    break;
                case 4: searchVehicle(fleet);    break;
                case 5: viewReservations(fleet); break;
            }
            return true;
        }
        if (e == Event::Escape || e == Event::Character('q') || e == Event::Character('Q')) {
            screen.ExitLoopClosure()(); return true;
        }
        return false;
    });

    screen.Loop(ev);
    return 0;
}
