//
// Created by Joseph Azrak on 16/08/2020.
//

#ifndef CPP_SATELLITE_ANALYZER_PROJECT_UCSSATELLITEDATABASE_H
#define CPP_SATELLITE_ANALYZER_PROJECT_UCSSATELLITEDATABASE_H

#include <iostream>
#include "UCSSatelliteEntry.h"
#include <vector>

/**
 * This class is a logical representation of the entirety of the UCS satellite database.
 * Its constructor automatically loads an original -- UNTOUCHED -- UCS csv file and parses it.
 */
class UCSSatelliteDatabase
{
private:
    std::string m_csv_path; /*!< String that holds the path for the UCS database csv file */
    std::vector<UCSSatelliteEntry> m_satellites; /*!< Vector of UCSSatelliteEntry objects that make up this database */
    double m_eccentricity_qualifier; /*!< Max allowed eccentricity value */
public:
    UCSSatelliteDatabase(const std::string &csv_path, double eccentricity_qualifier);
    ~UCSSatelliteDatabase();

    void compute_kepler_statistics();
    void dump_kepler_data_to_csv(std::string& path);
    void set_eccentricity_qualifier(double qualifier) { m_eccentricity_qualifier = qualifier; };
    void update_satellite_qualification();

    int get_disqualified_satellite_count() const;

    std::vector<double> get_mass_estimations();
};


#endif //CPP_SATELLITE_ANALYZER_PROJECT_UCSSATELLITEDATABASE_H
