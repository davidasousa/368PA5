#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef struct _listNode {
    struct _listNode* next;
    int weight;
    int board_number;
} listNode;

typedef struct _Node { // Node == Board
    struct _Node* pred;
    bool is_board_here; 
    bool in_pq;
    int board_number; // The Number Of The Board
    listNode* head;
    int dist; 

    int heap_pos;
} Node;

typedef struct _StackNode {
    struct _StackNode* prev;
    Node* node;
} StackNode;

Node** read_create_graph(FILE* fp, int* rows, int* columns, int* node_count);

void assign_adjacency(int rows, int columns, int node_count, Node* graph[]);

void upward_heapify(Node** heap, Node* node);

void downward_heapify(Node** heap, Node* node, int heap_size);

Node* extract_min(Node** heap, int heap_size);

Node** dijkstras(Node** graph, Node* start, int length);

void write_output(Node* right_bank, int node_count, FILE* fp, int rows, int columns);
