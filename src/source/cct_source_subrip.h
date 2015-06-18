//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_CCT_SUBRIP_SOURCE_H
#define CCTRANSLATE_CCT_SUBRIP_SOURCE_H

#include <stdio.h>
#include "cct_source.h"

#define SRT_MAX_LINE_LENGTH 1024
#define SRT_MAX_LINES 15

typedef struct cct_source_subrip_ctx { //TODO decide if params could be a part of this
    unsigned long counter;
    char *filename;
    FILE *file;
} cct_source_subrip_ctx;

typedef struct cct_source_subrip_params {
    char *filename;
} cct_source_subrip_params;

cct_status _cct_source_subrip_open(cct_source_ctx *, void *);
cct_status _cct_source_subrip_read(cct_source_ctx *, cct_sub_entry **, unsigned int *);
cct_status _cct_source_subrip_close(cct_source_ctx *);

cct_status cct_init_source_subrip(cct_source_ctx *);

#endif //CCTRANSLATE_CCT_SUBRIP_SOURCE_H
