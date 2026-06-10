# CMakeLists.txt - CMake build configuration, C++17 standard
cmake_minimum_required(VERSION 3.16)
project(CampusNavigation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force console subsystem (not WinMain)
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole")
endif()

# Source files
add_executable(CampusNavigation
    main.cpp
    LGraph.cpp
    LocationInfo.cpp
    CsvIO.cpp
    Algorithm.cpp
    CommandProcessor.cpp
    GraphException.h
    LGraph.h
    LocationInfo.h
    CsvIO.h
    Algorithm.h
    CommandProcessor.h
)