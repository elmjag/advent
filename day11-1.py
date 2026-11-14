#!/usr/bin/env python3

from pprint import pprint
from pathlib import Path

# FILE_NAME = "example-input-day11.txt"
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
        return f"Node({self.name} -> {conns})"


def get_graph(graph_desc):
    graph = {}
    for node_name in graph_desc.keys():
        graph[node_name] = Node(node_name)

    out_node = Node("out")

    for name, conns in graph_desc.items():
        node = graph[name]
        connected_nodes = []
        for n in conns:
            if n != "out":
                connected_nodes.append(graph[n])
            else:
                connected_nodes.append(out_node)
        node.set_connected_nodes(connected_nodes)

    return graph


def load_data():
    graph_desc = {}
    with Path(FILE_NAME).open() as f:
        while (line := f.readline()) != "":
            name, rest = line.split(":")
            graph_desc[name] = rest.strip().split(" ")

    return get_graph(graph_desc)


def count_paths(start: Node):
    assert not start.is_out()

    num_paths = 0
    for node in start.conns:
        if node.num_paths is not None:
            num_paths += node.num_paths
            continue

        if node.is_out():
            num_paths += 1
            continue

        count_paths(node)
        num_paths += node.num_paths

    start.num_paths = num_paths


graph = load_data()
count_paths(graph["you"])
password = graph["you"].num_paths

print(f"password: {password}")
