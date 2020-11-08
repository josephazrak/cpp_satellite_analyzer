//
// Created by Joseph Azrak on 16/08/2020.
//

#ifndef CPP_SATELLITE_ANALYZER_PROJECT_ECM_ANALYSIS_T_H
#define CPP_SATELLITE_ANALYZER_PROJECT_ECM_ANALYSIS_T_H

/**
 * Holds scenario analysis of a single ECM simulation.
 */
struct ecm_analysis_t {
    double qualifier;
    double kepler_mean;
    double kepler_median;
    double kepler_precision;
    double kepler_percent_error_mean;
    double kepler_percent_error_median;
    double kepler_percent_precision; /*!< relative standard deviation */
    double sec_mean;
    double sec_median;
    double sec_precision;
    double sec_percent_error_mean;
    double sec_percent_error_median;
    int    sats_disqualified;
};

#endif //CPP_SATELLITE_ANALYZER_PROJECT_ECM_ANALYSIS_T_H
