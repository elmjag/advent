#!/usr/bin/env python3

from pathlib import Path

# FILE_NAME = "example-input-day4.txt"
FILE_NAME = "input-day4.txt"


def get_rows():
    def to_bool(val):
        match val:
            case ".":
                return False
            case "@":
                return True
            case _:
                raise Exception(f"unexpected value '{val}'")

    with Path(FILE_NAME).open() as f:
        while (line := f.readline()) != "":
            yield [to_bool(v) for v in line.strip()]


class Grid:
    def __init__(self):
        self.cells = list(get_rows())
        self.width = len(self.cells[0])
        self.height = len(self.cells)

    def get_cells(self):
        for y in range(self.height):
            for x in range(self.width):
                yield x, y

    def valid_cell_coord(self, x, y) -> bool:
        return 0 <= x < self.width and 0 <= y < self.height

    def is_cell_occupied(self, x, y) -> bool:
        return self.cells[y][x]

    def clear_cell(self, x, y):
        self.cells[y][x] = False

    def get_neighbors(self, x, y):
        assert self.valid_cell_coord(x, y)

        def all_neighbors(x, y):
            yield x - 1, y - 1
            yield x, y - 1
            yield x + 1, y - 1
            yield x - 1, y
            pass
            yield x + 1, y
            yield x - 1, y + 1
            yield x, y + 1
            yield x + 1, y + 1

        for nx, ny in all_neighbors(x, y):
            if self.valid_cell_coord(nx, ny):
                yield nx, ny

    def count_occupied_neighbors(self, x, y):
        occupied = 0
        for nx, ny in self.get_neighbors(x, y):
            if self.is_cell_occupied(nx, ny):
                occupied += 1

        return occupied

    def get_accessable_cells(self):
        for x, y in grid.get_cells():
            if not grid.is_cell_occupied(x, y):
                continue

            if grid.count_occupied_neighbors(x, y) < 4:
                yield x, y

    def clear_accessable_cells(self) -> int:
        accessable = list(self.get_accessable_cells())
        for x, y in accessable:
            self.clear_cell(x, y)

        return len(accessable)


password = 0
grid = Grid()

while (cleared := grid.clear_accessable_cells()) > 0:
    password += cleared

print(f"password: {password}")
