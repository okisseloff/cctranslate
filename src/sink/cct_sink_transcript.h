//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/24/15
//

#ifndef CCTRANSLATE_CCT_SINK_TRANSCRIPT_H
#define CCTRANSLATE_CCT_SINK_TRANSCRIPT_H

#include <stdio.h>
#include "cct_sink.h"

typedef struct cct_sink_transcript_ctx {
    char *filename;
    FILE *file;
    int is_timed;
} cct_sink_transcript_ctx;

typedef struct cct_sink_transcript_params {
    char *filename;
    int is_timed;
} cct_sink_transcript_params;

cct_status _cct_sink_transcript_open(cct_sink_ctx *, void *);
cct_status _cct_sink_transcript_write(cct_sink_ctx *, cct_sub_entry *);
cct_status _cct_sink_transcript_close(cct_sink_ctx *);

cct_status cct_init_sink_transcript(cct_sink_ctx *);

#endif //CCTRANSLATE_CCT_SINK_TRANSCRIPT_H
