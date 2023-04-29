#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include "functions.h"

// function reads the input file assigns the rows and columns and returns the top left node of the graph
Node** read_create_graph(FILE* fp, int* rows, int* columns, int* node_count)
{
    fscanf(fp, "%d %d\n", rows, columns);

    *node_count = 0;
    *node_count = (*rows - 1) * (*columns);

    Node** list = malloc(sizeof(Node) * (*node_count + 2));
    
    int vert_idx = 0;

    while(!feof(fp))
    {
        for(int idx = 0; idx < *columns; idx++)
        {
            bool board_here;
            int board = fgetc(fp) - '0'; 
            if(board)
                board_here = true;
            else
                board_here = false;
            
            Node* new_vert = malloc(sizeof(*new_vert));
            *new_vert = (Node) {.dist = INT_MAX, .head = NULL, .pred = NULL, .node_idx = vert_idx, .is_board_here = board_here, .board_number = vert_idx, .in_pq = true};

            list[vert_idx] = new_vert;
            vert_idx++;
        }
        fscanf(fp, "\n");
    }

    return list;
}

static void add_adj(Node* base, Node* adj, bool is_collinear)
{
    int weight = 2;
    if(base -> node_idx >= 0 && adj -> node_idx != -2)
    {
        if(is_collinear == false && adj -> is_board_here == true)
            weight = 1;
        if(is_collinear == true && adj -> is_board_here == true)
            weight = 0;
    }
    else if(base -> node_idx == -1)
    {
        if(adj -> is_board_here == true)
            weight = 0;
        else
            weight = 1;
    }
    else if(adj -> node_idx == -2)
        weight = 1;
    

    listNode* new = malloc(sizeof(*new));
    *new = (listNode) {.weight = weight, .next = NULL, .board_number = adj -> board_number};

    if(base -> head == NULL)
        base -> head = new;
    else
    {
        listNode* temp = base -> head;
        while(temp -> next != NULL)
            temp = temp -> next;
        temp -> next = new;
    }
}

void assign_adjacency(int rows, int columns, int node_count, Node* graph[])
{
    int x = 1;
    int y = 1;

    Node* left_bank = graph[node_count];
    for(int row_idx = 0; row_idx < rows - 1; row_idx++)
        add_adj(left_bank, graph[row_idx * columns], false);
    
    for(int idx = 0; idx < node_count; idx++)
    {
        //printf("Node: %3d X: %3d Y: %3d\n", idx, x, y);
        
        // End Assign Left
        if(x + 1 <= columns) // Right
            add_adj(graph[idx], graph[idx + 1], false); 
        if(x - 1 > 0) // Left
            add_adj(graph[idx], graph[idx - 1], false);
        if(y - 1 > 0) // Up
            add_adj(graph[idx], graph[idx - columns], true);
        if(y + 1 <= rows - 1) // Down 
            add_adj(graph[idx], graph[idx + columns], true);
        if(y - 1 > 0 && x + 1 <= columns) // Up And To The Right
            add_adj(graph[idx], graph[idx - columns + 1], false);
        if(y + 1 <= rows - 1 && x + 1 <= columns) // Down And To The Right
            add_adj(graph[idx], graph[idx + columns + 1], false);
        if(y + 1 <= rows - 1 && x - 1 > 0) // Down And To The Left
            add_adj(graph[idx], graph[idx + columns - 1], false);
        if(y - 1 > 0 && x - 1 > 0) // Up And To The Left
            add_adj(graph[idx], graph[idx - columns - 1], false);

        if(x == columns)
            add_adj(graph[idx], graph[node_count + 1], false);

        x++;
        if(x > columns)
        {
            x = 1;
            y++;
        }
    }
    return;
}
// Nothing After This Point Should Use The Boards PQ IDX
void upward_heapify(Node** heap, Node* node, int* pq_idx)
{
    int child_idx = pq_idx[node -> board_number];
    int parent_idx = (child_idx - 1) / 2;

    while(heap[parent_idx] -> dist > heap[child_idx] -> dist)
    {
        Node* parent = heap[parent_idx];
        Node* child = heap[child_idx];
        Node** parent_loc = &heap[parent_idx];
        Node** child_loc = &heap[child_idx];

        *parent_loc = child;
        *child_loc = parent;

        pq_idx[child -> board_number] = parent_idx;
        pq_idx[parent -> board_number] = child_idx;

        child_idx = parent_idx;
        parent_idx = (parent_idx - 1) / 2;

        if(parent_idx < 0)
            break;
    }    
}

void downward_heapify(Node** heap, Node* node, int* pq_idx, int pq_size)
{
    int parent_idx = pq_idx[node -> board_number];
    int child_idx = parent_idx * 2 + 1;

    while(child_idx < pq_size)
    {
        Node* parent = heap[parent_idx];
        Node* child = heap[child_idx];

        if(child_idx < pq_size - 1 && child -> dist > heap[child_idx + 1] -> dist)
            child_idx++;

        if(heap[parent_idx] ->dist <= heap[child_idx] -> dist)
            break;
        else
        {
            heap[parent_idx] = child;
            heap[child_idx] = parent;
            pq_idx[child -> board_number] = parent_idx;
            pq_idx[parent -> board_number] = child_idx;
        }

        parent_idx = child_idx;
        child_idx = child_idx * 2 + 1;
    }
}

Node* extract_min(Node** graph, int pq_length, int* pq_idx)
{
    Node* min = graph[0];
    Node* end = graph[pq_length - 1];

    graph[pq_length - 1] = min;
    graph[0] = end;

    pq_idx[min -> board_number] = pq_length - 1;
    pq_idx[end -> board_number] = 0;

    downward_heapify(graph, graph[0], pq_idx, pq_length - 1);

    return min;
}
/*
static void print_heap(int length, Node** heap)
{
    for(int i = 0; i < length; i++)
    {
        printf("IDX: %d Board: %d Dist: %d\n", i, heap[i] -> board_number, heap[i] -> dist);
    }
    printf("\n--------------\n");
}

static void print_idx(int length, int* idxs)
{
    for(int i = 0; i < length; i++)
    {
        printf("Board: %d At Idx: %d\n", i, idxs[i]);
    }
    printf("\n--------------\n");
}
*/
int* dijkstras(Node** graph, Node* start, int length)
{
    int pq_length = length;
    int* pq_idx = malloc(sizeof(*pq_idx) * length);
    for(int idx = 0; idx < length; idx++) // Left Bank Is Board Count // Right Is + 1
        pq_idx[idx] = graph[idx] -> board_number;
    
    upward_heapify(graph, graph[length - 2], pq_idx);

    while(pq_length > 0)
    {
        Node* min = extract_min(graph, pq_length, pq_idx);
        pq_length--;

        listNode* adj = min -> head;
        while(adj != NULL)
        {
            Node* adj_node = graph[pq_idx[adj -> board_number]];

            if(adj_node -> in_pq == true && adj_node -> dist > min -> dist + adj -> weight)
            {
                adj_node -> dist = min -> dist + adj -> weight;
                adj_node -> pred = min;

                upward_heapify(graph, graph[pq_idx[adj -> board_number]], pq_idx);
            }

            adj = adj -> next;
        } 
    }
    return pq_idx;
}

static void push(StackNode** stacktop, StackNode* new)
{
    if(*stacktop != NULL)
        new -> prev = *stacktop;
    *stacktop = new;
    return;
}

static StackNode* pop(StackNode** stacktop)
{
    StackNode* popper = *stacktop;
    *stacktop = (*stacktop) -> prev;
    return popper;
}

void write_output(Node* right_bank, int node_count, Node** graph, int* pq_idx, FILE* fp, int rows, int columns)
{
    Node* curr = right_bank;
    StackNode* stack_top = NULL;
    
    while(curr -> pred != NULL)
    {
        StackNode* new = malloc(sizeof(*new));
        *new = (StackNode) {.prev = NULL, .node = curr};
        push(&stack_top, new);
        curr = curr -> pred;
    }
    StackNode* new = malloc(sizeof(*new));
    *new = (StackNode) {.prev = NULL, .node = curr};
    push(&stack_top, new);
    
    // Stack Has Been Created

    StackNode* L = pop(&stack_top);
    Node* LNode = L -> node;
    free(L);

    int y, x;
    int px = 0;
    int py = 0;
    while(stack_top != NULL)
    {
        StackNode* R = pop(&stack_top);
        Node* RNode = R -> node;
        free(R);

        y = (RNode -> board_number) / columns;
        x = RNode -> board_number - (columns * y);

        if(LNode -> board_number == node_count)
        {
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, -1, y, 0);
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, x, y + 1, x);
        }

        if(RNode -> board_number == node_count + 1)
        {
            fprintf(fp, "(%d,%d)(%d,%d)\n", py, px, py, px + 1);
        }
        
        if(x == px + 1 && py == y)
        {
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, px, y, x);
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, x, y + 1, x);
        }
        if(x == px + 1 && y == py - 1)
        {
            fprintf(fp, "(%d,%d)(%d,%d)\n", py, px, y + 1, x);
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, x, y + 1, x);
        }


       // printf("%d:(%d,%d)  %d:(%d,%d)\n",LNode -> board_number, px, py, RNode -> board_number, x, y);
        LNode = RNode;
        px = x;
        py = y;
    }

    return;
}

