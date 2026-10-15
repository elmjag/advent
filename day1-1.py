#!/usr/bin/env python3

from pathlib import Path


class Dial:
    def __init__(self):
        self.pos = 50
        self.password = 0

    def move(self, steps):
        self.pos = (self.pos + steps) % 100
        if self.pos == 0:
            self.password += 1


def moves():
    p = Path("input-day1.txt")
    with p.open() as f:
        while (l := f.readline()) != "":
            val = int(l[1:])
            if l[0] == "L":
                val = -val
            yield val


dial = Dial()

for steps in moves():
    dial.move(steps)

print(f"password: {dial.password}")
