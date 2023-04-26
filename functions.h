#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

// Graph Node 
// 1. Adjacency List
// 2. Bool Board There

typedef struct _listNode {
    struct _listNode* next;
    int weight;
    int idx;
} listNode;

typedef struct _Node { // Node == Board
    struct _Node* pred;
    bool is_board_here; 
    int idx;
    listNode* head;
} Node;

Node** read_create_graph(FILE* fp, int* rows, int* columns, int* node_count);

void assign_adjacency(int rows, int columns, int node_count, Node* graph[]);
