#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>

typedef struct edge edge_t;
typedef struct node node_t;

struct edge {
    node_t* toNode;
    float   weight;
    void*   data;
    edge_t* next;
};

struct node {
    int     id;
    void*   data;
    edge_t* edges;
};

typedef struct graph {
    node_t** nodes;
    int      nodeCount;
    int      cap;
} graph_t;

graph_t* createGraph(void);
void     freeGraph(graph_t* g);
node_t*  addNode(graph_t* g, int id, void* data);
edge_t*  addEdge(graph_t* g, int fromId, int toId, float weight, void* data);
node_t*  getNode(graph_t* g, int id);
edge_t*  getEdge(graph_t* g, int fromId, int toId);
int      removeNode(graph_t* g, int id);
int      removeEdge(graph_t* g, int fromId, int toId);
void     printGraph(graph_t* g);

#endif
