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
            *new_vert = (Node) {.dist = INT_MAX, .head = NULL, .pred = NULL, .heap_pos = vert_idx, .is_board_here = board_here, .board_number = vert_idx, .in_pq = true};

            list[vert_idx] = new_vert;
            vert_idx++;
        }
        fscanf(fp, "\n");
    }

    return list;
}

static void add_adj(Node* base, Node* adj, bool is_collinear, int node_count)
{
    int weight = 2;
    if(base -> board_number >= 0 && adj -> board_number != node_count + 1)
    {
        if(is_collinear == false && adj -> is_board_here == true)
            weight = 1;
        if(is_collinear == true && adj -> is_board_here == true)
            weight = 0;
    }
    else if(base -> board_number == node_count)
    {
        if(adj -> is_board_here == true)
            weight = 0;
        else
            weight = 1;
    }
    else if(adj -> board_number == node_count + 1)
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
        add_adj(left_bank, graph[row_idx * columns], false, node_count);

    for(int idx = 0; idx < node_count; idx++)
    {
        if(x + 1 <= columns) // Right
            add_adj(graph[idx], graph[idx + 1], false, node_count); 
        if(x - 1 > 0) // Left
            add_adj(graph[idx], graph[idx - 1], false, node_count);
        if(y - 1 > 0) // Up
            add_adj(graph[idx], graph[idx - columns], true, node_count);
        if(y + 1 <= rows - 1) // Down 
            add_adj(graph[idx], graph[idx + columns], true, node_count);
        if(y - 1 > 0 && x + 1 <= columns) // Up And To The Right
            add_adj(graph[idx], graph[idx - columns + 1], false, node_count);
        if(y + 1 <= rows - 1 && x + 1 <= columns) // Down And To The Right
            add_adj(graph[idx], graph[idx + columns + 1], false, node_count);
        if(y + 1 <= rows - 1 && x - 1 > 0) // Down And To The Left
            add_adj(graph[idx], graph[idx + columns - 1], false, node_count);
        if(y - 1 > 0 && x - 1 > 0) // Up And To The Left
            add_adj(graph[idx], graph[idx - columns - 1], false, node_count);

        if(x == columns)
            add_adj(graph[idx], graph[node_count + 1], false, node_count);

        x++;
        if(x > columns)
        {
            x = 1;
            y++;
        }
    }
    return;
}

void upward_heapify(Node** heap, Node* node)
{
    int child = node -> heap_pos;
    int parent = (child - 1) / 2;

    Node* new = heap[child];

    while(child > 0 && heap[parent] -> dist > new -> dist)
    {
        heap[child] = heap[parent];
        heap[child] -> heap_pos = child;

        child = parent;
        parent = (child - 1) / 2;
    }
    heap[child] = new;
    heap[child] -> heap_pos = child;
    node -> heap_pos = child;
    return;
}

void downward_heapify(Node** heap, Node* node, int heap_size)
{
    Node* temp = node;

    int i = node -> heap_pos;
    int j = 2 * i + 1;

    while(j <= heap_size - 1)
    {
        if(j < heap_size - 1 && heap[j] -> dist > heap[j + 1] -> dist)
            j++;
        if(temp -> dist <= heap[j] -> dist)
            break;
        else
        {
            heap[i] = heap[j];
            heap[j] -> heap_pos = i;
            i = j;
        }
        j = 2 * i + 1;
    }
    heap[i] = temp;
    heap[i] -> heap_pos = i;
    node -> heap_pos = i;
}

Node* extract_min(Node** graph, int heap_size)
{
    Node* min = graph[0];
    Node* end = graph[heap_size - 1];

    graph[heap_size - 1] = min;
    graph[heap_size - 1] -> heap_pos = heap_size - 1;

    graph[0] = end;
    graph[0] -> heap_pos = 0;

    downward_heapify(graph, graph[0], heap_size - 1);

    return min;
}

Node** dijkstras(Node** graph, Node* start, int length)
{
    int pq_length = length;

    Node** pointer_array = malloc(sizeof(*pointer_array) * length);
    for(int idx = 0; idx < length; idx++) // Left Bank Is Board Count // Right Is + 1
        pointer_array[idx] = &(*graph[idx]);
    
    upward_heapify(graph, graph[length - 2]);

    while(pq_length > 0)
    {
        Node* min = extract_min(graph, pq_length);
        pq_length--;


        listNode* adj = min -> head;
        while(adj != NULL)
        {
            Node* adj_node = pointer_array[adj -> board_number];

            if(adj_node -> in_pq == true && adj_node -> dist > min -> dist + adj -> weight)
            {
                adj_node -> dist = min -> dist + adj -> weight;
                adj_node -> pred = min;

                int heap_pos = pointer_array[adj -> board_number] -> heap_pos;
                upward_heapify(graph, graph[heap_pos]);
            }

            adj = adj -> next;
        } 
    } 

    return pointer_array;
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

void write_output(Node* right_bank, int node_count, FILE* fp, int rows, int columns)
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
        else if(RNode -> board_number == node_count + 1)
            fprintf(fp, "(%d,%d)(%d,%d)\n", py, px, py, px + 1); 
        else if(x == px + 1 && py == y) // Case 1
        {
            fprintf(fp, "(%d,%d)(%d,%d)\n", py, px, y, x); 
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, x, y + 1, x); 
        }
        else if(x == px - 1 && py == y) // Case 2
        {
            fprintf(fp, "(%d,%d)(%d,%d)\n", py, px, y, x); 
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, x, y + 1, x); 
        }
        else if(x == px && py - 1 == y) // Case 3
        {
            if(RNode -> is_board_here)
                fprintf(fp, "(%d,%d)(%d,%d)\n", py, px, py, px + 1); 
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, x, py, px);  
        }
        else if(x == px && py + 1 == y) // Case 4
        {
            if(RNode -> is_board_here)
                fprintf(fp, "(%d,%d)(%d,%d)\n", py, px, py, px + 1); 
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, x, py, px);  
        }
        else if(x == px + 1 && py - 1 == y) // Case 5
        {
            fprintf(fp, "(%d,%d)(%d,%d)\n", py, px, y + 1, x); 
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, x, y + 1, x); 
        }
        else if(x == px + 1 && py + 1 == y) // Case 6
        {
            fprintf(fp, "(%d,%d)(%d,%d)\n", py + 1, px, y, x); 
            fprintf(fp, "(%d,%d)(%d,%d)\n", y, x, y + 1, x); 
        }

        LNode = RNode;
        px = x;
        py = y;
    }

    return;
}

