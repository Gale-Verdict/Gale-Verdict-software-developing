cmake_minimum_required(VERSION 3.16)
project(EVChargingPlatform LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Sql)

add_executable(ev_charging_platform
    src/main.cpp
    src/charging_repository.h
    src/charging_repository.cpp
    src/charging_service.h
    src/charging_service.cpp
    src/socket_server.h
    src/socket_server.cpp
)

target_link_libraries(ev_charging_platform
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Sql
)
