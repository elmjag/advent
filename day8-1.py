#!/usr/bin/env python3

from pathlib import Path
from math import sqrt

# FILE_NAME = "example-input-day8.txt"
FILE_NAME = "input-day8.txt"


def get_connections_num():
    if "example" in FILE_NAME:
        return 10

    return 1000


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
        return

    connections = l.connections.union(r.connections)
    for box in connections:
        box.connections = connections


def connect_boxes(distances):
    for _, l, r in distances[: get_connections_num()]:
        connect_box_pair(l, r)


def get_unique_connections(boxes):
    conns = []
    included_boxes = set()
    for box in boxes:
        if box in included_boxes:
            continue

        conns.append((len(box.connections), box.connections))
        included_boxes = included_boxes.union(box.connections)

    conns.sort(key=lambda e: e[0], reverse=True)

    return conns


def get_password(connections):
    password = 1
    for n, _ in connections[:3]:
        password *= n

    return password


boxes = load_data()
distances = calc_distances(boxes)
connect_boxes(distances)
connections = get_unique_connections(boxes)
password = get_password(connections)
print(f"password: {password}")
