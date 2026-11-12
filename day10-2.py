#!/usr/bin/env python3
from dataclasses import dataclass
from pathlib import Path
from functools import cache
from itertools import combinations

# FILE_NAME = "example-input-day10.txt"
FILE_NAME = "input-day10.txt"


@dataclass
class Machine:
    buttons: tuple
    joltages: tuple[int]


def load_data():
    def parse_button(text: str):
        assert text[0] == "(" and text[-1] == ")"

        return tuple(int(b) for b in text[1:-1].split(","))

    def parse_buttons(conf):
        return tuple(parse_button(b) for b in conf)

    def parse_joltages(text: str):
        assert text[0] == "{" and text[-1] == "}"

        text = text[1:-1]
        return tuple(int(j) for j in text.split(","))

    def load():
        with Path(FILE_NAME).open() as f:
            while (line := f.readline()) != "":
                line = line.strip()
                parts = line.split(" ")

                buttons = parse_buttons(parts[1:-1])
                joltages = parse_joltages(parts[-1])

                yield Machine(buttons, joltages)

    return list(load())


def get_lights(joltages):
    return [j % 2 != 0 for j in joltages]


def button_combinations(machine_buttons, lights):
    def apply(buttons, num_lights):
        res = [False] * num_lights

        for button in buttons:
            for idx in button:
                res[idx] = not res[idx]

        return res

    num_lights = len(lights)

    for n in range(len(machine_buttons) + 1):
        for buttons in combinations(machine_buttons, n):
            if apply(buttons, num_lights) == lights:
                yield buttons


def is_zero_joltage(joltages):
    for j in joltages:
        if j != 0:
            return False

    return True


def is_negative_joltage(joltages):
    for j in joltages:
        if j < 0:
            return True

    return False


@cache
def solve(machine_butttons, joltages):
    def apply(joltages, buttons):
        res = [v for v in joltages]
        for button in buttons:
            for idx in button:
                res[idx] -= 1

        return tuple(v for v in res)

    if is_zero_joltage(joltages):
        return 0

    lights = get_lights(joltages)

    presses = 1_000_000

    for buttons in list(button_combinations(machine_butttons, lights)):
        new_joltages = apply(joltages, buttons)
        if is_negative_joltage(new_joltages):
            continue

        half_new_joltages = tuple(v // 2 for v in new_joltages)

        cur_presses = len(buttons)
        next_presses = solve(machine_butttons, half_new_joltages) * 2
        new_presses = cur_presses + next_presses
        presses = min(presses, new_presses)

    return presses


password = 0
for machinex in load_data():
    res = solve(machinex.buttons, machinex.joltages)
    assert res != 1_000_000
    print(".", end="", flush=True)
    password += res

print(f"\n  password: {password}")
