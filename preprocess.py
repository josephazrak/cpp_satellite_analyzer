# -*- coding: utf-8 -*-

'''
    Preprocesses UCS Database csv files and gets them ready for cpp_satellite_analyzer by removing unparsable lines
    and stripping weirdly-encoded characters.

    Usage: python preprocess.py --input [input_file] --output [sanitized_file]

    (c) 2020 Joseph Azrak
'''

import argparse
import codecs
import csv
import random
import string
from os import remove

UCS_DELIMITER = "\t"

def main():
    parser = argparse.ArgumentParser(description='preprocess a UCS satellite database for main analyzer program')
    parser.add_argument('-i', '--input', dest='csv_path', type=str, required=True, help="file path for untouched UCS csv file")
    parser.add_argument('-o', '--output', dest='out_path', type=str, required=True, help="output path for sanitized file (will overwrite)")

    args = parser.parse_args()

    file = codecs.open(args.csv_path, "r", encoding="utf-8", errors="ignore")
    lines = file.readlines()
    csv_header = lines[0]

    phase1_filename = "sanitizer.tmp{}.csv".format(''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for _ in range(16)))
    phase1_file = open(phase1_filename, "w+")
    phase1_file.write(csv_header) # begin building the sanitized file

    # Count number of delimiters in csv header
    expected_delimiter_count = csv_header.count(UCS_DELIMITER)

    for idx in range(1, len(lines)):
        line = lines[idx]
        if line.count(UCS_DELIMITER) == expected_delimiter_count:
            phase1_file.write(line)
        else:
            print("[Phase 1] ⚠️  Will not include line #{} in sanitized copy (expected {}, got {})".format(idx + 1, expected_delimiter_count, line.count(UCS_DELIMITER)))

    phase1_file.close()

    print("[Phase 1] ✅ Finished delimiter sanity check, stashed to [{}]".format(phase1_filename))

    # Phase 2: Parse the temporary file and manually re-calculate eccentricity values.
    #          Unfortunately, the original database file has inconsistencies here, so
    #          this has to be done.

    phase2_out_array = []

    with open(phase1_filename, 'r') as phase1_fh:
         d_reader = csv.DictReader(phase1_fh, delimiter=UCS_DELIMITER)
         corrupted_entries = 0

         for idx, line in enumerate(d_reader):
             # Recalculate eccentricity
             try:
                 period = float((line["Period (minutes)"]).replace(',', ''))
                 perigee = float((line["Perigee (km)"]).replace(',', ''))
                 apogee = float((line["Apogee (km)"]).replace(',', ''))
                 eccent_old = float((line["Eccentricity"]).replace(',', ''))

                 new_eccentricity = (apogee - perigee) / (apogee + perigee)

                 line["Eccentricity"] = new_eccentricity

                 phase2_out_array.append(line)
             except:
                 print("[Phase 2] ⚠️  Will not include (cref) line #{} in sanitized copy (the row is corrupt) ".format(idx + 1)) # Line #'s differ from Phase 1.
                 corrupted_entries += 1

         print("[Phase 2] ✅ Finished eccentricity re-calculation. {} row(s) skipped due to corruption or missing data.".format(corrupted_entries))
         print("[Phase 2] -  Writing output file...")

         with open(args.out_path, "w") as phase2_fh:
             d_writer = csv.DictWriter(phase2_fh, delimiter=UCS_DELIMITER, fieldnames=d_reader.fieldnames)
             d_writer.writeheader()
             for _idx, _line in enumerate(phase2_out_array):
                d_writer.writerow(_line)

         # Remove temporary file
         remove(phase1_filename)

         print("[Phase 2] ✅ All done. Written to {}".format(args.out_path))

if __name__ == "__main__":
    main()
