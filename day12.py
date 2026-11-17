#!/usr/bin/env python3

from pprint import pprint
from pathlib import Path
from dataclasses import dataclass

FILE_NAME = "input-day12.txt"


@dataclass
class Shape:
    pattern: list[str]

    def footprint(self):
        fprnt = 0
        for l in self.pattern:
            for b in l:
                if b == "#":
                    fprnt += 1

        return fprnt


@dataclass
class Region:
    width: int
    height: int
    shapes: list[int]

    def check_footprint(self, shapes):
        req_footprint = 0
        for idx, num in enumerate(self.shapes):
            req_footprint += shapes[idx].footprint() * num

        area = self.width * self.height
        return req_footprint <= area


def load_data():
    def load_lines():
        with Path(FILE_NAME).open() as f:
            while (line := f.readline()) != "":
                yield line.strip()

    def find_first_regions_line(lines):
        for i, line in enumerate(lines):
            if "x" in line:
                return i

        assert False, "no region lines found"

    def parse_shapes(lines):
        for i in range(0, len(lines), 5):
            yield Shape(lines[i + 1 : i + 4])

    def parse_regions(lines):
        for line in lines:
            size, shapes = line.split(":")
            width, height = size.split("x")
            shapes = [int(s) for s in shapes.strip().split(" ")]

            yield Region(int(width), int(height), shapes)

    lines = list(load_lines())
    regions_start = find_first_regions_line(lines)

    shapes = list(parse_shapes(lines[:regions_start]))
    regions = list(parse_regions(lines[regions_start:]))

    return shapes, regions


shapes, regions = load_data()

password = 0
for reg in regions:
    if reg.check_footprint(shapes):
        password += 1

print(f"password: {password}")
