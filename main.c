#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include "functions.h"
#include <string.h>

static void print_list(listNode* head)
{
    printf(" -> (%d with weight %d) ", head -> board_number, head -> weight);
    if(head -> next != NULL)
        print_list(head -> next);
    return;
}

static void free_list(listNode* head)
{
    if(head -> next != NULL)
        free_list(head -> next);
    free(head);
    return;
}

int main(int argc, char* argv[])
{
    int rows;
    int columns;
    int node_count;

    FILE* fp = fopen(argv[1], "r");
    if(fp == NULL)
        return EXIT_FAILURE;

    Node** list = read_create_graph(fp, &rows, &columns, &node_count); 
    fclose(fp);

    Node* left_bank = malloc(sizeof(*left_bank));
    *left_bank = (Node) {.dist = 0, .heap_pos = node_count, .head = NULL, .is_board_here = false, .pred = NULL, .board_number = node_count, .in_pq = true};
    
    Node* right_bank = malloc(sizeof(*right_bank));
    *right_bank = (Node) {.dist = INT_MAX, .head = NULL, .heap_pos = node_count + 1, .is_board_here = false, .pred = NULL, .board_number = node_count + 1, .in_pq = true};

    list[node_count] = left_bank;
    list[node_count + 1] = right_bank;

    assign_adjacency(rows, columns, node_count, list); 

    Node** pointer_array = dijkstras(list, left_bank, (node_count + 2));

    fp = fopen(argv[2], "w");
    if(fp == NULL)
        return EXIT_FAILURE;
    write_output(right_bank, node_count, fp, rows, columns);
    fclose(fp);
    for(int idx = 0; idx < node_count + 2; idx++)
    {
        if(list[idx] -> head != NULL)
            free_list(list[idx] -> head);
        free(list[idx]);
    }

    free(list);
    free(pointer_array);

    return EXIT_SUCCESS;
}
