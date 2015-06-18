//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_CCT_SUB_ENTRY_H
#define CCTRANSLATE_CCT_SUB_ENTRY_H

#define CCT_SUB_LINE_MAX_LENGTH 1024

typedef struct cct_sub_entry {
    unsigned long counter; //index of the entry
    unsigned long start_time; //milliseconds
    unsigned long end_time; //milliseconds
    unsigned int lines_count;
    char ** lines;
} cct_sub_entry;

cct_sub_entry *cct_sub_entry_create();

void cct_sub_entry_print(cct_sub_entry *);
void cct_sub_entry_init(cct_sub_entry *);
void cct_sub_entry_cleanup(cct_sub_entry *);

#endif //CCTRANSLATE_CCT_SUB_ENTRY_H
