//
// Created by Joseph Azrak on 16/08/2020.
//

#ifndef CPP_SATELLITE_ANALYZER_PROJECT_UCSSATELLITEENTRY_H
#define CPP_SATELLITE_ANALYZER_PROJECT_UCSSATELLITEENTRY_H

#include <iostream>
#include "candidate_satellite_t.h"

typedef double kepler_relation_coord_t;
typedef double mass_t;

/**
 * This class holds a single satellite entry from the UCS satellite database. It only
 * contains relevant parameters.
 */
class UCSSatelliteEntry
{
private:
    bool m_qualifying; /*!< Whether this satellite qualifies for calculations */
    int m_disqualification_reason; /*!< Why the satellite is disqualified, if that is the case */
    std::string m_orbit_class; /*!< Variable(s) from UCS DB */
    int m_satellite_row_id; /*!< Row number in the UCS DB */
    double m_longitude, m_perigee, m_apogee, m_eccentricity, m_inclination, m_period, m_launch_mass;/*!< Variable(s) from UCS DB*/
    kepler_relation_coord_t m_kepler_x = 0; /*!< Computed Kepler x-coordinate */
    kepler_relation_coord_t m_kepler_y = 0; /*!< Computed Kepler y-coordinate */
    mass_t kepler_mass = 0; /*!< Estimation of the mass of the Earth using these orbital parameters */
public:
    explicit UCSSatelliteEntry(candidate_satellite_t& sat);
    ~UCSSatelliteEntry();

    __unused void whoami();
    void compute_kepler_statistics();

    inline bool isQualified() const { return m_qualifying; };
    inline double getKeplerX() const { return m_kepler_x; };
    inline double getKeplerY() const { return m_kepler_y; };
    inline double getKeplerMass() const { return kepler_mass; };
    inline void update_satellite_qualification(double new_eccentricity_qualifier) { if (new_eccentricity_qualifier != 0) { m_qualifying = (m_eccentricity <= new_eccentricity_qualifier); } else { m_qualifying = (m_eccentricity == 0); } };

};


#endif //CPP_SATELLITE_ANALYZER_PROJECT_UCSSATELLITEENTRY_H
