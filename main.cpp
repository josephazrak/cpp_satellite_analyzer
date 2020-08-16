/**
 * satellite_analyzer.cpp
 * 
 * C++ code to analyze the UCS Satellite Database written by Joseph Azrak on July-11-2020
 * This program takes a valid tab-separated CSV file (can be found on below link), extracts useful Kepler information,
 * and computes Kepler parameters automatically. Analysis of around 2.5k satellites takes 30ms, including CSV parse.
 * 
 * The satellite database file can be found at https://www.ucsusa.org/resources/satellite-database. 
 * This code is used for an IB Internal Assessment in Physics.
 * 
 * Uses libraries:
 *  ben-strasser/fast-cpp-csv-parser  [https://github.com/ben-strasser/fast-cpp-csv-parser]
 * 
 * To build this code, run `c++ -std=c++11 -Wall satellite_analyzer.cpp -o satellite_analyzer`
 * 
 * @copyright (c) 2020 Joseph Azrak
 * @author Joseph Azrak
 */

#include <iostream>
#include <cmath>
#include <chrono>
#include <algorithm>
#include "include/csv.h"
#include "Settings.h"
#include "Util.cpp"
#include "UCSSatelliteDatabase.h"
#include "ecm_analysis_t.h"

using string = std::string;

int main(int argc, char **argv)
{
    // Exit if the program has not been invoked properly.
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " db.csv output.csv eccentricity_qualifier" << std::endl;
        exit(1);
    }

    bool is_multiple_eccent_mode = false;
    string eqm_user_response = "n";
    string eqm_qualifier_min = "0";
    string eqm_qualifier_max = "0";
    string eqm_qualifier_steps = "1";
    double qualifier_min;
    double qualifier_max;
    int qualifier_steps;
    double qualifier_step_size;

    std::cout << "Would you like to enter multiple eccentricity qualifier mode? [yN] ";
    std::cin >> eqm_user_response;

    if (eqm_user_response == "y")
    {
        is_multiple_eccent_mode = true;
    } else {
        std::cout << "Using singular eccentricity qualifier provided in shell" << std::endl;
    }

    if (is_multiple_eccent_mode)
    {
        Util_fn::ask_question_str("Minimum eccentricity qualifier value: ", eqm_qualifier_min);
        Util_fn::ask_question_str("Maximum eccentricity qualifier value: ", eqm_qualifier_max);
        Util_fn::ask_question_str("Number of steps: ", eqm_qualifier_steps);

        qualifier_min =       std::stod(eqm_qualifier_min);
        qualifier_max =       std::stod(eqm_qualifier_max);
        qualifier_steps =     std::stoi(eqm_qualifier_steps);
        qualifier_step_size = (qualifier_max - qualifier_min) / qualifier_steps;

        std::cout << "Testing scenarios with eccentricity qualifiers " << qualifier_min << "->" << qualifier_max << " ss=" << qualifier_step_size << std::endl;
    }

    auto program_start = std::chrono::high_resolution_clock::now();

    string csv_path, output_path;
    double eccentricity_qualifier;

    try 
    {
        csv_path = argv[1];
        output_path = argv[2];
        eccentricity_qualifier = std::stod(argv[3]);
    } catch(std::invalid_argument& err) {
        std::cout << "Usage: " << argv[0] << " db.csv output.csv eccentricity_qualifier" << std::endl;
        exit(1);
    }

    // Only load the database once, regardless of whether we are in MEQ mode
    UCSSatelliteDatabase database(csv_path, eccentricity_qualifier);

    if (is_multiple_eccent_mode)
    {
        std::vector<ecm_analysis_t> ecm_results_vec;

        std::cout << "Computing statistics for scenario 1/" << qualifier_steps;
        for (int i = 0; i <= qualifier_steps; ++i)
        {
            std::cout << "\rComputing statistics for scenario " << i << "/" << qualifier_steps; 

            double current_qualifier = qualifier_min + qualifier_step_size * i;

            database.set_eccentricity_qualifier(current_qualifier);
            database.update_satellite_qualification();
            database.compute_kepler_statistics();

            // Get a mass-estimation vector for this eccentricity resultset, then average it and find the
            // standard deviation (to calculate precision of the answer).
            std::vector<double> mass_estimations = database.get_mass_estimations();

            double vec_mean =      Util_fn::vector_mean(mass_estimations);
            double vec_median =    Util_fn::vector_median(mass_estimations);
            double vec_precision = Util_fn::vector_standard_deviation(mass_estimations);

            double percent_error_mean = abs(((vec_mean - LITERATURE_VALUE) / (LITERATURE_VALUE)) * 100);
            double percent_error_median = abs(((vec_median - LITERATURE_VALUE) / (LITERATURE_VALUE)) * 100);
            double percent_standard_deviation = (vec_precision / vec_mean) * 100;

            ecm_analysis_t results{};

            results.qualifier = current_qualifier;
            results.mean = vec_mean;
            results.median = vec_median;
            results.precision = vec_precision;
            results.percent_error_mean = percent_error_mean;
            results.percent_error_median = percent_error_median;
            results.sats_disqualified = database.get_disqualified_satellite_count() ;
            results.percent_precision = percent_standard_deviation;

            // __builtin_trap(); -- Debugger call

            ecm_results_vec.push_back(results);
        }

    string ecm_output_path;

    std::cout << std::endl << "Save sim data to: ";
    std::cin >> ecm_output_path;

    std::ofstream csv_fstream;
    csv_fstream.open(ecm_output_path);
    csv_fstream << "max_eccentricity,mass_mean,mass_median,mass_std_dev,mass_std_dev_percent,percent_error_mean,percent_error_median,sats_disqualified" << std::endl;

    for (ecm_analysis_t& result : ecm_results_vec)
    {
        csv_fstream
            << result.qualifier << ","
            << result.mean << ","
            << result.median << ","
            << result.precision << ","
            << result.percent_precision << ","
            << result.percent_error_mean << ","
            << result.percent_error_median << ","
            << result.sats_disqualified
        << std::endl;
    }

    csv_fstream.close();
    std::cout << "Done" << std::endl;

    } else {
        /**
         * For each satellite in the now-parsed satellite database, compute Kepler statistics
         * -- this computes (x, y) pairs using the relationship
         *
         * y = m_period^2
         * x = (4 * pi * (apogeePerigeeMean+earthRadius)^3)/gravitationalConstant
         */

        database.compute_kepler_statistics();
        database.dump_kepler_data_to_csv(output_path);

        auto prog_exec_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - program_start);

        std::cout << "Dumped kepler data to csv" << std::endl;
        std::cout << "Program execution time: " << prog_exec_time.count() << "ms" << std::endl;
    }

    return 0;
}
