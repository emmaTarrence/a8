#ifndef LINKED_LIST_HELPER_H
#define LINKED_LIST_HELPER_H


#include <stdbool.h>
#include <limits.h>

typedef struct Node { 
    int weight; 
    struct Node* next;
    int length; 
} Node;

typedef struct Edge {
    int to_node; 
    int weight_count;
    struct Edge *next; 
    Node* weight;
} Edge; 

typedef struct GraphNode { 
    int label;
    Edge *edges; 
    struct GraphNode *next;
} GraphNode; 

typedef struct DistanceNode {
    int node;              // Node label
    int distance;          // Distance from the source
    struct DistanceNode *next;
} DistanceNode;

typedef struct VisitedNode {
    int node;              // Node label
    bool visited;          // Visited status
    struct VisitedNode *next;
} VisitedNode;

typedef struct ParentNode {
    int node;                 // The current node
    int parent;               // The parent of the current node
    struct ParentNode *next;  // Pointer to the next ParentNode
} ParentNode;

void add_node(GraphNode **head, int label);
GraphNode* find_node(GraphNode *head, int label);
GraphNode* create_node(int label);
Edge* create_edge(int to_node, int *weights, int weight_count);
void add_edge(GraphNode *head, int from_node, int to_node, int *weights, int weight_count);
GraphNode* read_graph_file(const char *filename);
void print_graph(GraphNode *head);
void free_graph(GraphNode *head);

ParentNode* initialize_parent();
void set_parent(ParentNode **head, int node, int parent);
int get_parent(ParentNode *head, int node);
void free_parent_list(ParentNode *head);

DistanceNode* initialize_distances(GraphNode *graph, int from_node);
VisitedNode* initialize_visited(GraphNode *graph);
bool is_visited(VisitedNode *visited, int node);
void update_distance(DistanceNode *distances, int node, int new_distance);
void mark_visited(VisitedNode *visited, int node);
void free_distances(DistanceNode *distances);
void free_visited(VisitedNode *visited);


#endif // GRAPH_H
