#!/usr/bin/env python3
from pathlib import Path

# FILE_NAME = "example-input-day6.txt"
FILE_NAME = "input-day6.txt"


def load_data():
    data = []

    with Path(FILE_NAME).open() as f:
        while (line := f.readline()) != "":
            data.append(line.strip().split())

    return data


def perform_op(r, l, op):
    match op:
        case "+":
            return r + l
        case "*":
            return r * l
        case _:
            assert False, f"unexpected op {op}"


def process(data):
    init_val = {
        "+": 0,
        "*": 1,
    }
    ops = data[-1]
    res = [init_val[op] for op in ops]

    for row in data[:-1]:
        for col, val in enumerate(row):
            op = ops[col]
            res[col] = perform_op(res[col], int(val), op)

    return sum(res)


data = load_data()
password = process(data)
print(f"password: {password}")
