#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "line_input.h"
#include "vec.h"

// #define FILE_NAME "example-input-day11-1.txt"
#define FILE_NAME "input-day11.txt"

char *trim(char *str);

//
// Node
//

typedef struct
{
    char *name;
    char *conn_names;
    int num_paths;
    Vec conns;
} Node;

Node *node_new(char *name, char *conn_names)
{
    Node *self = malloc(sizeof(Node));

    self->name = strdup(name);
    self->conn_names = strdup(conn_names);
    self->num_paths = -1;
    vec_init(&(self->conns), sizeof(Node *));

    return self;
}

Vec *node_get_conns(Node *self) { return &(self->conns); }

void node_add_conn(Node *self, Node *conn)
{
    vec_push(node_get_conns(self), &conn);
}

void node_print(Node *self)
{
    printf("Node( %s : ", self->name);
    Vec *conns = node_get_conns(self);
    for (size_t i = 0; i < vec_len(conns); i += 1)
    {
        Node *n = *(Node **)vec_get(conns, i);
        printf("%s ", n->name);
    }
    printf(")\n");
}

//
// Graph
//

typedef struct
{
    Vec nodes;
} Graph;

void graph_init(Graph *self) { vec_init(&(self->nodes), sizeof(Node *)); }

Vec *graph_get_nodes(Graph *self) { return &(self->nodes); }

void graph_add_node(Graph *self, Node *node)
{
    vec_push(graph_get_nodes(self), &node);
}

Node *graph_lookup_node(Graph *self, char *name)
{
    Vec *nodes = graph_get_nodes(self);
    size_t len = vec_len(nodes);
    for (size_t i = 0; i < len; i += 1)
    {
        Node *node = *(Node **)vec_get(nodes, i);
        if (strcmp(node->name, name) == 0)
        {
            return node;
        }
    }

    assert(false); // node not found
}

void graph_resolve_refs(Graph *self)
{
    Vec *nodes = graph_get_nodes(self);
    size_t len = vec_len(nodes);
    for (size_t i = 0; i < len; i += 1)
    {
        Node *node = *(Node **)vec_get(nodes, i);
        char *name = trim(strtok(node->conn_names, " "));
        while (name != NULL)
        {
            Node *conn = graph_lookup_node(self, name);
            node_add_conn(node, conn);

            name = trim(strtok(NULL, " "));
        }
    }
}

void graph_print(Graph *self)
{
    Vec *nodes = graph_get_nodes(self);
    size_t len = vec_len(nodes);
    for (size_t i = 0; i < len; i += 1)
    {
        Node **node = vec_get(nodes, i);
        node_print(*node);
    }
}

//
// Utils
//

char *trim(char *str)
{
    if (str == NULL)
    {
        return str;
    }

    for (size_t i = strlen(str) - 1; i > 0 && isspace(str[i]); i -= 1)
    {
        str[i] = '\0';
    }

    return str;
}

//
// Hauptbahnhof
//

int count_paths(Node *start, char *end)
{
    assert(strcmp(start->name, end) != 0);

    int num_paths = 0;

    Vec *conns = node_get_conns(start);
    size_t len = vec_len(conns);
    for (size_t i = 0; i < len; i += 1)
    {
        Node *node = *(Node **)vec_get(conns, i);
        if (node->num_paths != -1)
        {
            num_paths += node->num_paths;
            continue;
        }

        if (strcmp(node->name, end) == 0)
        {
            num_paths += 1;
            continue;
        }

        num_paths += count_paths(node, end);
    }

    start->num_paths = num_paths;
    return num_paths;
}

void load_data(Graph *graph)
{
    LineInput li;
    line_input_init(&li, FILE_NAME);

    Node *node;
    char *line;
    while ((line = line_input_next(&li)) != NULL)
    {
        char *name = strtok(line, ":");
        char *conns = strtok(NULL, ":");
        node = node_new(name, conns);
        graph_add_node(graph, node);
    }

    graph_add_node(graph, node_new("out", ""));

    graph_resolve_refs(graph);
}

int main()
{
    Graph graph;
    graph_init(&graph);

    load_data(&graph);

    int password = count_paths(graph_lookup_node(&graph, "you"), "out");
    printf("password: %d\n", password);
}
