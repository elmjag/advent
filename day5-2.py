#!/usr/bin/env python3
from pathlib import Path

# FILE_NAME = "example-input-day5.txt"
# FILE_NAME = "test-input-day5.txt"
FILE_NAME = "input-day5.txt"


def load_data():
    ranges = []

    with Path(FILE_NAME).open() as f:
        while (line := f.readline()) != "\n":
            s, e = line.strip().split("-")
            ranges.append((int(s), int(e)))

    return ranges


def merge_ranges(range1, range2):
    if range1[1] >= range2[0]:
        if range1[1] < range2[1]:
            res = range1[0], range2[1]
        else:
            res = range1

        return res

    return None


def squash_first(squashed, unsqushed):
    if len(unsqushed) == 0:
        return squashed

    r1 = unsqushed.pop()
    while len(unsqushed) > 0:
        merged = merge_ranges(r1, unsqushed[-1])
        if merged is None:
            break
        else:
            r1 = merged
            unsqushed.pop()

    return squash_first(squashed + [r1], unsqushed)


def squash_ranges(ranges):
    return squash_first([], sorted(ranges, key=lambda r: r[0], reverse=True))


password = 0
for s, e in squash_ranges(load_data()):
    password += e - s + 1

print(f"password: {password}")
