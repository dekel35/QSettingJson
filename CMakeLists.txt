cmake_minimum_required(VERSION 3.14)

project(TestQSettingsJson LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(TestQSettingsJson
  main.cpp
  QSettingsJson.cpp
  TestQSettingsJson.h TestQSettingsJson.cpp
)

target_link_libraries(TestQSettingsJson Qt${QT_VERSION_MAJOR}::Core)
