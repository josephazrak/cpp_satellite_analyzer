//
// Created by Joseph Azrak on 16/08/2020.
//

#include "UCSSatelliteEntry.h"
#include "Util.cpp"
#include "candidate_satellite_t.h"
#include <cmath>

using string = std::string;

/**
 * Validates the data for selected variables from the UCS Satellite Database. Checks whether this satellite
 * is qualifying and populates member variables.
 *
 * @param sat Candidate satellite entry (candidate_satellite_t instance) to check—if check passes, the candidate is flagged as disqualified.
 */
UCSSatelliteEntry::UCSSatelliteEntry(candidate_satellite_t& sat)
{
    // If we are missing any required field, then disqualify this satellite.
    m_qualifying = !(sat.p_orbit_class.empty()    || sat.p_longitude.empty() || sat.p_perigee.empty() || sat.p_apogee.empty() || sat.p_eccentricity.empty()
                     || sat.p_inclination.empty()    || sat.p_period.empty()    || sat.p_launch_mass.empty());

    if (!m_qualifying)
    {
        m_disqualification_reason = DISQ_REASON_MISSING_PARAMETER;
        return;
    }

    /**
     * Unfortunately, the UCS CSV file is not very program-friendly.
     * We need to do extra parsing on the strings provided here.
     *
     * All numeric properties (all except p_orbit_class) must be stripped of commas (,) and
     * quotes (") as this will hinder double parsing
     */

    Util_fn::strprestod(sat.p_longitude);
    Util_fn::strprestod(sat.p_perigee);
    Util_fn::strprestod(sat.p_apogee);
    Util_fn::strprestod(sat.p_eccentricity);
    Util_fn::strprestod(sat.p_inclination);
    Util_fn::strprestod(sat.p_period);
    Util_fn::strprestod(sat.p_launch_mass);

    m_orbit_class = sat.p_orbit_class;
    m_satellite_row_id = sat.p_satellite_row_id;

    try {
        /* Now, we try to parse these values as doubles.
         * Hopefully this won't mess up. */
        m_longitude = std::stod(sat.p_longitude);
        m_perigee = std::stod(sat.p_perigee) * 1000; // CONVERSION from km to m.
        m_apogee = std::stod(sat.p_apogee) * 1000; // CONVERSION from km to m.
        m_eccentricity = std::stod(sat.p_eccentricity);
        m_inclination = std::stod(sat.p_inclination);
        m_period = std::stod(sat.p_period) * 60; // CONVERSION from minutes to seconds.
        m_launch_mass = std::stod(sat.p_launch_mass);

        /* If our m_eccentricity is higher than the command-line parameter provided,
         * disqualify this satellite. */
        if (m_eccentricity > sat.eccentricity_qualifier)
        {
            m_disqualification_reason = DISQ_REASON_ECCENTRICITY;
            m_qualifying = false;
        }

    } catch (std::invalid_argument&) {
        std::cout << "Something went wrong at sat " << sat.p_satellite_row_id << std::endl;
        std::cout << "\t"<< sat.p_longitude << " " << sat.p_perigee << " " << sat.p_apogee << " " << sat.p_eccentricity << " " << sat.p_inclination << " " << sat.p_period << " " << sat.p_launch_mass << std::endl;
    }
}

/**
 * Returns a human-readable string in table format which displays the satellite's
 * name along with its orbital parameters.
 */
void UCSSatelliteEntry::whoami()
{
    /* This function returns a human-readable string which provides the current
     * satellite's information. */

    std::vector<std::string> prop {m_orbit_class,
                                   Util_fn::num_to_rounded_str(m_longitude),
                                   Util_fn::num_to_rounded_str(m_perigee),
                                   Util_fn::num_to_rounded_str(m_apogee),
                                   Util_fn::num_to_rounded_str(m_eccentricity),
                                   Util_fn::num_to_rounded_str(m_inclination),
                                   Util_fn::num_to_rounded_str(m_period),
                                   Util_fn::num_to_rounded_str(m_launch_mass)};

    Util_fn::print_tabular(prop, TABLE_OUTPUT_PADDING);
}

UCSSatelliteEntry::~UCSSatelliteEntry() = default;

/**
 * Computes several statistics for the satellite based on Kepler's 3rd law.
 * The kepler_x and kepler_y pairs are calculated from a generalization of Kepler's 3rd law.
 * The mass of the Earth can be estimated by taking the slope of the result of a regression of these coordinates.
 *
 * The kepler_mass estimate is a second method for calculating the mass of the Earth—it functions
 * by rearranging Kepler's 3rd law to solve for the Earth's mass. These numbers must later be averaged
 * over all satellites to find a valid answer.
 */

void UCSSatelliteEntry::compute_kepler_statistics()
{
    m_kepler_y = pow(m_period, 2);
    m_kepler_x = (4.0 * (2.0 * acos(0.0)) * pow((m_apogee + m_perigee)/2.0 + RADIUS_OF_THE_EARTH, 3.0))/GRAVITATIONAL_CONSTANT;

    /* Along with the m_kepler_x and m_kepler_y statistics, we also compute a kepler_mass statistic
     * This approximation of the mass of the Earth is unique to this satellite and can later be averaged */

    double kepler_mass_numerator = (4.0 * pow((2.0 * acos(0.0)),2.0) * pow((m_apogee + m_perigee)/2.0 + RADIUS_OF_THE_EARTH, 3.0));
    double kepler_mass_denominator = (pow(m_period, 2.0) * GRAVITATIONAL_CONSTANT);
    mass_t result = kepler_mass_numerator / kepler_mass_denominator;

    kepler_mass = result;
}

