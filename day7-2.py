#!/usr/bin/env python3
from pathlib import Path

# FILE_NAME = "example-input-day7.txt"
FILE_NAME = "input-day7.txt"


def find_start_timeline(line: str) -> int:
    return [1 if s == "S" else 0 for s in line]


def haz_splitters(line: str):
    return "^" in line


def get_timeline(line: str, orig):
    res = orig.copy()

    for idx, char in enumerate(line):
        if orig[idx] == 0:
            continue

        if char == "^":
            res[idx - 1] = orig[idx] + res[idx - 1]
            res[idx] = 0
            res[idx + 1] = orig[idx] + res[idx + 1]

    return res


def load_data():
    with Path(FILE_NAME).open() as f:
        line = f.readline()
        timeline = find_start_timeline(line)

        while (line := f.readline()) != "":
            if haz_splitters(line):
                timeline = get_timeline(line, timeline)

    return sum(timeline)


password = load_data()
print(f"password: {password}")
