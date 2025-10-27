#ifndef CITYDATA_H
#define CITYDATA_H

double geoDist(double lat1, double lon1, double lat2, double lon2);
double dijkstra(graph_t* G, node_t* s, node_t* t);

#endif
