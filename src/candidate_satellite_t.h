//
// Created by Joseph Azrak on 16/08/2020.
//

#ifndef CPP_SATELLITE_ANALYZER_PROJECT_CANDIDATE_SATELLITE_T_H
#define CPP_SATELLITE_ANALYZER_PROJECT_CANDIDATE_SATELLITE_T_H

#include <iostream>

/**
 * Holds a satellite's information before it is parsed and selected
 * by UCSSatelliteEntry::UCSSatelliteEntry
 */
struct candidate_satellite_t {
    int p_satellite_row_id;
    std::string p_orbit_class;
    std::string p_longitude;
    std::string p_perigee;
    std::string p_apogee;
    std::string p_eccentricity;
    std::string p_inclination;
    std::string p_period;
    std::string p_launch_mass;
    double eccentricity_qualifier;
};

#endif //CPP_SATELLITE_ANALYZER_PROJECT_CANDIDATE_SATELLITE_T_H
