#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "functions.h"
#include <string.h>

static void print_list(listNode* head)
{
    printf(" -> (%d with weight %d) ", head -> idx, head -> weight);
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
    assign_adjacency(rows, columns, node_count, list);

    for(int idx = 0; idx < node_count; idx++)
    {
        printf("%2d", idx);
        if(list[idx] -> head == NULL)
            printf(" NULL");
        else
            print_list(list[idx] -> head);
        printf("\n");
    }

    fclose(fp);
    for(int idx = 0; idx < node_count; idx++)
    {
        if(list[idx] -> head != NULL)
            free_list(list[idx] -> head);
        free(list[idx]);
    }
    free(list);

    

    return EXIT_SUCCESS;
}
