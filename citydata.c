#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "citydata.h"
#include "graph.h"
#include "testgraph.h"
#include "citydata.h"
#ifndef M_PI
#define M_PI 3.1415926535897932384
#endif

static double d2r(double d){ return d * (M_PI / 180.0); }

double geoDist(double lat1, double lon1, double lat2, double lon2){  //haversine formula
    const double R = 6371000.0; /* meters */
    double p1 = d2r(lat1), p2 = d2r(lat2);
    double dlat = d2r(lat2 - lat1), dlon = d2r(lon2 - lon1);
    double a = sin(dlat/2)*sin(dlat/2) + cos(p1)*cos(p2)*sin(dlon/2)*sin(dlon/2);
    return 2 * R * atan2(sqrt(a), sqrt(1 - a));
}

double dijkstra(graph_t* G, node_t* s, node_t* t){
    int N = G->nodeCount;
    double *dist = (double*)malloc(sizeof(double)*N);
    unsigned char *used = (unsigned char*)calloc(N, 1);
    if (!dist || !used) { free(dist); free(used); return -1.0; }

    for (int i=0;i<N;i++) dist[i] = 1e300;

    int si = -1, ti = -1;
    for (int i=0;i<N;i++) {
        if (G->nodes[i] == s) si = i;
        if (G->nodes[i] == t) ti = i;
    }
    if (si < 0 || ti < 0) { free(dist); free(used); return -1.0; }

    dist[si] = 0.0;

    for (int step=0; step<N; step++) {
        int u = -1; double best = 1e301;
        for (int i=0;i<N;i++) if (!used[i] && dist[i] < best) { best = dist[i]; u = i; }
        if (u == -1) break;
        used[u] = 1;
        if (u == ti) break;

        for (edge_t* e = G->nodes[u]->edges; e; e = e->next) {
            int v = -1;
            for (int i=0;i<N;i++) if (G->nodes[i] == e->toNode) { v = i; break; }
            if (v < 0 || used[v]) continue;
            double nd = dist[u] + (double)e->weight;
            if (nd < dist[v]) dist[v] = nd;
        }
    }

    double ans = dist[ti];
    if (ans > 9e299) ans = -1.0;
    free(dist); free(used);
    return ans;
}
