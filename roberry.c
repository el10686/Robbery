#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct edge {
    long int a, b;
    int c, none, all;
} Edge;

typedef struct node {
    long int val, index;
    struct node *next;
} Node;

typedef struct subset {
    long int parent, rank;
} Subset;

static Node **adjacency;
static Edge *edge;
static Subset *subsets;
long int *parent, *low, *disc, *visited, *duplicate, cntAll = 0, cntNone = 0, time;

void find_bridges(long int u) {
    Node *p;
    long int v;

    low[u] = disc[u] = ++time;
    p = adjacency[u];
    visited[u] = 1;
    while (p) {
        v = p->val;
        if (!visited[v]) {
            parent[v] = u;
            find_bridges(v);
            low[u] = MIN(low[u], low[v]);
            if (low[v] > disc[u] && !duplicate[p->index])
                edge[p->index].all = 1;
        } else if (v != parent[u])
            low[u] = MIN(low[u], disc[v]);
        p = p->next;
    }
}

int contains(Node *p, long int x) {
    while (p) {
        if (p->val == x) {
            duplicate[p->index] = 1;
            return 1;
        }
        p = p->next;
    }
    return 0;
}

long int find(long int x) {
    if (subsets[x].parent != x)
        subsets[x].parent = find(subsets[x].parent);
    return subsets[x].parent;
}

void merge(long int x, long int y) {
    long int xrepr = find(x);
    long int yrepr = find(y);

    if (subsets[xrepr].rank < subsets[yrepr].rank)
        subsets[xrepr].parent = yrepr;
    else if (subsets[yrepr].rank < subsets[xrepr].rank)
        subsets[yrepr].parent = xrepr;
    else {
        subsets[yrepr].parent = xrepr;
        subsets[xrepr].rank++;
    }
}

int cmpfunc(const void *a, const void *b) {
    const Edge *a1 = a;
    const Edge *b1 = b;
    return (a1->c - b1->c);
}

int main() {
    long int N, M;

    scanf("%ld", &N);
    scanf("%ld", &M);

    edge = malloc(sizeof(Edge) * M);

    for (long int i = 0; i < M; i++) {
        scanf("%ld", &edge[i].a);
        edge[i].a--;
        scanf("%ld", &edge[i].b);
        edge[i].b--;
        scanf("%d", &edge[i].c);
        edge[i].none = edge[i].all = 0;
    }

    qsort(edge,(size_t) M, sizeof(Edge), cmpfunc);

    subsets = malloc(sizeof(Subset) * N);
    adjacency = malloc(sizeof(Node) * N);

    for (long int i = 0; i < N; i++) {
        subsets[i].parent = i;
        subsets[i].rank = 0;
        adjacency[i] = NULL;
    }

    duplicate = malloc(sizeof(long int) * M);
    memset(duplicate, 0 , sizeof(long int) * M);

    parent = malloc(sizeof(long int) * N);
    low = malloc(sizeof(long int) * N);
    disc = malloc(sizeof(long int) * N);
    visited = malloc(sizeof(long int) * N);

    long int i = 0;
    while (i < M) {
        long int j = i;
        while (j < M && edge[j].c == edge[i].c) {
            long int x = find(edge[j].a);
            long int y = find(edge[j].b);
            if (x == y) {
                cntNone++;
                edge[j].none = 1;
            }
            j++;
        }

        for (long int k = 0; k < j; k++) {
            if (!edge[k].none) {
                long int x = find(edge[k].a);
                long int y = find(edge[k].b);
                if(!contains(adjacency[x], y)) {
                    Node *p = malloc(sizeof(Node));
                    p->val = x;
                    p->next = adjacency[y];
                    adjacency[y] = p;
                    p->index = k;
                    p = malloc(sizeof(Node));
                    p->val = y;
                    p->next = adjacency[x];
                    adjacency[x] = p;
                    p->index = k;
                } else
                    duplicate[k] = 1;
            }
        }

        time = 0;
        memset(parent, 0, sizeof(long int) * N);
        memset(low, 0, sizeof(long int) * N);
        memset(disc, 0, sizeof(long int) * N);
        memset(visited, 0, sizeof(long int) * N);

        for (long int k = 0; k < N; k++)
            if (!visited[k] && adjacency[k])
                find_bridges(k);

        for (long int k = 0; k < N; k++) {
            Node *p = adjacency[k];
            adjacency[k] = NULL;
            while (p) {
                Node *temp = p;
                p = p->next;
                free(temp);
            }
        }

        for(; i < j; i++) {
            if (edge[i].all)
                cntAll++;
            long int x = find(edge[i].a);
            long int y = find(edge[i].b);
            if (x != y)
                merge(x, y);
        }
    }

    printf("%ld\n%ld\n%ld\n", cntAll, cntNone, M - cntAll - cntNone);

    return 0;
}