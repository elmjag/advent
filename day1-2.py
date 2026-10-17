#!/usr/bin/env python3
from pathlib import Path


NOTCHES = 100


def spinn(start_pos, steps):
    pos = start_pos
    dir = -1 if steps < 0 else 1
    remaining_steps = abs(steps)
    zero_passed = 0

    while remaining_steps > 0:
        pos = (pos + dir) % NOTCHES
        if pos == 0:
            zero_passed += 1

        remaining_steps -= 1

    return pos, zero_passed


class Dial:
    def __init__(self):
        self.pos = 50
        self.password = 0

    def move(self, steps):
        self.pos, zero_passed = spinn(self.pos, steps)
        self.password += zero_passed


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
