#!/usr/bin/env python3
from functools import reduce
from pathlib import Path


def get_example_banks():
    return [
        "987654321111111",
        "811111111111119",
        "234234234234278",
        "818181911112111",
    ]


def get_banks():
    p = Path("input-day3.txt")
    with p.open() as f:
        while (line := f.readline()) != "":
            yield line.strip()


def get_joltage(bank: str):
    def find_max(acc, j):
        if acc is None:
            return j

        if acc[1] < j[1]:
            return j

        return acc

    def get_max(joltages):
        return reduce(lambda acc, j: find_max(acc, j), joltages, None)

    joltages = [(i, int(j)) for (i, j) in enumerate(bank)]

    first = get_max(joltages[:-1])
    second = get_max(joltages[first[0] + 1 :])

    return first[1] * 10 + second[1]


def get_total_joltage():
    joltages = [get_joltage(bank) for bank in get_banks()]
    return sum(joltages)


password = get_total_joltage()
print(f"password: {password}")
