cmake_minimum_required(VERSION 3.20)
project(FleetProject CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Bundle MinGW runtime into the exe — no DLLs needed to run
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()

# FTXUI: downloaded automatically on first build
include(FetchContent)
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG        v5.0.0
)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_MakeAvailable(ftxui)

# Executables land in the project root (next to .txt data files)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_SOURCE_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}")

# Shared sources
set(SHARED_SRC
    src/Date.cpp
    src/Car.cpp
    src/Reservation.cpp
    src/FileManager.cpp
)

# app_1 - Fleet Manager
add_executable(app_1 src/app1_main.cpp ${SHARED_SRC})
target_include_directories(app_1 PRIVATE src)
target_link_libraries(app_1 PRIVATE ftxui::component ftxui::dom ftxui::screen)
set_target_properties(app_1 PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# app_2 - Customer Portal
add_executable(app_2 src/app2_main.cpp ${SHARED_SRC})
target_include_directories(app_2 PRIVATE src)
target_link_libraries(app_2 PRIVATE ftxui::component ftxui::dom ftxui::screen)
set_target_properties(app_2 PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
