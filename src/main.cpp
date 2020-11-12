/**
 * satellite_analyzer.cpp
 * 
 * C++ code to analyze the UCS Satellite Database written by Joseph Azrak on July-11-2020. This code is used for an IB Internal Assessment in Physics.
 * This program takes a valid tab-separated CSV file (can be found on below link), extracts useful Kepler information,
 * and computes Kepler parameters automatically. Analysis of around 2.5k satellites takes 30ms, including CSV parse.
 * 
 * The satellite database file can be found at https://www.ucsusa.org/resources/satellite-database.
 * 
 * Uses open-source libraries as described in LICENSE.
 * 
 * To build this code, run `./build.sh`. You need Cmake.
 *
 * Usage: cpp-satellite-analyzer [options]

 * Arguments:
 * --input     	input CSV file for this analysis action[Required]
 * --output    	output CSV file for this analysis action[Required]
 * --ecc       	eccentricity qualifier (for non-MEQ mode)
 * --meq       	enter multiple eccentricity qualifier mode
 * --meq-min   	minimum eccentricity (for MEQ mode)
 * --meq-max   	maximum eccentricity (for MEQ mode)
 * --meq-steps 	number of steps (for MEQ mode)
 * 
 * @copyright (c) 2020 Joseph Azrak
 * @author Joseph Azrak
 */

#define LOGURU_WITH_STREAMS 1

#include <iostream>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <set>
#include "include/csv.h"
#include "include/loguru.cpp"
#include "include/argparse.hpp"
#include "Settings.h"
#include "Util.cpp"
#include "UCSSatelliteDatabase.h"
#include "ecm_analysis_t.h"

using string = std::string;

typedef string filename_t;

bool file_exists(const string& filename);

bool file_exists(const string& filename)
{
    std::ifstream infile(filename);
    return infile.good();
}

int main(int argc, char **argv)
{
    loguru::init(argc, argv);

    argparse::ArgumentParser program("cpp-satellite-analyzer");

    // Set up command-line arguments
    // Thanks p-ranav/argparse!

    program.add_argument("--input")
            .required()
            .help("input CSV file for this analysis action")
            .action([](const std::string &value) {
                if (!file_exists(value))
                {
                    LOG_S(ERROR) << "The file " << value << " does not exist. ";
                    exit(1);
                }
                return value;
            });

    program.add_argument("--output")
        .required()
        .help("output CSV file for this analysis action");

    program.add_argument("--ecc")
        .help("eccentricity qualifier (for non-MEQ mode)")
        .default_value(-1.00)
        .action([](const std::string &value) {
            return std::stod(value);
        });

    program.add_argument("--meq")
            .help("enter multiple eccentricity qualifier mode")
            .default_value(false)
            .implicit_value(true);

    program.add_argument("--meq-min")
        .help("minimum eccentricity (for MEQ mode)")
        .default_value(string("NA"));

    program.add_argument("--meq-max")
        .default_value(string("NA"))
        .help("maximum eccentricity (for MEQ mode)");

    program.add_argument("--meq-steps")
        .default_value(string("NA"))
        .help("number of steps (for MEQ mode)");

    filename_t sOutputFile;
    filename_t sInputFile;
    bool bIsMeqMode = false;
    double dMeqMin;
    double dMeqMax;
    double dMeqStepSize;
    int iMeqSteps;
    double dEccentricityQualifier;

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& error) {
        LOG_S(ERROR) << error.what();
        exit(1);
    }

    bIsMeqMode = program.get<bool>("--meq");
    sInputFile = program.get<string>("--input");
    sOutputFile = program.get<string>("--output");
    dEccentricityQualifier = program.get<double>("--ecc");

    if (bIsMeqMode)
    {
        // Make sure we have all needed MEQ variables. If one is NaN, the try-catch block will
        // take care of it.

        try {
            dMeqMin = std::stod(program.get<string>("--meq-min"));
            dMeqMax = std::stod(program.get<string>("--meq-max"));
            iMeqSteps = std::stoi(program.get<string>("--meq-steps"));
            dMeqStepSize = (dMeqMax - dMeqMin) / iMeqSteps;
        } catch (const std::invalid_argument& error) {
            LOG_S(ERROR) << "Argument parse failed. You might be missing an argument for MEQ-mode: " << error.what();
            exit(1);
        }
    } else {
        // If not in MEQ mode, we need to make sure that the user has specified
        // an eccentricity qualifier.
        if (dEccentricityQualifier == -1)
        {
            LOG_S(ERROR) << "Please specify an eccentricity qualifier with --ecc <qualifier>";
            exit(1);
        }
    }

    // At this point, we have input/output paths, eccentricity qualifier, and MEQ parameters
    // (if needed). First, parse the CSV file as this is common to both MEQ and
    // non-MEQ operations.
    //
    // dEccentricityQualifier is by default set to zero. This is ok for MEQ mode, as
    // MEQ mode by design varies this eccentricity qualifier anyway, using
    // UCSSatelliteDatabase::update_satellite_qualification().

    UCSSatelliteDatabase satellite_database(sInputFile, dEccentricityQualifier);

    // DEBUG: Print all parsed args.
    // LOG_S(INFO) << "INP: " << sInputFile;
    // LOG_S(INFO) << "OUT: " << sOutputFile;
    // LOG_S(INFO) << "MEQ: " << (bIsMeqMode ? "YES" : "NO");
    //
    // if (bIsMeqMode)
    // {
    //    LOG_S(INFO) << "    dMeqMin   " << dMeqMin;
    //    LOG_S(INFO) << "    dMeqMax   " << dMeqMax;
    //    LOG_S(INFO) << "    iMeqSteps " << iMeqSteps;
    //    LOG_S(INFO) << "    dMeqSs    " << dMeqStepSize;
    // }
    //
    // LOG_S(INFO) << "ECC-QUAL: " << std::to_string(dEccentricityQualifier);

    if (bIsMeqMode)
    {
        // ----------------------------------------------------------------------
        //                      MEQ MODE LOGIC BEGIN
        // ----------------------------------------------------------------------

        std::vector<ecm_analysis_t> meq_result_vector {};

        for (int i = 0; i <= iMeqSteps; ++i)
        {
            // Tell the UCSSatelliteDatabase that we are updating the candidacy settings
            // and reload the satellite qualification info.
            satellite_database.set_eccentricity_qualifier(dMeqMin + dMeqStepSize * i);
            satellite_database.update_satellite_qualification();

            // Recompute Kepler and secondary statistics for these new candidacy settings.
            satellite_database.compute_kepler_statistics();
            satellite_database.compute_secondary_method();

            // We have now computed a kepler/secondary result-set for this specific eccentricity-qualifier.
            // Get some useful results from this simulation and save to ecm_analysis_t instance.
            std::vector<double> kep_mass_estimations = satellite_database.get_mass_estimations();
            std::vector<double> sec_mass_estimations = satellite_database.get_secondary_mass_estimations();

            double kep_vec_mean      = Util_fn::vector_mean(kep_mass_estimations);
            double kep_vec_median    = Util_fn::vector_median(kep_mass_estimations);
            double kep_vec_precision = Util_fn::vector_standard_deviation(kep_mass_estimations);

            double sec_vec_mean      = Util_fn::vector_mean(sec_mass_estimations);
            double sec_vec_median    = Util_fn::vector_median(sec_mass_estimations);
            double sec_vec_precision = Util_fn::vector_standard_deviation(sec_mass_estimations);

            double kep_percent_error_mean         = abs(((kep_vec_mean - LITERATURE_VALUE) / (LITERATURE_VALUE)) * 100);
            double kep_percent_error_median       = abs(((kep_vec_median - LITERATURE_VALUE) / (LITERATURE_VALUE)) * 100);
            double kep_percent_standard_deviation = (kep_vec_precision / kep_vec_mean) * 100;

            double sec_percent_error_mean         = abs(((sec_vec_mean - LITERATURE_VALUE) / (LITERATURE_VALUE)) * 100);
            double sec_percent_error_median       = abs(((sec_vec_median - LITERATURE_VALUE) / (LITERATURE_VALUE)) * 100);
            double sec_percent_standard_deviation = (sec_vec_precision / sec_vec_mean) * 100;

            ecm_analysis_t result = {
                    (dMeqMin + dMeqStepSize * i), kep_vec_mean, kep_vec_median, kep_vec_precision, kep_percent_error_mean,
                    kep_percent_error_median, kep_percent_standard_deviation, sec_vec_mean, sec_vec_median, sec_vec_precision,
                    sec_percent_error_mean, sec_percent_error_median, satellite_database.get_disqualified_satellite_count()
            };

            // Stash this simulation result to the result-set vector.
            meq_result_vector.push_back(result);
        }

        // Now, we have a populated meq_result_vector with n = iMeqSteps simulation entries.
        // We need to output this data to a csv file.

        // In order to avoid duplicate entries
        std::set<int> already_seen_rows {};

        // Open a file handle to the desired output file.
        std::ofstream csv_fstream;
        csv_fstream.open(sOutputFile);
        csv_fstream << "max_eccentricity,kep_mass_mean,kep_mass_median,kep_mass_std_dev,kep_mass_std_dev_percent,kep_percent_error_mean,kep_percent_error_median,sec_mean,sec_median,sec_std_dev,sec_percent_error_mean,sec_percent_error_median,sats_disqualified,sats_used" << std::endl;

        for (ecm_analysis_t& result : meq_result_vector)
        {
            if (already_seen_rows.find(result.sats_disqualified) != already_seen_rows.end())
                continue; // We have already seen this datapoint, do not export it.

            csv_fstream
                    << result.qualifier << ","
                    << result.kepler_mean << ","
                    << result.kepler_median << ","
                    << result.kepler_precision << ","
                    << result.kepler_percent_precision << ","
                    << result.kepler_percent_error_mean << ","
                    << result.kepler_percent_error_median << ","
                    << result.sec_mean << ","
                    << result.sec_median << ","
                    << result.sec_precision << ","
                    << result.sec_percent_error_mean << ","
                    << result.sec_percent_error_median << ","
                    << result.sats_disqualified << ","
                    << (satellite_database.get_satellite_count() - result.sats_disqualified)
                    << std::endl;

            already_seen_rows.insert(result.sats_disqualified);
        }

        csv_fstream.close();

        LOG_S(INFO) << "Finished MEQ operation!";
        LOG_S(INFO) << "Completed " << iMeqSteps << " simulation(s) totalling approx " << satellite_database.get_satellite_count() * 2 * iMeqSteps << " calculations";
    } else {
        // ----------------------------------------------------------------------
        //                      NON-MEQ MODE LOGIC BEGIN
        // ----------------------------------------------------------------------
        satellite_database.compute_kepler_statistics();
        satellite_database.compute_secondary_method();
        satellite_database.dump_kepler_data_to_csv(sOutputFile);

        LOG_S(INFO) << "Finished analysis operation!";
        LOG_S(INFO) << "Data saved to " << sOutputFile << ".";
    }

    return 0;
}
