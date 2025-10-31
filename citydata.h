#ifndef CITYDATA_H
#define CITYDATA_H

#include "graph.h"
#include "testgraph.h"

double geoDist(double lat1, double lon1, double lat2, double lon2);
double dijkstra(graph_t* G, node_t* s, node_t* t);
node_t* find_by_name(graph_t* G, const char* name);
int load_graph(const char* path, graph_t** outG);
char* clone_string(const char* s);

#endif
