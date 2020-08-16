# satellite_analyzer.cpp
This program analyzes the [UCS Satellite Database](https://www.ucsusa.org/resources/satellite-database) to arrive at interesting conclusions about the mass of the Earth. Grab a fresh copy there.

It uses Kepler's 3rd law, which can be summarized as $T^2=\frac{4\pi^2}{GM}a^3$.

It also supports running "variation simulations" to see how changing eccentricity qualifier affects mass estimation statistics.

As cited in LICENSE, it uses ben-strasser's fast-cpp-csv-parser library to parse the database file.

Build with clang++: ``c++ -std=c++11 -Wall satellite_analyzer.cpp -o satellite_analyzer``
Usage: ``satellite_analyzer ucs-db.csv output.csv [eccentricity_qualifier]``

This program is used in an Internal Assessment for the International Baccalaureate physics programme.