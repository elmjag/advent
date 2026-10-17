#!/usr/bin/env python3

from pathlib import Path


def get_ranges():
    text = Path("input-day2.txt").read_text().strip()

    for id_range in text.split(","):
        start, end = id_range.split("-")
        yield range(int(start), int(end) + 1)


def get_example_ranges():
    yield range(11, 22 + 1)
    yield range(95, 115 + 1)
    yield range(998, 1012 + 1)
    yield range(1188511880, 1188511890 + 1)
    yield range(222220, 222224 + 1)
    yield range(1698522, 1698528 + 1)
    yield range(446443, 446449 + 1)
    yield range(38593856, 38593862 + 1)


def is_invalid_range(id: str) -> bool:
    half = int(len(id) / 2)
    return id[:half] == id[half:]


def get_invalid_ids(id_ranges):
    for id_range in id_ranges:
        for id in id_range:
            if is_invalid_range(str(id)):
                yield id


invalids = get_invalid_ids(get_ranges())
password = sum(invalids)

print(f"password: {password}")
