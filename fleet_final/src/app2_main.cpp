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
               }) | border;
    });
    auto ev = CatchEvent(renderer, [&](Event e) {
        if (e == Event::Return || e == Event::Escape ||
            e == Event::Character('q') || e == Event::Character('Q')) {
            screen.ExitLoopClosure()(); return true;
        }
        return false;
    });
    screen.Loop(ev);
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
                       (isError ? color(Color::Red) : color(Color::Green)) | bold | hcenter,
                   separator(),
                   text(msg) | hcenter,
                   separator(),
                   btn->Render() | hcenter,
               }) | border | size(WIDTH, EQUAL, (int)msg.size() + 10);
    });
    screen.Loop(renderer);
}

// comenzi/actiuni

// Afiseaza doar masinile disponibile pentru inchiriere
static void viewAvailable(const std::vector<Car>& fleet) {
    std::vector<std::string> lines;
    for (auto& c : fleet)
        if (c.isAvailable()) lines.push_back("  " + c.toDisplay());
    if (lines.empty()) lines.push_back("  No vehicles are currently available.");
    showList(" Available Vehicles ", lines);
}

// Creeaza o rezervare pentru o masina intr-un interval de date
static void makeReservation(std::vector<Car>& fleet,
                             std::vector<Reservation>& resv) {
    std::string custId, carId, sd, sm, sy, ed, em, ey;
    bool ok = showForm(" Make Reservation ", {
        {"Your customer ID",   &custId},
        {"Car ID to reserve",  &carId},
        {"Start Day",          &sd},
        {"Start Month",        &sm},
        {"Start Year",         &sy},
        {"End Day",            &ed},
        {"End Month",          &em},
        {"End Year",           &ey},
    });
    if (!ok) return;

    auto it = std::find_if(fleet.begin(), fleet.end(),
                           [&](const Car& c){ return c.getId() == carId; });
    if (it == fleet.end()) { msgBox("Car '" + carId + "' not found.", true); return; }
    if (!it->isAvailable()) { msgBox("Car '" + carId + "' is currently unavailable.", true); return; }

    Date start, end;
    try {
        start = Date(std::stoi(sd), std::stoi(sm), std::stoi(sy));
        end   = Date(std::stoi(ed), std::stoi(em), std::stoi(ey));
    } catch(...) { msgBox("Invalid date values.", true); return; }

    if (!start.isValid() || !end.isValid()) { msgBox("Invalid date entered.", true); return; }
    if (end < start) { msgBox("End date must be after start date.", true); return; }

    std::string resId = FileManager::generateId("RES", (int)resv.size());
    resv.emplace_back(resId, *it, start, end, custId);
    it->setAvailable(false);
    FileManager::saveFleet(fleet);
    FileManager::saveReservations(resv);
    msgBox("Reservation " + resId + " created successfully!");
}

// Anuleaza o rezervare dupa ID si marcheaza masina ca disponibila
static void cancelReservation(std::vector<Car>& fleet,
                               std::vector<Reservation>& resv) {
    std::string resId;
    if (!showForm(" Cancel Reservation ", {{"Reservation ID", &resId}})) return;
    auto it = std::find_if(resv.begin(), resv.end(),
                           [&](const Reservation& r){ return r.getReservationId() == resId; });
    if (it == resv.end()) { msgBox("Reservation '" + resId + "' not found.", true); return; }
    std::string carId = it->getCarId();
    resv.erase(it);
    auto cit = std::find_if(fleet.begin(), fleet.end(),
                            [&](const Car& c){ return c.getId() == carId; });
    if (cit != fleet.end()) cit->setAvailable(true);
    FileManager::saveFleet(fleet);
    FileManager::saveReservations(resv);
    msgBox("Reservation " + resId + " cancelled. Vehicle is now available.");
}

// Afiseaza toate rezervarile unui client dupa ID-ul acestuia
static void viewMyReservations(const std::vector<Reservation>& resv) {
    std::string custId;
    if (!showForm(" View My Reservations ", {{"Your customer ID", &custId}})) return;
    std::vector<std::string> lines;
    for (auto& r : resv)
        if (r.getCustomerId() == custId) lines.push_back("  " + r.toDisplay());
    if (lines.empty()) lines.push_back("  No reservations found for: " + custId);
    showList(" Reservations for " + custId, lines);
}

// functia main

int main() {
    std::vector<std::string> entries = {
        "  View available vehicles",
        "  Make a reservation",
        "  Cancel a reservation",
        "  View my reservations",
        "  Quit",
    };

    auto screen = ScreenInteractive::Fullscreen();
    int  sel    = 0;
    auto menu   = Menu(&entries, &sel);

    auto renderer = Renderer(menu, [&] {
        return vbox({
                   text(""),
                   text(" CUSTOMER PORTAL – Student 2 ") | bold | color(Color::Cyan) | hcenter,
                   text(""),
                   separator(),
                   menu->Render() | frame | flex,
                   separator(),
                   text("  ↑↓ Navigate   Enter Select   Q Quit") | color(Color::GrayDark),
               }) | border;
    });

    auto ev = CatchEvent(renderer, [&](Event e) {
        if (e == Event::Return) {
            if (sel == 4) { screen.ExitLoopClosure()(); return true; }
            auto fleet = FileManager::loadFleet();
            auto resv  = FileManager::loadReservations(fleet);
            switch (sel) {
                case 0: viewAvailable(fleet);           break;
                case 1: makeReservation(fleet, resv);   break;
                case 2: cancelReservation(fleet, resv); break;
                case 3: viewMyReservations(resv);       break;
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
