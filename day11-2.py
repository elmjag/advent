#!/usr/bin/env python3

from pprint import pprint
from pathlib import Path

# FILE_NAME = "example-input-day11-2.txt"
FILE_NAME = "input-day11.txt"


class Node:
    def __init__(self, name):
        self.name = name
        self.conns = []
        self.num_paths = None

    def set_connected_nodes(self, conns: list):
        self.conns = conns

    def is_out(self):
        return self.name == "out"

    def __repr__(self):
        conns = [n.name for n in self.conns]
        return f"Node({self.name} -> {conns} {self.num_paths})"


def get_graph(graph_desc):
    graph = {}
    for node_name in graph_desc.keys():
        graph[node_name] = Node(node_name)

    graph["out"] = Node("out")

    for name, conns in graph_desc.items():
        node = graph[name]
        connected_nodes = []
        for n in conns:
            connected_nodes.append(graph[n])
        node.set_connected_nodes(connected_nodes)

    return graph


def load_data():
    graph_desc = {}
    with Path(FILE_NAME).open() as f:
        while (line := f.readline()) != "":
            name, rest = line.split(":")
            graph_desc[name] = rest.strip().split(" ")

    return get_graph(graph_desc)


def count_paths(start: Node, end: str):
    assert start.name != end

    num_paths = 0
    for node in start.conns:
        if node.num_paths is not None:
            num_paths += node.num_paths
            continue

        if node.name == end:
            num_paths += 1
            continue

        num_paths += count_paths(node, end)

    start.num_paths = num_paths
    return num_paths


def get_num_paths(graph, start: str, end: str):
    for node in graph.values():
        node.num_paths = None

    res = count_paths(graph[start], end)
    return res


graph = load_data()

password = (
    get_num_paths(graph, "svr", "fft")
    * get_num_paths(graph, "fft", "dac")
    * get_num_paths(graph, "dac", "out")
)

print(f"password: {password}")
