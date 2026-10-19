#!/usr/bin/env python3

from pathlib import Path

# FILE_NAME = "example-input-day5.txt"
FILE_NAME = "input-day5.txt"


class IdRanges:
    def __init__(self):
        self.ranges = []

    def add_range(self, ranges_str):
        s, e = ranges_str.split("-")
        self.ranges.append((int(s), int(e)))

    def is_fresh(self, id):
        for s, e in self.ranges:
            if s <= id <= e:
                return True

        return False


def load_data():
    ranges = IdRanges()
    ids = []

    with Path(FILE_NAME).open() as f:
        while (line := f.readline()) != "\n":
            ranges.add_range(line.strip())

        while (line := f.readline()) != "":
            ids.append(int(line.strip()))

    return ranges, ids


ranges, ids = load_data()
password = 0

for id in ids:
    if ranges.is_fresh(id):
        password += 1

print(f"password: {password}")
