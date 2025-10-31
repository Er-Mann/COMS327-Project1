#include "graph.h"
#include "testgraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void chop_crlf(char* s) {
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r'))
        s[--n] = '\0';
}

static int split_tabs(char* line, char* out[], int max) {
    int k = 0;
    char* p = line;

    while (*p && k < max) {
        while (*p == '\t' || *p == '\n' || *p == '\r')
            ++p;
        if (!*p)
            break;

        out[k++] = p;

        while (*p && *p != '\t' && *p != '\n' && *p != '\r')
            ++p;
        if (*p) {
            *p = '\0';
            ++p;
        }
    }

    return k;
}

static char* sdup(const char* s) {
    size_t n = strlen(s);
    char* p = (char*)malloc(n + 1);
    if (p)
        memcpy(p, s, n + 1);
    return p;
}

int main(void){
    graph_t* G = createGraph();
    if(!G){ fprintf(stderr,"no mem for graph\n"); return 1; }

    char buf[8192];

    if(!fgets(buf,sizeof buf,stdin)) return 0; chop_crlf(buf);
    long poiN = strtol(buf,NULL,10);

    for(long i=0;i<poiN;i++){
        if(!fgets(buf,sizeof buf,stdin)) return 1; chop_crlf(buf);
        char* f[4]; if(split_tabs(buf,f,4)!=4) return 1;
        node_extra_t* nx = (node_extra_t*)calloc(1,sizeof *nx);
        if(!nx) return 1;
        nx->name = sdup(f[1]);
        nx->lat  = (float)strtod(f[2],NULL);
        nx->lon  = (float)strtod(f[3],NULL);
        if(!addNode(G, (int)strtol(f[0],NULL,10), nx)){
            free(nx->name); free(nx); return 1;
        }
    }

    if(!fgets(buf,sizeof buf,stdin)) return 1; chop_crlf(buf);
    long roadN = strtol(buf,NULL,10);


    for(long i=0;i<roadN;i++){
        if(!fgets(buf,sizeof buf,stdin)) return 1; chop_crlf(buf);
        char* f[6]; if(split_tabs(buf,f,6)!=6) return 1;
        edge_extra_t* ex = (edge_extra_t*)calloc(1,sizeof *ex);
        if(!ex) return 1;
        ex->roadName = sdup(f[5]);
        if(!addEdge(G,
                    (int)strtol(f[0],NULL,10),
                    (int)strtol(f[1],NULL,10),
                    (float)strtod(f[2],NULL),
                    ex)){
            free(ex->roadName); free(ex); return 1;
        }
    }

    printGraph(G);
    freeGraph(G);
    return 0;
}
