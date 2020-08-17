cmake_minimum_required(VERSION 3.16)
project(cpp_satellite_analyzer_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp_satellite_analyzer_project main.cpp UCSSatelliteEntry.cpp UCSSatelliteEntry.h Util.cpp Settings.h candidate_satellite_t.h ecm_analysis_t.h UCSSatelliteDatabase.cpp UCSSatelliteDatabase.h)