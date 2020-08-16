//
// Created by Joseph Azrak on 16/08/2020.
//

#include "UCSSatelliteDatabase.h"
#include "include/csv.h"
#include <vector>
#include <fstream>

using string = std::string;

/**
 * Parses the UCS CSV file given at csv_path and populates the m_satellites vector with
 * UCSSatelliteEntry entities that represent each satellite in the database.
 *
 * @param csv_path               Path of UCS CSV file to parse
 * @param eccentricity_qualifier Maximum eccentricity value allowed to be a qualifier satellite
 */
UCSSatelliteDatabase::UCSSatelliteDatabase(const string &csv_path, double eccentricity_qualifier)
{
    std::cout << "Parsing CSV file... " << std::endl;

    io::CSVReader<8, io::trim_chars<' '>, io::no_quote_escape<'\t'>, io::throw_on_overflow, io::single_line_comment<'#'>> in(csv_path);
    in.read_header(io::ignore_extra_column,
                   "Class of Orbit", "Longitude of GEO (degrees)", "Perigee (km)", "Apogee (km)", "Eccentricity", "Inclination (degrees)",
                   "Period (minutes)", "Launch Mass (kg.)");
    int count = 0;
    int disqualified_satellites = 0;

    string pre_orbit, pre_longitude, pre_perigee, pre_apogee, pre_eccentricity, pre_inclination, pre_period, pre_launch_mass;

    auto start = std::chrono::high_resolution_clock::now();

    while (in.read_row(pre_orbit, pre_longitude, pre_perigee, pre_apogee, pre_eccentricity, pre_inclination, pre_period, pre_launch_mass))
    {
        count++;

        candidate_satellite_t candidate_satellite = {
                count, pre_orbit, pre_longitude, pre_perigee, pre_apogee, pre_eccentricity, pre_inclination, pre_period, pre_launch_mass, eccentricity_qualifier
        };

        // Create a UCSSatelliteEntry object to match this raw CSV entry
        UCSSatelliteEntry entry(candidate_satellite);

        if (!entry.isQualified())
            disqualified_satellites++;

        // Push this to guy the satellite vector
        m_satellites.push_back(entry);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "\rParsed satellites [" << count << "] in " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Disqu. satellites [" << disqualified_satellites << "] due to missing properties or bad eccentricity" << std::endl;

    m_csv_path = csv_path;
}

UCSSatelliteDatabase::~UCSSatelliteDatabase() = default;

/**
 * Calls UCSSatelliteEntry::compute_kepler_statistics() on each element of the
 * member satellite vector.
 */
void UCSSatelliteDatabase::compute_kepler_statistics()
{
    /* Iterate through the satellite vector. Call each satellite's member
     * compute_kepler_statistics function. */

    int computedSatellites = 0;

    for (UCSSatelliteEntry &entry : m_satellites)
    {
        computedSatellites++;

        // If the satellite does not qualify, then don't bother computing statistics
        if (!entry.isQualified())
            continue;

        entry.compute_kepler_statistics();
        // std::cout << "\rComputing extended statistics [" << computedSatellites << "]";
    }

    // std::cout << "\rComputing extended statistics [done]" << std::endl;
}

/**
 * Returns a human-readable string in table format which displays the satellite's
 * name along with its orbital parameters.
 * @param path Path of the file where the results should be written, in CSV format.
 */
void UCSSatelliteDatabase::dump_kepler_data_to_csv(string& path)
{
    std::ofstream basicOfstream;
    basicOfstream.open(path);
    basicOfstream << "x,y,mass_estimation" << std::endl;

    for (UCSSatelliteEntry &entry : m_satellites)
    {
        // If this entry is disqualified, ignore it.
        if (!entry.isQualified())
            continue;

        basicOfstream << entry.getKeplerX() << "," << entry.getKeplerY() << "," << entry.getKeplerMass() << std::endl;
    }

    basicOfstream.close();
}

/**
 * Dynamically updates the qualification status for each satellite in the member satellite
 * vector. This is usually used to refresh qualifier satellites after the eccentricity qualifier
 * changes.
 */
void UCSSatelliteDatabase::update_satellite_qualification()
{
    for (UCSSatelliteEntry &satellite : m_satellites) {
        satellite.update_satellite_qualification(m_eccentricity_qualifier);
    }
}

/**
 * Returns a vector of the satellite database's individual satellites'
 * mass estimations
 */
std::vector<double> UCSSatelliteDatabase::get_mass_estimations()
{
    std::vector<double> vec;

    for (UCSSatelliteEntry &satellite : m_satellites)
    {
        if (!satellite.isQualified())
            continue;

        vec.push_back(satellite.getKeplerMass());
    }

    return vec;
}

int UCSSatelliteDatabase::get_disqualified_satellite_count() const {
    int buf = 0;

    for (const UCSSatelliteEntry& entry : m_satellites)
    {
        if (!entry.isQualified())
            ++buf;
    }

    return buf;
}
