#!/usr/bin/env python3

from pathlib import Path
from dataclasses import dataclass

# FILE_NAME = "example-input-day9.txt"
FILE_NAME = "input-day9.txt"


@dataclass
class Point:
    x: int
    y: int


class Line:
    def __init__(self, start: Point, end: Point):
        self.start = start
        self.end = end

        if self.start.x == self.end.x:
            self.is_horizontal = False
        else:
            self.is_horizontal = True

    def is_crossing(self, line: "Line"):
        assert self.is_horizontal != line.is_horizontal

        if self.is_horizontal:
            a = min(self.start.x, self.end.x)
            b = max(self.start.x, self.end.x)

            c = min(line.start.y, line.end.y) + 1
            d = max(line.start.y, line.end.y) - 1

            return a <= line.start.x <= b and c <= self.start.y <= d
        else:
            a = min(self.start.y, self.end.y)
            b = max(self.start.y, self.end.y)

            c = min(line.start.x, line.end.x) + 1
            d = max(line.start.x, line.end.x) - 1

            return a <= line.start.y <= b and c <= self.start.x <= d

    def __repr__(self):
        return f"Line(({self.start}) -> ({self.end}))"


@dataclass
class Floor:
    tiles: list
    hlines: list
    vlines: list

    def crosses_outline(self, line: Line):
        lines = self.vlines if line.is_horizontal else self.hlines

        for l in lines:
            if l.is_crossing(line):
                return True

        return False


def load_data():
    def load_tiles():
        with Path(FILE_NAME).open() as f:
            while (line := f.readline()) != "":
                x, y = [int(v) for v in line.split(",")]
                yield Point(x, y)

    def make_lines(tiles):
        t1 = tiles[0]

        for t2 in tiles[1:]:
            yield Line(t1, t2)
            t1 = t2

        yield Line(t2, tiles[0])

    tiles = list(load_tiles())

    hlines = []
    vlines = []
    for line in make_lines(tiles):
        if line.is_horizontal:
            hlines.append(line)
        else:
            vlines.append(line)

    return Floor(tiles, hlines, vlines)


def calc_area(t1: Point, t2: Point):
    dx = abs(t2.x - t1.x) + 1
    dy = abs(t2.y - t1.y) + 1

    area = dx * dy
    return area


def valid_rect(t1, t2, floor: Floor):

    p1 = t1
    p2 = Point(t2.x, t1.y)
    p3 = t2
    p4 = Point(t1.x, t2.y)

    line1 = Line(p1, p2)
    line2 = Line(p2, p3)
    line3 = Line(p3, p4)
    line4 = Line(p4, p1)

    for line in [line1, line2, line3, line4]:
        if floor.crosses_outline(line):
            return False

    return True


def all_areas(floor: Floor):
    def gen_areas():
        for i, t1 in enumerate(floor.tiles):
            for t2 in floor.tiles[i + 1 :]:
                yield calc_area(t1, t2), t1, t2,

    return sorted(gen_areas(), key=lambda e: e[0], reverse=True)


def first_valid(areas, floor):
    for i, (area, t1, t2) in enumerate(areas):
        if i % 3000 == 0:
            print(".", end="", flush=True)

        if valid_rect(t1, t2, floor):
            if i > 3000:
                print()
            return area

    assert False, "no valid area found"


floor = load_data()
areas = all_areas(floor)
password = first_valid(areas, floor)
print(f"password: {password}")
