//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_CCT_SUBRIP_SINK_H
#define CCTRANSLATE_CCT_SUBRIP_SINK_H

#include <stdio.h>
#include "cct_sink.h"

typedef struct cct_sink_subrip_ctx {
    unsigned long counter;
    char *filename;
    FILE *file;
} cct_sink_subrip_ctx;

typedef struct cct_sink_subrip_params {
    char *filename;
} cct_sink_subrip_params;

cct_status _cct_sink_subrip_open(cct_sink_ctx *, void *);
cct_status _cct_sink_subrip_write(cct_sink_ctx *, cct_sub_entry *);
cct_status _cct_sink_subrip_close(cct_sink_ctx *);

cct_status cct_init_sink_subrip(cct_sink_ctx *);

#endif //CCTRANSLATE_CCT_SUBRIP_SINK_H
