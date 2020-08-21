# C++ UCS Satellite Database analysis tools

<p align="center"><img src="https://github.com/jazrak/cpp_satellite_analyzer/workflows/C/C++%20CI/badge.svg"></img></p>

This program analyzes the [UCS Satellite Database](https://www.ucsusa.org/resources/satellite-database) to arrive at interesting conclusions about the mass of the Earth. The overarching theory used for this is Kepler's 3rd law.

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
```
Usage: cpp-satellite-analyzer [options] 

Arguments:
--input     	input CSV file for this analysis action [Required]
--output    	output CSV file for this analysis action [Required]
--ecc       	eccentricity qualifier (for non-MEQ mode)
--meq       	enter multiple eccentricity qualifier mode
--meq-min   	minimum eccentricity (for MEQ mode) * 
--meq-max   	maximum eccentricity (for MEQ mode) * 
--meq-steps 	number of steps (for MEQ mode)      *

(* indicates arguments necessary if --meq is passed)
```

This program is used in an Internal Assessment for the International Baccalaureate physics programme.
