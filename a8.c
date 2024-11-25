#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_VERTICES 1000
#define INF INT_MAX

// Structure for an edge in the adjacency list
typedef struct Edge {
    int target;          // Target vertex
    int *weights;        // Array of weights for each period
    struct Edge *next;   // Pointer to the next edge
} Edge;

// Structure for the graph
typedef struct Graph {
    int V;               // Number of vertices
    int N;               // Period of weights
    Edge *adj[MAX_VERTICES]; // Adjacency list
} Graph;

// Structure for a node in the priority queue
typedef struct Node {
    int vertex;
    int step;
    int cost;
} Node;

// Min-heap for priority queue
typedef struct MinHeap {
    Node *nodes[MAX_VERTICES * 10];
    int size;
} MinHeap;

// Function to create a new graph
Graph *create_graph(int V, int N) {
    Graph *graph = malloc(sizeof(Graph));
    graph->V = V;
    graph->N = N;
    for (int i = 0; i < V; ++i) {
        graph->adj[i] = NULL;
    }
    return graph;
}

// Function to add an edge to the graph
void add_edge(Graph *graph, int src, int dest, int *weights) {
    Edge *edge = malloc(sizeof(Edge));
    edge->target = dest;
    edge->weights = weights;
    edge->next = graph->adj[src];
    graph->adj[src] = edge;
}

// Helper function to create a new node for the priority queue
Node *new_node(int vertex, int step, int cost) {
    Node *node = malloc(sizeof(Node));
    node->vertex = vertex;
    node->step = step;
    node->cost = cost;
    return node;
}

// Function to initialize a min-heap
MinHeap *create_min_heap() {
    MinHeap *heap = malloc(sizeof(MinHeap));
    heap->size = 0;
    return heap;
}

// Min-heap utility functions
void swap(Node **a, Node **b) {
    Node *temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(MinHeap *heap, int idx) {
    if (idx && heap->nodes[idx]->cost < heap->nodes[(idx - 1) / 2]->cost) {
        swap(&heap->nodes[idx], &heap->nodes[(idx - 1) / 2]);
        heapify_up(heap, (idx - 1) / 2);
    }
}

void heapify_down(MinHeap *heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size && heap->nodes[left]->cost < heap->nodes[smallest]->cost) {
        smallest = left;
    }

    if (right < heap->size && heap->nodes[right]->cost < heap->nodes[smallest]->cost) {
        smallest = right;
    }

    if (smallest != idx) {
        swap(&heap->nodes[idx], &heap->nodes[smallest]);
        heapify_down(heap, smallest);
    }
}

void insert_min_heap(MinHeap *heap, Node *node) {
    heap->nodes[heap->size] = node;
    heapify_up(heap, heap->size++);
}

Node *extract_min(MinHeap *heap) {
    if (!heap->size) return NULL;
    Node *root = heap->nodes[0];
    heap->nodes[0] = heap->nodes[--heap->size];
    heapify_down(heap, 0);
    return root;
}

// Dijkstra's algorithm with periodic weights
int *dijkstra(Graph *graph, int start, int end, int *path_len) {
    int V = graph->V;
    int N = graph->N;

    // 2D array to store distances for each vertex at each step
    int dist[V][N];
    int prev[V][N];
    bool visited[V][N];

    for (int i = 0; i < V; i++) {
        for (int j = 0; j < N; j++) {
            dist[i][j] = INF;
            prev[i][j] = -1;
            visited[i][j] = false;
        }
    }

    // Initialize the starting vertex
    dist[start][0] = 0;

    MinHeap *heap = create_min_heap();
    insert_min_heap(heap, new_node(start, 0, 0));

    while (heap->size > 0) {
        Node *current = extract_min(heap);
        int u = current->vertex;
        int step = current->step % N;
        int current_cost = current->cost;
        free(current);

        if (visited[u][step]) continue;
        visited[u][step] = true;

        if (u == end) break;

        // Explore neighbors
        Edge *edge = graph->adj[u];
        while (edge) {
            int v = edge->target;
            int next_step = (step + 1) % N;
            int weight = edge->weights[step];
            int new_cost = current_cost + weight;

            if (new_cost < dist[v][next_step]) {
                dist[v][next_step] = new_cost;
                prev[v][next_step] = u;
                insert_min_heap(heap, new_node(v, next_step, new_cost));
            }
            edge = edge->next;
        }
    }

    // Find the minimum cost to reach the end node across all steps
    int min_cost = INF;
    int final_step = -1;
    for (int i = 0; i < N; i++) {
        if (dist[end][i] < min_cost) {
            min_cost = dist[end][i];
            final_step = i;
        }
    }

    if (min_cost == INF) {
        *path_len = 0;
        return NULL;
    }

    // Reconstruct the path
    int *path = malloc(V * sizeof(int));
    *path_len = 0;

    for (int at = end, step = final_step; at != -1; step = (step - 1 + N) % N) {
        path[(*path_len)++] = at;
        at = prev[at][step];
    }

    // Reverse the path to get start → end
    for (int i = 0; i < *path_len / 2; i++) {
        int temp = path[i];
        path[i] = path[*path_len - i - 1];
        path[*path_len - i - 1] = temp;
    }

    return path;
}


// Main function to process input and output
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    int V, N;
    fscanf(file, "%d %d", &V, &N);

    Graph *graph = create_graph(V, N);

    while (!feof(file)) {
        int src, dest, *weights = malloc(N * sizeof(int));
        fscanf(file, "%d %d", &src, &dest);
        for (int i = 0; i < N; ++i) {
            fscanf(file, "%d", &weights[i]);
        }
        add_edge(graph, src, dest, weights);
    }

    fclose(file);

    int start, end;
    while (scanf("%d %d", &start, &end) == 2) {
        int path_len;
        int *path = dijkstra(graph, start, end, &path_len);
        for (int i = 0; i < path_len; i++) {
            if (i > 0) printf(" ");
            printf("%d", path[i]);
        }
        printf("\n");
        free(path);
    }

    return EXIT_SUCCESS;
}
