//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#include <stdio.h>
#include <stdlib.h>
#include "cct_sub_entry.h"

void cct_sub_entry_print(cct_sub_entry *entry)
{
    if (!entry) {
        printf("print_entry: entry argument is NULL\n");
        return;
    }

    printf("sub entry #%lu\n", entry->counter);
    printf("start: %lu\n", entry->start_time);
    printf("end: %lu\n", entry->end_time);

    printf("lines count: %d\n", entry->lines_count);
    if (!entry->lines) {
        printf("print_entry: no lines allocated\n");
        return;
    }

    printf("lines:\n");
    for (unsigned int i = 0; i < entry->lines_count; i++) {
        if (!entry->lines[i]) {
            printf("print_entry: line[%d] is not allocated\n", i);
        }
        printf("%s\n", entry->lines[i]);
    }
}

void cct_sub_entry_cleanup(cct_sub_entry *entry)
{
    for (unsigned int i =0; i < entry->lines_count; i++) {
        free(entry->lines[i]);
    }
    free(entry->lines);
    entry->lines = NULL;
}

void cct_sub_entry_init(cct_sub_entry *entry)
{
    entry->counter = 0;
    entry->start_time = 0;
    entry->end_time = 0;
    entry->lines_count = 0;
    entry->lines = NULL;
}

cct_sub_entry *cct_sub_entry_create()
{
    cct_sub_entry *entry = (cct_sub_entry *) malloc(sizeof(cct_sub_entry));
    if (!entry) {
        perror("cct_sub_entry_create: malloc() failed");
        exit(EXIT_FAILURE);
    }
    cct_sub_entry_init(entry);
    return entry;
}
