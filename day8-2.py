#!/usr/bin/env python3

from pathlib import Path
from math import sqrt

# FILE_NAME = "example-input-day8.txt"
FILE_NAME = "input-day8.txt"


class Box:
    def __init__(self, coords):
        self.x, self.y, self.z = coords
        self.connections = set([self])

    def __repr__(self):
        return f"Box({self.x} {self.y} {self.z})"


def parse_line(line: str):
    parts = line.strip().split(",")
    return [int(n) for n in parts]


def load_data():
    def _load():
        with Path(FILE_NAME).open() as f:
            while (line := f.readline()) != "":
                yield Box(parse_line(line))

    return list(_load())


def distance(b1, b2):
    dist = sqrt((b1.x - b2.x) ** 2 + (b1.y - b2.y) ** 2 + (b1.z - b2.z) ** 2)

    return dist


def calc_distances(boxes):
    distances = []

    for i, c1 in enumerate(boxes):
        for c2 in boxes[i + 1 :]:
            dist = distance(c1, c2)
            distances.append((dist, c1, c2))

    distances.sort(key=lambda e: e[0])

    return distances


def connect_box_pair(l: Box, r: Box):
    if l in r.connections:
        assert r in l.connections
        assert r.connections == l.connections
        # already connected
        return False

    connections = l.connections.union(r.connections)
    for box in connections:
        box.connections = connections

    return True


def connect_boxes(distances):
    last_connected = None
    for _, l, r in distances:
        if connect_box_pair(l, r):
            last_connected = (l, r)

    return last_connected


def get_password(last_connected):
    l, r = last_connected

    return l.x * r.x


boxes = load_data()
distances = calc_distances(boxes)
last_connected = connect_boxes(distances)
password = get_password(last_connected)
print(f"password: {password}")
