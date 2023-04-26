#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "functions.h"

// function reads the input file assigns the rows and columns and returns the top left node of the graph
Node** read_create_graph(FILE* fp, int* rows, int* columns, int* node_count)
{
    fscanf(fp, "%d %d\n", rows, columns);

    *node_count = 0;
    *node_count = (*rows - 1) * (*columns);

    Node** list = malloc(sizeof(Node) * (*node_count));
    
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
            *new_vert = (Node) {.head = NULL, .pred = NULL, .idx = vert_idx, .is_board_here = board_here};

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
    if(is_collinear == false && adj -> is_board_here == true)
        weight = 1;
    if(is_collinear == true && adj -> is_board_here == true)
        weight = 0;
    

    listNode* new = malloc(sizeof(*new));
    *new = (listNode) {.weight = weight, .next = NULL, .idx = adj -> idx};

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

    for(int idx = 0; idx < node_count; idx++)
    {
        //printf("Node: %3d X: %3d Y: %3d\n", idx, x, y);
       
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

        x++;
        if(x > columns)
        {
            x = 1;
            y++;
        }
    }
    return;
}
