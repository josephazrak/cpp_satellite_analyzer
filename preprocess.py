'''
    Preprocesses UCS Database csv files and gets them ready for cpp_satellite_analyzer by removing unparsable lines
    and stripping weirdly-encoded characters.

    Usage: python preprocess.py --input [input_file] --output [sanitized_file]

    (c) 2020 Joseph Azrak
'''

import argparse
import codecs

UCS_DELIMITER = "\t"

def main():
    parser = argparse.ArgumentParser(description='preprocess a UCS satellite database for main analyzer program')
    parser.add_argument('-i', '--input', dest='csv_path', type=str, required=True, help="file path for untouched UCS csv file")
    parser.add_argument('-o', '--output', dest='out_path', type=str, required=True, help="output path for sanitized file (will overwrite)")

    args = parser.parse_args()

    file = codecs.open(args.csv_path, "r", encoding="utf-8", errors="ignore")
    lines = file.readlines()
    csv_header = lines[0]

    sanitized_file = open(args.out_path, "w+")
    sanitized_file.write(csv_header) # begin building the sanitized file

    # Count number of delimiters in csv header
    expected_delimiter_count = csv_header.count(UCS_DELIMITER)

    for idx in range(1, len(lines)):
        line = lines[idx]
        if line.count(UCS_DELIMITER) == expected_delimiter_count:
            sanitized_file.write(line)
        else:
            print("[warn] Will not include line #{} in sanitized copy (expected {}, got {})".format(idx + 1, expected_delimiter_count, line.count(UCS_DELIMITER)))

    print("[done] Built sanitized file at {}".format(args.out_path))
if __name__ == "__main__":
    main()
