#ifndef GRAPH_H
#define GRAPH_H


#include <stddef.h>


typedef struct edge edge_t;
typedef struct node node_t;


struct edge {
node_t* toNode;
float weight;
void* data; // user payload (edge)
edge_t* next; // next in adjacency list
};


struct node {
int id;
void* data; // user payload (node)
edge_t* edges; // head of outgoing edges
};


typedef struct {
node_t** nodes; // dynamic array of node pointers
int nodeCount;
int edgeCount;
int nodeSpace; // capacity of nodes[]
} graph_t;


// leave prototypes exactly as requested by the spec
graph_t* createGraph();
void freeGraph(graph_t* graph);
node_t* addNode(graph_t* graph, int id, void* data);
edge_t* addEdge(graph_t* graph, int fromId, int toId, float weight, void* data);
node_t* getNode(graph_t* graph, int id);
edge_t* getEdge(graph_t* graph, int fromId, int toId);
int removeNode(graph_t* graph, int id);
int removeEdge(graph_t* graph, int fromId, int toId);
void printGraph(graph_t* graph);


#endif // GRAPH_H