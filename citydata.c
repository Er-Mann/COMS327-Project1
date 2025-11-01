#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "citydata.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384
#endif

static double d2r(double d) {
    return d * (M_PI / 180.0);
}

double geoDist(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000.0;
    double p1 = d2r(lat1);
    double p2 = d2r(lat2);
    double dlat = d2r(lat2 - lat1);
    double dlon = d2r(lon2 - lon1);
    double a = sin(dlat / 2) * sin(dlat / 2)
             + cos(p1) * cos(p2) * sin(dlon / 2) * sin(dlon / 2);
    return 2 * R * atan2(sqrt(a), sqrt(1 - a));
}

double dijkstra(graph_t* G, node_t* s, node_t* t) {
    int N = G->nodeCount;
    double* dist = (double*)malloc(sizeof(double) * N);
    unsigned char* used = (unsigned char*)calloc(N, 1);

    if (!dist || !used) {
        free(dist);
        free(used);
        return -1.0;
    }

    for (int i = 0; i < N; i++) {
        dist[i] = 1e300;
    }

    int si = -1;
    int ti = -1;

    for (int i = 0; i < N; i++) {
        if (G->nodes[i] == s) si = i;
        if (G->nodes[i] == t) ti = i;
    }

    if (si < 0 || ti < 0) {
        free(dist);
        free(used);
        return -1.0;
    }

    dist[si] = 0.0;

    for (int step = 0; step < N; step++) {
        int u = -1;
        double best = 1e301;

        for (int i = 0; i < N; i++) {
            if (!used[i] && dist[i] < best) {
                best = dist[i];
                u = i;
            }
        }

        if (u == -1)
            break;

        used[u] = 1;

        if (u == ti)
            break;

        edge_t* e = G->nodes[u]->edges;

        while (e) {
            int v = -1;

            for (int i = 0; i < N; i++) {
                if (G->nodes[i] == e->toNode) {
                    v = i;
                    break;
                }
            }

            if (v >= 0 && !used[v]) {
                double nd = dist[u] + (double)e->weight;
                if (nd < dist[v]) {
                    dist[v] = nd;
                }
            }

            e = e->next;
        }
    }

    double ans = dist[ti];
    if (ans > 9e299) {
        ans = -1.0;
    }

    free(dist);
    free(used);

    return ans;
}

static void trim(char* s) {
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

static int split_tabs(char* line, char* out[], int kmax) {
    int k = 0;
    char* p = line;

    while (*p && k < kmax) {
        while (*p == '\t' || *p == '\n' || *p == '\r') ++p;
        if (!*p) break;
        out[k++] = p;
        while (*p && *p != '\t' && *p != '\n' && *p != '\r') ++p;
        if (*p) {
            *p = '\0';
            ++p;
        }
    }
    return k;
}

char* clone_string(const char* s) {
    size_t n = strlen(s);
    char* r = (char*)malloc(n + 1);
    if (r) memcpy(r, s, n + 1);
    return r;
}

node_t* find_by_name(graph_t* G, const char* name) {
    for (int i = 0; i < G->nodeCount; i++) {
        node_t* n = G->nodes[i];
        if (!n || !n->data) continue;

        node_extra_t* d = (node_extra_t*)n->data;
        if (d->name && strcmp(d->name, name) == 0)
            return n;
    }
    return NULL;
}

int load_graph(const char* path, graph_t** outG) {
    FILE* f = fopen(path, "r");
    if (!f) return 0;

    graph_t* G = createGraph();
    if (!G) {
        fclose(f);
        return 0;
    }

    char buf[8192];
    char* a[6];

    if (!fgets(buf, sizeof buf, f)) {
        fclose(f);
        return 0;
    }
    trim(buf);
    long N = strtol(buf, NULL, 10);

    for (long i = 0; i < N; i++) {
        if (!fgets(buf, sizeof buf, f)) {
            fclose(f);
            return 0;
        }
        trim(buf);
        if (split_tabs(buf, a, 4) != 4) {
            fclose(f);
            return 0;
        }

        node_extra_t* nx = (node_extra_t*)calloc(1, sizeof *nx);
        if (!nx) {
            fclose(f);
            return 0;
        }

        nx->name = clone_string(a[1]);
        nx->lat = (float)strtod(a[2], NULL);
        nx->lon = (float)strtod(a[3], NULL);

        if (!addNode(G, (int)strtol(a[0], NULL, 10), nx)) {
            fclose(f);
            return 0;
        }
    }

    if (!fgets(buf, sizeof buf, f)) {
        fclose(f);
        return 0;
    }
    trim(buf);
    long M = strtol(buf, NULL, 10);

    for (long i = 0; i < M; i++) {
        if (!fgets(buf, sizeof buf, f)) {
            fclose(f);
            return 0;
        }
        trim(buf);
        if (split_tabs(buf, a, 6) != 6) {
            fclose(f);
            return 0;
        }

        edge_extra_t* ex = (edge_extra_t*)calloc(1, sizeof *ex);
        if (!ex) {
            fclose(f);
            return 0;
        }

        ex->roadName = clone_string(a[5]);

        if (!addEdge(G,
                     (int)strtol(a[0], NULL, 10),
                     (int)strtol(a[1], NULL, 10),
                     (float)strtod(a[2], NULL),
                     ex)) {
            fclose(f);
            return 0;
        }
    }

    fclose(f);
    *outG = G;
    return 1;
}

static void usage(void) {
    printf("Usage: citydata -f <file.tsv> [options]\n");
    printf("  -location <name>\n");
    printf("  -distance <name1> <name2>\n");
    printf("  -diameter\n");
    printf("  -roaddist <name1> <name2>\n");
}

typedef enum {
    OP_LOC,
    OP_DIST,
    OP_DIAM,
    OP_ROAD
} opk_t;

typedef struct {
    opk_t k;
    char* a;
    char* b;
} op_t;

int main(int argc, char** argv) {
    if (argc == 1) {
        usage();
        return 0;
    }

    const char* file = NULL;
    op_t ops[64];
    int opn = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            file = argv[++i];
        }
        else if (strcmp(argv[i], "-location") == 0 && i + 1 < argc) {
            ops[opn++] = (op_t){ OP_LOC, argv[++i], NULL };
        }
        else if (strcmp(argv[i], "-distance") == 0 && i + 2 < argc) {
            ops[opn++] = (op_t){ OP_DIST, argv[++i], argv[++i] };
        }
        else if (strcmp(argv[i], "-diameter") == 0) {
            ops[opn++] = (op_t){ OP_DIAM, NULL, NULL };
        }
        else if (strcmp(argv[i], "-roaddist") == 0 && i + 2 < argc) {
            ops[opn++] = (op_t){ OP_ROAD, argv[++i], argv[++i] };
        }
    }

    if (!file) {
        usage();
        return 0;
    }

    graph_t* G = NULL;
    if (!load_graph(file, &G)) {
        fprintf(stderr, "Could not load %s\n", file);
        return 1;
    }

    for (int i = 0; i < opn; i++) {
        op_t* op = &ops[i];

        if (op->k == OP_LOC) {
            node_t* n = find_by_name(G, op->a);
            if (!n) {
                printf("NOTFOUND\n");
                continue;
            }

            node_extra_t* d = (node_extra_t*)n->data;
            printf("%.6f %.6f\n", (double)d->lat, (double)d->lon);
        }
        else if (op->k == OP_DIST) {
            node_t* a = find_by_name(G, op->a);
            node_t* b = find_by_name(G, op->b);
            if (!a || !b) {
                printf("NOTFOUND\n");
                continue;
            }

            node_extra_t* da = (node_extra_t*)a->data;
            node_extra_t* db = (node_extra_t*)b->data;
            printf("%.3f\n", geoDist(da->lat, da->lon, db->lat, db->lon));
        }
        else if (op->k == OP_DIAM) {
            double best = -1;
            node_extra_t* A = NULL;
            node_extra_t* B = NULL;

            for (int u = 0; u < G->nodeCount; u++) {
                node_extra_t* du = (node_extra_t*)G->nodes[u]->data;
                for (int v = u + 1; v < G->nodeCount; v++) {
                    node_extra_t* dv = (node_extra_t*)G->nodes[v]->data;
                    double d = geoDist(du->lat, du->lon, dv->lat, dv->lon);
                    if (d > best) {
                        best = d;
                        A = du;
                        B = dv;
                    }
                }
            }

            if (A && B) {
                printf("%.5f %.5f %.5f %.5f %.2f\n",
                       (double)A->lat, (double)A->lon,
                       (double)B->lat, (double)B->lon, best);
            }
            else {
                printf("0 0 0 0 0\n");
            }
        }
        else if (op->k == OP_ROAD) {
            node_t* s = find_by_name(G, op->a);
            node_t* t = find_by_name(G, op->b);
            if (!s || !t) {
                printf("NOTFOUND\n");
                continue;
            }

            double d = dijkstra(G, s, t);
            if (d < 0) {
                printf("UNREACHABLE\n");
            }
            else {
                printf("%.3f\n", d);
            }
        }
    }

    freeGraph(G);
    return 0;
}
