//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/24/15
//

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "cct_sink.h"
#include "cct_sink_transcript.h"


cct_status cct_init_sink_transcript(cct_sink_ctx * ctx)
{
    ctx->open = _cct_sink_transcript_open;
    ctx->write = _cct_sink_transcript_write;
    ctx->close = _cct_sink_transcript_close;

    cct_sink_transcript_ctx *tctx = malloc(sizeof(cct_sink_transcript_ctx));
    if (!tctx) {
        perror("cct_init_sink_transcript: can't allocate cct_sink_transcript_ctx");
        return CCT_FATAL;
    }

    tctx->file = NULL;
    tctx->filename = NULL;
    ctx->ctx_data = (void *) tctx;

    return CCT_OK;
}

cct_status _cct_sink_transcript_open(cct_sink_ctx *ctx, void *data)
{
    cct_sink_transcript_params *params = (cct_sink_transcript_params *) data;
    cct_sink_transcript_ctx *tctx = (cct_sink_transcript_ctx *) ctx->ctx_data;

    tctx->filename = strdup(params->filename);
    tctx->file = fopen(tctx->filename, "w");
    if (!tctx->file) {
        perror("_cct_sink_transcript_open: can't open input file");
        return CCT_FATAL;
    }
    tctx->is_timed = params->is_timed;
    return CCT_OK;
}

cct_status _cct_sink_transcript_write(cct_sink_ctx *ctx, cct_sub_entry *entry)
{
    cct_sink_transcript_ctx *tctx = (cct_sink_transcript_ctx *) ctx->ctx_data;

    unsigned long st_h = 0, st_m = 0, st_s = 0, st_ms = 0,
        et_h = 0, et_m = 0, et_s = 0, et_ms = 0;


    if (tctx->is_timed) {
        st_h = entry->start_time / (60 * 60 * 1000),
        st_m = (entry->start_time - st_h * (60 * 60 * 1000)) / (60 * 1000),
        st_s = (entry->start_time - st_h * (60 * 60 * 1000) - st_m * (60 * 1000)) / 1000,
        st_ms = (entry->start_time - st_h * (60 * 60 * 1000) - st_m * (60 * 1000) - st_s * 1000);

        et_h = entry->end_time / (60 * 60 * 1000),
        et_m = (entry->end_time - et_h * (60 * 60 * 1000)) / (60 * 1000),
        et_s = (entry->end_time - et_h * (60 * 60 * 1000) - et_m * (60 * 1000)) / 1000,
        et_ms = (entry->end_time - et_h * (60 * 60 * 1000) - et_m * (60 * 1000) - et_s * 1000);
    }

    for (unsigned int i = 0; i < entry->lines_count; i++) {
        if (tctx->is_timed) {
            fprintf(tctx->file, "%02lu:%02lu:%02lu,%03lu|", st_h, st_m, st_s, st_ms);
            fprintf(tctx->file, "%02lu:%02lu:%02lu,%03lu|POP|", et_h, et_m, et_s, et_ms);
        }
        fprintf(tctx->file, "%s\r\n", entry->lines[i]);
    }

    return CCT_OK;
}

cct_status _cct_sink_transcript_close(cct_sink_ctx *ctx)
{
    cct_sink_transcript_ctx *tctx = (cct_sink_transcript_ctx *) ctx->ctx_data;

    free(tctx->filename);

    if (fclose(tctx->file)) {
        perror("_cct_sink_transcript_close: can't close output file");
        return CCT_FATAL;
    }

    tctx->file = NULL;
    tctx->filename = NULL;

    return CCT_OK;
}
