cmake_minimum_required(VERSION 3.16)
project(cpp_satellite_analyzer_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp_satellite_analyzer_project src/main.cpp src/UCSSatelliteEntry.cpp src/UCSSatelliteEntry.h src/Util.cpp src/Settings.h src/candidate_satellite_t.h src/ecm_analysis_t.h src/UCSSatelliteDatabase.cpp src/UCSSatelliteDatabase.h)