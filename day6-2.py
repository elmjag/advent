#!/usr/bin/env python3
from pathlib import Path

# FILE_NAME = "example-input-day6.txt"
FILE_NAME = "input-day6.txt"


def split_num_line(line, ops):
    remaining = line
    for op in ops:
        op_len = len(op)
        yield remaining[: op_len - 1]
        remaining = remaining[op_len:]


def split_ops_line(line):
    part = line[0]
    for c in line[1:]:
        if c != " ":
            if c == "\n":
                yield part + c
            else:
                yield part
            part = c
        else:
            part += c


def load_data():
    data = []
    nums = []
    ops = None

    with Path(FILE_NAME).open() as f:
        while (line := f.readline()) != "":
            if line[0] in {"*", "+"}:
                ops = line
            else:
                nums.append(line)

    ops = list(split_ops_line(ops))
    for row in nums:
        data.append(list(split_num_line(row, ops)))

    data.append(ops)

    return data


def perform_op(r, l, op):
    match op:
        case "+":
            return r + l
        case "*":
            return r * l
        case _:
            assert False, f"unexpected op '{op}'"


def get_nums(data, col):
    num_rows = len(data) - 1
    first_cell = data[0][col]

    for p in range(len(first_cell)):
        num = ""
        for row in range(num_rows):
            num += data[row][col][p]

        yield int(num)


def process(data):
    init_val = {
        "+": 0,
        "*": 1,
    }
    ops = [op.strip() for op in data[-1]]
    res = [init_val[op] for op in ops]

    for col in range(len(ops)):
        for num in get_nums(data, col):
            op = ops[col]
            res[col] = perform_op(res[col], num, op)
    return sum(res)


data = load_data()
password = process(data)
print(f"password: {password}")
