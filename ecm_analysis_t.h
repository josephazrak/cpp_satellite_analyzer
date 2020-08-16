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
    double mean;
    double median;
    double precision;
    double  percent_error_mean;
    double  percent_error_median;
    int    sats_disqualified;
    double percent_precision; /*!< relative standard deviation */
};

#endif //CPP_SATELLITE_ANALYZER_PROJECT_ECM_ANALYSIS_T_H
