cmake_minimum_required(VERSION 3.8)

project(anagram_derivation)

find_package(Qt5 REQUIRED COMPONENTS Core)

add_executable(${PROJECT_NAME}
    "resources.qrc"
    "main.cpp"
    "dictionary.cpp"
    "dictionary.h"
    "histogram.cpp"
    "histogram.h"
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    CXX_STANDARD 14
)
