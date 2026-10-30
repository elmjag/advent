#!/usr/bin/env python3
from pathlib import Path

# FILE_NAME = "example-input-day7.txt"
FILE_NAME = "input-day7.txt"


def find_start_beam(line: str):
    for pos, char in enumerate(line):
        if char == "S":
            return {pos}


def find_next_beams(line: str, beams):
    splits = 0
    for pos, char in enumerate(line):
        if char != "^":
            continue
        assert char == "^"
        if pos not in beams:
            continue

        splits += 1
        beams.remove(pos)
        beams.add(pos - 1)
        beams.add(pos + 1)

    return beams, splits


def print_beams(beams, line):
    for pos in range(len(line)):
        if pos in beams:
            print("|", end="")
        else:
            print(".", end="")
    print(f"   {beams=}")


def load_data():
    total_splits = 0

    with Path(FILE_NAME).open() as f:
        line = f.readline()
        beams = find_start_beam(line)
        while (line := f.readline()) != "":
            beams, splits = find_next_beams(line, beams)
            total_splits += splits

    return total_splits


password = load_data()
print(f"password: {password}")
