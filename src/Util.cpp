//
// Created by Joseph Azrak on 16/08/2020.
//
#ifndef CPP_SATELLITE_ANALYZER_PROJECT_UTIL
#define CPP_SATELLITE_ANALYZER_PROJECT_UTIL

#include <iostream>
#include "Settings.h"
#include <fstream>
#include <numeric>
#include <climits>
#include <vector>

using string = std::string;

namespace Util_fn
{
    /**
     * Strips a character from the given string.
     *
     * @param original The string from which to strip the characters.
     * @param target   The character which should be removed.
     */
    static void strstripchar(string &original, char target)
    {
        original.erase(std::remove(original.begin(), original.end(), target), original.end());
    }

/**
 * Prepares a string-y double such as "\"39.4\"", or 39,112.3 to be converted to a double
 * number by stripping characters from it
 *
 * @param original The string that will later be converted to a double
 */
    static void strprestod(string &original)
    {
        strstripchar(original, '"');
        strstripchar(original, ',');
    }

/**
 * Rounds a number, such as a double, to PRINTOFF_ROUND_SF sig figs, and then stringifies it.
 *
 * @param original The number in question.
 */
    static string num_to_rounded_str(double &original)
    {
        double factor = pow(10.0, PRINTOFF_ROUND_SF - ceil(log10(abs(original))));
        return std::to_string(round(original * factor) / factor);
    }

/**
 * Prints a single row of values in tabular format.
 *
 * @param strings         Vector of strings to print.
 * @param separator_width The space in between the columns.
 */
    static void print_tabular(const std::vector<string>& strings, int separator_width)
    {
        for (const string &pr : (strings))
        {
            std::cout << std::left << std::setw(separator_width) << std::setfill(' ') << pr;
        }

        std::cout << std::endl;
    }

/**
 * Takes a vector of double numbers and returns the mean
 */
    static double vector_mean(std::vector<double>& vec)
    {
        return (std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size());
    }

/**
 * Takes a vector of double numbers and returns the median
 */
    static double vector_median(std::vector<double>& vec)
    {
        size_t n = vec.size() / 2;
        std::nth_element(vec.begin(), vec.begin() + n, vec.end());
        return vec[n];
    }

/**
 * Calculates the standard deviation of a vector of doubles
 */
    static double vector_standard_deviation(std::vector<double>& v)
    {
        double buf = 0;
        double mean = vector_mean(v);

        for (double element : v)
            buf += pow(element - mean, 2);

        return sqrt(buf / v.size());
    }
}

#endif //CPP_SATELLITE_ANALYZER_PROJECT_UTIL