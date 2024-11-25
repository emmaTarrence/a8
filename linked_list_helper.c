// #include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list_helper.h"
int numNodes;

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

DistanceNode* initialize_distances(GraphNode *graph, int from_node) {
    DistanceNode *head = NULL;
    GraphNode *current = graph;

    while (current != NULL) {
        DistanceNode *new_node = (DistanceNode *)malloc(sizeof(DistanceNode));
        new_node->node = current->label;
        new_node->distance = (current->label == from_node) ? 0 : INT_MAX;
        new_node->next = head;
        head = new_node;

        current = current->next;
    }

    return head;
}

VisitedNode* initialize_visited(GraphNode *graph) {
    VisitedNode *head = NULL;
    GraphNode *current = graph;

    while (current != NULL) {
        VisitedNode *new_node = (VisitedNode *)malloc(sizeof(VisitedNode));
        new_node->node = current->label;
        new_node->visited = false;
        new_node->next = head;
        head = new_node;

        current = current->next;
    }

    return head;
}

ParentNode *initialize_parent() {
    return NULL;  // Start with an empty list
}
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

Edge* create_edge(int to_node, int *weights, int weight_count) {
    Edge *new_edge = (Edge*)malloc(sizeof(Edge));
    if (new_edge == NULL) {
        perror("Error allocating memory for new edge");
        return NULL;
    }
    new_edge->to_node = to_node;
    
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
            first_node = new_weight_node;
            last_node = new_weight_node;
        } else {
            last_node->next = new_weight_node;
            last_node = new_weight_node;
        }
    }

    if (first_node != NULL) {
        last_node->next = first_node;
    }

    new_edge->weight = first_node;
    new_edge->weight_count = weight_count;
    new_edge->next = NULL;
    return new_edge;
}

void add_node(GraphNode **head, int label) {
    if (find_node(*head, label) != NULL) {
        return;  // Node already exists
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

void add_edge(GraphNode *head, int from_node, int to_node, int *weights, int weight_count) {
    GraphNode *from = find_node(head, from_node);
    if (from == NULL) {
        return;
    }

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

GraphNode* read_graph_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    GraphNode *head = NULL;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        int from_node, to_node;
        if (sscanf(line, "%d %d", &from_node, &to_node) == 2) {
            int weights[10];
            int weight_count = 0;
            char *token = strtok(line, " ");
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");

            while (token != NULL && weight_count < 10) {
                weights[weight_count] = atoi(token);
                weight_count++;
                token = strtok(NULL, " ");
            }

            add_node(&head, from_node);
            add_node(&head, to_node);
            add_edge(head, from_node, to_node, weights, weight_count);
        }
    }

    fclose(file);
    return head;
}

void print_graph(GraphNode *head) {
    GraphNode *node = head;
    while (node != NULL) {
        printf("Node %d:\n", node->label);
        Edge *edge = node->edges;
        while (edge != NULL) {
            printf("  -> To Node %d, Weights: ", edge->to_node);
            Node *current_weight = edge->weight;
            if (current_weight != NULL) {
                do {
                    printf("%d ", current_weight->weight);
                    current_weight = current_weight->next;
                } while (current_weight != edge->weight);
            }
            printf("\n");
            edge = edge->next;
        }
        printf("\n");
        node = node->next;
    }
}

void free_distances(DistanceNode *distances) {
    while (distances != NULL) {
        DistanceNode *temp = distances;
        distances = distances->next;
        free(temp);
    }
}

void free_visited(VisitedNode *visited) {
    while (visited != NULL) {
        VisitedNode *temp = visited;
        visited = visited->next;
        free(temp);
    }
}

void free_graph(GraphNode *head) {
    while (head != NULL) {
        GraphNode *node_to_free = head;
        Edge *edge = head->edges;

        while (edge != NULL) {
            Edge *edge_to_free = edge;
            if (edge->weight != NULL) {
                Node *current_weight = edge->weight;
                Node *start = current_weight;
                do {
                    Node *weight_to_free = current_weight;
                    current_weight = current_weight->next;
                    free(weight_to_free);
                } while (current_weight != start);
            }
            edge = edge->next;
            free(edge_to_free);
        }

        head = head->next;
        free(node_to_free);
    }
}

// Implement other functions similarly...

