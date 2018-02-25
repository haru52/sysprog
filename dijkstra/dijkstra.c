// Dijkstra's Shortest Path First Algorithm

#include <stdio.h>

#define NNODE 6
#define INF 100 // infinity

void dijkstra(int cost[NNODE][NNODE], int i, int dist[NNODE], int prev[NNODE])
{
    
}

int main(void)
{
    int cost[NNODE][NNODE] = {
        {0, 2, 5, 1, INF, INF},
        {2, 0, 3, 2, INF, INF},
        {5, 3, 0, 3, 1, 5},
        {1, 2, 3, 0, 1, INF},
        {INF, INF, 1, 1, 0, 2},
        {INF, INF, 5, INF, 2, 0}
    };
    char node_names[NNODE] = {'A', 'B', 'C', 'D', 'E', 'F'};

    int dist[NNODE];
    int prev[NNODE];
    int i, j;

    for (i = 0; i < NNODE; i++) {
        dijkstra(cost, i, dist, prev);
        printf("root node %c:\n", node_names[i]);
        printf("    [%c,%c,%d] ", node_names[0], node_names[prev[0]], dist[0]);
        for (j = 1; j < NNODE - 1; j++) {
            printf("[%c,%c,%d] ", node_names[j], node_names[prev[j]], dist[j]);
        }
        printf("[%c,%c,%d]\n", node_names[NNODE - 1], node_names[prev[NNODE - 1]], dist[NNODE - 1]);
    }

    return 0;
}
