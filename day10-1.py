#!/usr/bin/env python3

from pathlib import Path
from dataclasses import dataclass
from pprint import pprint

# FILE_NAME = "example-input-day10.txt"
FILE_NAME = "input-day10.txt"


@dataclass
class Machine:
    lights: list[bool]
    buttons: list[int]
    joltages: list[int]


def apply(machine: Machine, sequence):
    lights = [False] * len(machine.lights)

    for buttons in sequence:
        for button in buttons:
            lights[button] = not lights[button]

    res = lights == machine.lights
    return res


def load_data():
    def parse_lights(text: str):
        assert text[0] == "[" and text[-1] == "]"

        for l in text[1:-1]:
            yield True if l == "#" else False

    def parse_button(text: str):
        assert text[0] == "(" and text[-1] == ")"

        return [int(b) for b in text[1:-1].split(",")]

    def parse_buttons(conf):
        return [parse_button(b) for b in conf]

    def parse_joltages(text: str):
        assert text[0] == "{" and text[-1] == "}"

        text = text[1:-1]
        return [int(j) for j in text.split(",")]

    def load():
        with Path(FILE_NAME).open() as f:
            while (line := f.readline()) != "":
                line = line.strip()
                parts = line.split(" ")

                lights = list(parse_lights(parts[0]))
                buttons = parse_buttons(parts[1:-1])
                joltages = parse_joltages(parts[-1])

                yield Machine(lights, buttons, joltages)

    return list(load())


def button_perms(buttons):
    perms = [[b] for b in buttons]

    for p in perms:
        yield p

    while True:
        new_perms = []
        for p in perms:
            for b in buttons:
                np = p + [b]
                yield np
                new_perms.append(np)

        perms = new_perms


def get_shortests_seq(machines):
    def get_shortests(machine):
        for p in button_perms(machine.buttons):
            if apply(machine, p):
                return len(p)

    for machine in machines:
        yield get_shortests(machine)
        print(".", end="", flush=True)
    print()


machines = load_data()
password = sum(get_shortests_seq(machines))
print(f"password: {password}")
