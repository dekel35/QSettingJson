cmake_minimum_required(VERSION 3.14)

project(TestQSettingsJson)

add_compile_options(-g -O0 -pedantic -Wall)
find_package(Qt6Gui REQUIRED)
add_subdirectory(test)

TARGET_LINK_LIBRARIES(TestQSettingsJson  Qt6::Core Qt6::Gui )
