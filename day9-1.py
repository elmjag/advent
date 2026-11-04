#!/usr/bin/env python3

from pathlib import Path

# FILE_NAME = "example-input-day9.txt"
FILE_NAME = "input-day9.txt"


def load_data():
    def _load():
        with Path(FILE_NAME).open() as f:
            while (line := f.readline()) != "":
                x, y = [int(v) for v in line.split(",")]
                yield x, y

    return list(_load())


def calc_area(t1, t2):
    x1, y1 = t1
    x2, y2 = t2

    dx = abs(x2 - x1) + 1
    dy = abs(y2 - y1) + 1

    area = dx * dy
    return area


def all_areas(tiles):
    for i, t1 in enumerate(tiles):
        for t2 in tiles[i + 1 :]:
            yield calc_area(t1, t2)


tiles = load_data()
password = max(all_areas(tiles))
print(f"password: {password}")
