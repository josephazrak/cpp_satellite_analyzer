<h1 align="center">C++ UCS Satellite Database analysis tools</h1>

<p align="center"><img src="https://github.com/jazrak/cpp_satellite_analyzer/workflows/C/C++%20CI/badge.svg"></img></p>

This program's goal is to calculate the mass of the Earth given orbital data about the satellites around it. Only the [UCS Satellite Database](https://www.ucsusa.org/resources/satellite-database) is supported in this program. The overarching theory used for this is Kepler's 3rd law.

It also supports running "variation simulations" to see how changing eccentricity qualifier affects mass estimation statistics ("MEQ mode").

Some 3rd party libraries are used (see CREDITS). The code itself is licensed under the MIT license; see LICENSE.

## Building
This code uses the Cmake build system. Install cmake to your machine and then simply run the script `build.sh` like this:
```
$ chmod +x build.sh
$ ./build.sh
```
The binary will pop into a new bin/ folder.

## Usage
### Obtaining the database
Download a fresh copy of the Union of Concerned Scientists satellite database [here](https://www.ucsusa.org/resources/satellite-database). Be sure to select the "Database (text format)" link.
### Sanitizing the database
Sometimes, the downloaded csv file contains irregularities and is unparsable. You can use the included `preprocess.py` Python script to sanitize the file like so:
```
$ python preprocess.py --input database.csv --output database_sanitized.csv
```
### Running the tool
```
Usage: cpp-satellite-analyzer [options]

Arguments:
--input     	input UCS satellite database file for this analysis action [required]
--output    	output CSV file for this analysis action                   [required]
--ecc       	eccentricity qualifier (for non-MEQ mode)
--meq       	enter multiple eccentricity qualifier mode
--meq-min   	minimum eccentricity (for MEQ mode)                        *
--meq-max   	maximum eccentricity (for MEQ mode)                        *
--meq-steps 	number of steps (for MEQ mode)                             *

(* indicates arguments necessary if --meq is passed)
```

This program is used in an Internal Assessment for the International Baccalaureate physics programme.
