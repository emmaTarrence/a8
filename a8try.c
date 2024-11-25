#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> // For INT_MAX
#include <stdbool.h> // For boolean types


#define MAX_WEIGHTS 10  // Define a max number of weights per edge
int numNodes = 0; 

typedef struct Node{ 
    int weight; 
    struct Node* next;
    int length; 
}Node;
// Function to print the graph

// Define the structure to represent an edge (from one node to another)
typedef struct Edge {
    int to_node;  // To node label
    Node* weight;  // Array to store weights for the edge
    int weight_count;  // Number of weights for this edge
    struct Edge *next;  // Pointer to the next edge (linked list of edges)
} Edge;


// Define the structure to represent a node
typedef struct GraphNode {
    int label;  // Node label
    Edge *edges;  // Pointer to the list of edges (adjacency list)
    struct GraphNode *next;  // Pointer to the next node in the graph (linked list)
} GraphNode;

// Function to create a new graph node
GraphNode* create_node(int label) {
    GraphNode *new_node = (GraphNode*)malloc(sizeof(GraphNode));
    if (new_node == NULL) {
        perror("Error allocating memory for new node");
        return NULL;
    }
    new_node->label = label;
    new_node->edges = NULL;  // No edges initially
    new_node->next = NULL;  // No next node initially
    numNodes++;
    return new_node;
}

// Function to create a new edge
Edge* create_edge(int to_node, int *weights, int weight_count) {
    Edge *new_edge = (Edge*)malloc(sizeof(Edge));
    if (new_edge == NULL) {
        perror("Error allocating memory for new edge");
        return NULL;
    }
    new_edge->to_node = to_node;
    
    // Create circular linked list of weights
    Node *first_node = NULL, *last_node = NULL;
    for (int i = 0; i < weight_count; i++) {
        Node *new_weight_node = (Node*)malloc(sizeof(Node));
        if (new_weight_node == NULL) {
            perror("Error allocating memory for weight node");
            return NULL;
        }
        new_weight_node->weight = weights[i];
        new_weight_node->next = NULL;
        
        if (first_node == NULL) {
            // First node in the circular list
            first_node = new_weight_node;
            last_node = new_weight_node;
        } else {
            last_node->next = new_weight_node;  // Link the previous node to the new node
            last_node = new_weight_node;  // Move the last pointer to the new node
        }
    }
    
    if (first_node != NULL) {
        last_node->next = first_node;  // Make it circular by linking last node to first
    }

    new_edge->weight = first_node;  // Set the circular weight list as the edge's weight
    new_edge->weight_count = weight_count;
    new_edge->next = NULL;
    return new_edge;
}


// Function to find a node by label (returns NULL if not found)
GraphNode* find_node(GraphNode *head, int label) {
    GraphNode *current = head;
    while (current != NULL) {
        if (current->label == label) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Function to add a new node to the graph (to the end of the linked list)
void add_node(GraphNode **head, int label) {
    if (find_node(*head, label) != NULL) {
        return;  // Node already exists, no need to add it
    }

    GraphNode *new_node = create_node(label);
    if (*head == NULL) {
        *head = new_node;
    } else {
        GraphNode *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

// Function to add an edge between two nodes
void add_edge(GraphNode *head, int from_node, int to_node, int *weights, int weight_count) {
    GraphNode *from = find_node(head, from_node);
    if (from == NULL) {
        return;  // From node does not exist
    }

    // Create a new edge and add it to the adjacency list of the from node
    Edge *new_edge = create_edge(to_node, weights, weight_count);
    if (from->edges == NULL) {
        from->edges = new_edge;
    } else {
        Edge *current = from->edges;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_edge;
    }
}

// Function to read from the file and populate the graph
GraphNode* read_graph_file(const char *filename) {
    FILE *file = fopen(filename, "r");  // Open the file in read mode
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    GraphNode *head = NULL;  // Start with an empty linked list of nodes

    char line[256];  // Buffer to hold each line
    while (fgets(line, sizeof(line), file)) {  // Read each line
        // Remove the newline character at the end of the line if present
        line[strcspn(line, "\n")] = 0;

        // Parse the from and to node labels
        int from_node, to_node;
        if (sscanf(line, "%d %d", &from_node, &to_node) == 2) {
            // Parse the weights (remaining numbers in the line)
            int weights[MAX_WEIGHTS];
            int weight_count = 0;
            char *token = strtok(line, " ");  // Tokenize by spaces
            token = strtok(NULL, " ");  // Skip the "from_node"
            token = strtok(NULL, " ");  // Skip the "to_node"

            // Parse the weights
            while (token != NULL && weight_count < MAX_WEIGHTS) {
                weights[weight_count] = atoi(token);  // Convert string to integer
                weight_count++;
                token = strtok(NULL, " ");  // Get next token (next weight)
            }

            // Add the nodes to the graph if they don't exist
            add_node(&head, from_node);
            add_node(&head, to_node);

            // Add the edge between the nodes
            add_edge(head, from_node, to_node, weights, weight_count);
        }
    }

    fclose(file);  // Close the file
    return head;  // Return the head of the graph
}

// Function to print the graph for verification
void print_graph(GraphNode *head) {
    GraphNode *node = head;
    while (node != NULL) {
        printf("Node %d:\n", node->label);
        Edge *edge = node->edges;
        while (edge != NULL) {
            printf("  -> To Node %d, Weights: ", edge->to_node);
            for (int i = 0; i < edge->weight_count; i++) {
                printf("%d ", edge->weight->weight);
            }
            printf("\n");
            edge = edge->next;
        }
        printf("\n");
        node = node->next;
    }
}

// Function to free allocated memory for the graph
void free_graph(GraphNode *head) {
    GraphNode *node = head;
    while (node != NULL) {
        GraphNode *to_free = node;
        Edge *edge = node->edges;
        while (edge != NULL) {
            Edge *edge_to_free = edge;
            edge = edge->next;
            free(edge_to_free);
        }
        node = node->next;
        free(to_free);
    }
}

#define MAX_NODES 100 // Define a max number of nodes for simplicity

// Helper function to find the minimum distance node not yet visited
int find_min_distance_node(int *distances, bool *visited, int node_count) {
    int min_distance = INT_MAX;
    int min_index = -1;

    for (int i = 0; i < node_count; i++) { // may need to be
        if (!visited[i] && distances[i] < min_distance) {
            min_distance = distances[i];
            min_index = i;
        }
    }
    return min_index;
}
void print_path(int *parent, int node) {
    if (node == -1) {
        return;
    }
    print_path(parent, parent[node]);
    printf("%d ", node);
}
// Function to find the shortest path using Dijkstra's algorithm
void find_shortest_path(GraphNode *graph, int from_node, int to_node) {
    int distances[MAX_NODES];    // Array to store the shortest distance to each node
    bool visited[MAX_NODES];     // Array to track visited nodes
    int parent[MAX_NODES];       // Array to store the parent of each node in the shortest path

    // Initialize distances, visited, and parent arrays
    for(int j = 0; j < MAX_WEIGHTS; j++){ 
    for (int i = 0; i < MAX_NODES; i++) {
        distances[i] = INT_MAX;
        visited[i] = false;
        parent[i] = -1;
    }
    }
    // Set the distance to the starting node as 0
    distances[from_node] = 0;
        // int j = 0; 

    // Dijkstra's algorithm main loop
    for (int i = 0; i < MAX_NODES - 1; i++) {
        // Find the node with the smallest distance that hasn't been visited yet
        int current = find_min_distance_node(distances, visited, MAX_NODES);
        if (current == -1 || current == to_node) {
            break; // No more reachable nodes or reached the target node
        }

        visited[current] = true;

        // Get the current node from the graph
        GraphNode *node = find_node(graph, current);
        if (node == NULL) {
            continue;
        }

        // Update distances for each neighbor
        Edge *edge = node->edges;
//int turn = 0; 
     if (edge->weight->weight == '\0') { 
               // j = 0; // make doubly linked so you can go back?? maybe make circuilar; 
            }
        while (edge != NULL) {
       
            int neighbor = edge->to_node;

            // For simplicity, we'll assume that we're using the first weight of the edge
            printf("i: %d\n", i); 
            printf("weight: %d\n", edge->weight->weight);
            printf("neightbor: %d\n", neighbor);
            printf("current: %d\n", current);

            int weight = edge->weight->weight;

            // Check if a shorter path to the neighbor exists
            if (!visited[neighbor] && distances[current] != INT_MAX &&
                distances[current]+ weight < distances[neighbor]) {
                distances[neighbor] = distances[current] + weight;
                parent[neighbor] = current;
                printf("in if \n");
                    //i--;

            }
//i++;
            edge = edge->next;
        }
        edge->weight = edge->weight->next; 
}

    // Check if there's a path to the destination
    if (distances[to_node] == INT_MAX) {
        printf("No path exists between %d and %d.\n", from_node, to_node);
    } else {
        printf("Shortest path from %d to %d: ", from_node, to_node);
        print_path(parent, to_node);
        printf("\n");
        printf("Total weight: %d\n", distances[to_node]);
    }
}

        // int q[MAX_NODES] , from = 0, rear = 0; 
        // bool visited[MAX_NODES] = {false};

        // visited[from_node] = true;
        // q[rear++] = from_node; 

        // while(from< rear) { 
        //     int curr = q[from++];
        //     printf("%d\n", curr); 
        //     for(int i = 0; i < numNodes; i++) { 
        //         if(adj[curr][i]==1 && !visited[1]) { 

        //         }
        //     })
        // }
//     int distances[MAX_NODES];    // Array to store the shortest distance to each node
//     bool visited[MAX_NODES];     // Array to track visited nodes
//     int parent[MAX_NODES];       // Array to store the parent of each node in the shortest path

//     // Initialize distances, visited, and parent arrays
//     for (int i = 0; i < MAX_NODES; i++) {
//         distances[i] = INT_MAX;
//         visited[i] = false;
//         parent[i] = -1;
//     }

//     // Set the distance to the starting node as 0
//     distances[from_node] = 0;

//     // Iterate through all nodes in the graph
//     for (int i = 0; i < MAX_NODES - 1; i++) {
//         // Find the node with the smallest distance that hasn't been visited yet
//         int current = find_min_distance_node(distances, visited, MAX_NODES);
//         if (current == -1 || current == to_node) {
//             break; // No more reachable nodes or reached the target node
//         }

//         visited[current] = true;

//         // Get the current node from the graph
//         GraphNode *node = find_node(graph, current);
//         if (node == NULL) {
//             continue;
//         }

//         // Update distances for each neighbor
//         Edge *edge = node->edges;
//         while (edge != NULL) {
//             int neighbor = edge->to_node;

//             // Iterate through weights sequentially for this edge
//             for (int weight_index = 0; weight_index < edge->weight_count; weight_index++) {
//                 int weight = edge->weights[weight_index];

//                 // Check if a shorter path to the neighbor exists
//                 if (!visited[neighbor] && distances[current] != INT_MAX &&
//                     distances[current] + weight < distances[neighbor]) {
//                     distances[neighbor] = distances[current] + weight;
//                     parent[neighbor] = current;
//                 }
//             }
//             edge = edge->next;
//         }
//     }

//     // Check if there's a path to the destination
//     if (distances[to_node] == INT_MAX) {
//         printf("No path exists between %d and %d.\n", from_node, to_node);
//     } else {
//         printf("Shortest path from %d to %d: ", from_node, to_node);
//         print_path(parent, to_node);
//         printf("\n");
//         printf("Total weight: %d\n", distances[to_node]);
//     }
// }


int main(int argc, char *argv[]) {
//   /  const char *filename = argv[1];
    GraphNode *graph = read_graph_file("graph.txt"); //change before submitting 

    if (graph == NULL) {
        return 1;  // Error reading file
    }

    // int from_node, to_node;
    // if (scanf("%d %d", &from_node, &to_node) != 2) {
    //     printf("Invalid input\n");
    //     return 1;
    // }
    find_shortest_path(graph, 0,3);

 // print_graph(graph);  // Print the graph for verification
    free_graph(graph);  // Free allocated memory
    return 0;
}
