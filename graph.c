#include "graph.h"


int removeNode(graph_t* g, int id) {
int idx = index_of_id(g, id);
if (idx < 0) return 0;
node_t* victim = g->nodes[idx];


// drop all incoming edges pointing at victim
for (int i = 0; i < g->nodeCount; ++i) {
node_t* n = g->nodes[i];
if (!n || n == victim) continue;
edge_t** link = &n->edges;
for (edge_t* cur = *link; cur; cur = *link) {
if (cur->toNode == victim) {
*link = cur->next;
free(cur->data);
free(cur);
g->edgeCount--;
continue; // keep same link (we just spliced)
}
link = &cur->next;
}
}


// free victim's outgoing edges + node payload
free_edges(victim->edges);
free(victim->data);
free(victim);


// fill the hole with the last element (order isn't required)
g->nodes[idx] = g->nodes[g->nodeCount - 1];
g->nodes[g->nodeCount - 1] = NULL;
g->nodeCount--;
return 1;
}


void printGraph(graph_t* g) {
if (!g) return;
for (int i = 0; i < g->nodeCount; ++i) {
node_t* n = g->nodes[i];
if (!n) continue;
printf("Node %d:\n", n->id);
for (edge_t* e = n->edges; e; e = e->next) {
int to = e->toNode ? e->toNode->id : -1;
printf(" -> %d (weight: %.2f)", to, e->weight);
if (e->data) printf(" [data]");
putchar('\n');
}
if (!n->edges) printf(" (no outgoing edges)\n");
}
}