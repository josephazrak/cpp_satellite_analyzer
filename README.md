# C++ UCS Satellite Database analysis tools
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
Usage: 
```
$ cpp_satellite_analyzer_project 
    --input <input csv> 
    --output <output csv> 
    [--meq-mode 
        --meq-min <number>
        --meq-max <number>
        --meq-steps <number>]
```

This program is used in an Internal Assessment for the International Baccalaureate physics programme.
