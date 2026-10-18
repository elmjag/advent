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


def find_biggest_pair(joltages):
    def find_max(acc, j):
        if acc is None:
            return j

        if acc[1] < j[1]:
            return j

        return acc

    return reduce(lambda acc, j: find_max(acc, j), joltages, None)


def get_max_joltage(joltages, on_len):
    if on_len == 0:
        return ""

    if on_len == 1:
        part = list(enumerate(joltages))
    else:
        part = list(enumerate(joltages[: -(on_len - 1)]))

    biggest = find_biggest_pair(part)

    remaining = joltages[biggest[0] + 1 :]

    return str(biggest[1]) + get_max_joltage(remaining, on_len - 1)


def get_total_joltage():
    joltages = [int(get_max_joltage(bank, 12)) for bank in get_banks()]
    return sum(joltages)


password = get_total_joltage()
print(f"password: {password}")
