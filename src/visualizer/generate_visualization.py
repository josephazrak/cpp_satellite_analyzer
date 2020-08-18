#    generate_visualization.py
#
#    Takes a file from the C++ data preprocessor (github:jazrak/cpp_satellite_analyzer) and creates useful
#    graphical visualizations using matplotlib.
#
#    Example usage:
#        1. generate a preprocessed analysis file
#            `satellite_analyzer [data.csv] [unused.csv] 0`
#            y[enter]0[enter]1[enter]200[enter]processed.csv
#
#        2. generate visualizations
#            `python3 generate_visualization.py kepler-xypair processed.csv`
#
#    SYNTAX: python3 generate_visualization.py [PLOT_NAME] [DATA_FILE]
#
#    PLOT_NAME: string, one of...
#              PLOT_NAME                  DESCRIPTION                        DATA
#            ------------------------+----------------------------+---------------------------------
#        1. 'kepler-xypair'          | `x vs y kepler pairs `     | [data-type: single-eccent-mode]
#        2. 'eccent-mean-error'      | `max. eccent vs mean error`| [data-type: multiple-eccent-mode]
#        3. 'eccent-mean-error-disq' | `same as 2 plus #disq sats`| [data-type: multiple-eccent-mode]
#
#    Used in an International Baccalaureate Internal Assessment for Physics.
#
#    (c) 2020 Joseph Azrak

import matplotlib.pyplot as plt
import numpy
import argparse
import daiquiri
import logging
import csv

<<<<<<< HEAD
daiquiri.setup(level=logging.INFO)
_logger = daiquiri.getLogger('viz')

X_TICKS_NUMBER = 8
Y_TICKS_NUMBER = 10

def plot_kepler_xypair(args: argparse.Namespace):
    # plot-arg 'kepler-xypair';
    #
    # uses the 'graphical method' to estimate the mass of the Earth by fitting a LSRL to the linear
    # relationship given by 'keplerX' vs 'keplerY'; the negative reciprocal of the slope of the LSRL
    # is the mass estimation.
=======
    Used in an International Baccalaureate Internal Assessment for Physics.
>>>>>>> parent of ac457c2... change README

    _logger.info('starting plot',
                 plot_type=args.plot_name)

    _logger.info("reading csv file at {}".format(args.data_source),
                 plot_type=args.plot_name)

    sats_kepler_x = []
    sats_kepler_y = []

    # read csv file

    with open(args.data_source, newline='\n') as csv_file:
        reader = csv.reader(csv_file, delimiter=',')
        # DEBUG: only take first n value
        n = 10000
        cur = 0
        for row in reader:
            if cur >= n:
                break

            if row[0] == "x":
                # we are in header row, skip this
                continue

            sats_kepler_x.append(float(row[0]))
            sats_kepler_y.append(float(row[1]))

            cur += 1

    _logger.info("satellite count: {}".format(len(sats_kepler_x)),
                 plot_type=args.plot_name)

    # Plot data series
    plt.plot(sats_kepler_x, sats_kepler_x, marker='o')

    # Space out ticks ('step size') on x- and y- axes
    x_ticks_list = numpy.arange(
        min(sats_kepler_x), 
        max(sats_kepler_x), 
        (max(sats_kepler_x) - min(sats_kepler_x)) / X_TICKS_NUMBER)
    
    y_ticks_list = numpy.arange(
        min(sats_kepler_y), 
        max(sats_kepler_y), 
        (max(sats_kepler_y) - min(sats_kepler_y)) / Y_TICKS_NUMBER)
    
    plt.xticks(x_ticks_list)
    plt.yticks(y_ticks_list)

    # normalize axes ranges
    #plt.xlim(min(sats_kepler_x), max(sats_kepler_x))
    #plt.ylim(min(sats_kepler_y), max(sats_kepler_y))

    plt.axis("tight")
    plt.tight_layout()

    _logger.info("y-ticks: {}, min(y): {}, max(y): {}".format(y_ticks_list, min(sats_kepler_y), max(sats_kepler_y)),
                plot_type=args.plot_name)

    _logger.info("x-ticks: {}, min(x): {}, max(x): {}".format(x_ticks_list, min(sats_kepler_x), max(sats_kepler_x)),
                plot_type=args.plot_name)

    _logger.info("# of x-ticks: {}, y-ticks: {}".format(len(x_ticks_list), len(y_ticks_list)),
                plot_type=args.plot_name)

    _logger.info("cur xlim: {} ylim: {}".format(plt.xlim(), plt.ylim()),
                plot_type=args.plot_name)

    # Show axis labels
    plt.xlabel("Kepler x-coordinate")
    plt.ylabel("Kepler y-coordinate")

    # Display plot
    plt.show()

def plot_eccent_mean_error(args: argparse.Namespace):
    3 + 2


if __name__ == "__main__":
    # get args passed
    parser = argparse.ArgumentParser(description='Use a kepler datafile to generate matplotlib visualizations.')

    parser.add_argument('plot_name', type=str,
                        help='an integer for the accumulator')
    parser.add_argument('data_source', type=str,
                        help='csv file from preprocessor')

    args = parser.parse_args()

    # now, plot_name = args.plot_name, data_source = args.data_source

    if args.plot_name == "kepler-xypair":
        plot_kepler_xypair(args)
    elif args.plot_name == "eccent-mean_error":
        plot_eccent_mean_error(args)
