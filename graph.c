#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

static int index_of_id(graph_t* g, int id){
    for(int i=0;i<g->nodeCount;i++) if (g->nodes[i] && g->nodes[i]->id==id) return i;
    return -1;
}

static void free_edges(edge_t* e){
    while(e){ edge_t* n=e->next; free(e->data); free(e); e=n; }
}

graph_t* createGraph(void){
    graph_t* g = (graph_t*)calloc(1,sizeof *g);
    if(!g) return NULL;
    g->cap = 16;
    g->nodes = (node_t**)calloc(g->cap, sizeof *g->nodes);
    if(!g->nodes){ free(g); return NULL; }
    return g;
}

void freeGraph(graph_t* g){
    if(!g) return;
    for(int i=0;i<g->nodeCount;i++){
        node_t* n = g->nodes[i];
        if(!n) continue;
        free_edges(n->edges);
        free(n->data);
        free(n);
    }
    free(g->nodes);
    free(g);
}

static int ensure_cap(graph_t* g){
    if(g->nodeCount < g->cap) return 1;
    int nc = g->cap * 2;
    node_t** nn = (node_t**)realloc(g->nodes, nc * sizeof *nn);
    if(!nn) return 0;
    g->nodes = nn; g->cap = nc; return 1;
}

node_t* addNode(graph_t* g, int id, void* data){
    if(!g) return NULL;
    if(index_of_id(g,id)>=0) return NULL;
    if(!ensure_cap(g)) return NULL;
    node_t* n = (node_t*)calloc(1,sizeof *n);
    if(!n) return NULL;
    n->id = id;
    n->data = data;
    n->edges = NULL;
    g->nodes[g->nodeCount++] = n;
    return n;
}

node_t* getNode(graph_t* g, int id){
    int i = index_of_id(g,id);
    return (i<0)?NULL:g->nodes[i];
}

edge_t* addEdge(graph_t* g, int fromId, int toId, float weight, void* data){
    node_t* a = getNode(g, fromId);
    node_t* b = getNode(g, toId);
    if(!a || !b) return NULL;
    edge_t* e = (edge_t*)calloc(1,sizeof *e);
    if(!e) return NULL;
    e->toNode = b;
    e->weight = weight;
    e->data = data;
    e->next = a->edges;
    a->edges = e;
    return e;
}

edge_t* getEdge(graph_t* g, int fromId, int toId){
    node_t* a = getNode(g, fromId);
    node_t* b = getNode(g, toId);
    if(!a || !b) return NULL;
    for(edge_t* e=a->edges; e; e=e->next) if(e->toNode==b) return e;
    return NULL;
}

int removeEdge(graph_t* g, int fromId, int toId){
    node_t* a = getNode(g, fromId);
    node_t* b = getNode(g, toId);
    if(!a || !b) return 0;
    edge_t** link = &a->edges;
    for(edge_t* e=*link; e; e=*link){
        if(e->toNode==b){
            *link = e->next;
            free(e->data);
            free(e);
            return 1;
        } else {
            link = &e->next;
        }
    }
    return 0;
}

int removeNode(graph_t* g, int id){
    int idx = index_of_id(g, id);
    if (idx < 0) return 0;
    node_t* victim = g->nodes[idx];

    for (int i = 0; i < g->nodeCount; ++i) {
        node_t* n = g->nodes[i];
        if (!n || n == victim) continue;
        edge_t** link = &n->edges;
        for (edge_t* cur = *link; cur; cur = *link) {
            if (cur->toNode == victim) {
                *link = cur->next;
                free(cur->data);
                free(cur);
            } else {
                link = &cur->next;
            }
        }
    }

    free_edges(victim->edges);
    free(victim->data);
    free(victim);

    for (int i = idx + 1; i < g->nodeCount; ++i) g->nodes[i-1] = g->nodes[i];
    g->nodes[--g->nodeCount] = NULL;
    return 1;
}

void printGraph(graph_t* g){
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
