//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#include <stdlib.h>
#include <string.h>

#include "cct_sink.h"
#include "cct_sink_subrip.h"


cct_status cct_init_sink_subrip(cct_sink_ctx * ctx)
{
    ctx->open = _cct_sink_subrip_open;
    ctx->write = _cct_sink_subrip_write;
    ctx->close = _cct_sink_subrip_close;

    cct_sink_subrip_ctx *sctx = malloc(sizeof(cct_sink_subrip_ctx));
    if (!sctx) {
        perror("cct_init_sink_subrip: can't allocate cct_sink_subrip_ctx");
        return CCT_FATAL;
    }

    sctx->file = NULL;
    sctx->filename = NULL;
    sctx->counter = 0;

    ctx->ctx_data = (void *) sctx;

    return CCT_OK;
}

cct_status _cct_sink_subrip_open(cct_sink_ctx *ctx, void *data)
{
    cct_sink_subrip_params *params = (cct_sink_subrip_params *) data;
    cct_sink_subrip_ctx *sctx = (cct_sink_subrip_ctx *) ctx->ctx_data;

    sctx->filename = strdup(params->filename);
    sctx->file = fopen(sctx->filename, "w");
    if (!sctx->file) {
        perror("_cct_sink_subrip_open: can't open input file");
        return CCT_FATAL;
    }
    sctx->counter = 0;

    return CCT_OK;
}

cct_status _cct_sink_subrip_write(cct_sink_ctx *ctx, cct_sub_entry *entry)
{
    cct_sink_subrip_ctx *sctx = (cct_sink_subrip_ctx *) ctx->ctx_data;

    fprintf(sctx->file, "%lu\r\n", entry->counter);

    unsigned long st_h = entry->start_time / (60 * 60 * 1000),
            st_m = (entry->start_time - st_h * (60 * 60 * 1000)) / (60 * 1000),
            st_s = (entry->start_time - st_h * (60 * 60 * 1000) - st_m * (60 * 1000)) / 1000,
            st_ms = (entry->start_time - st_h * (60 * 60 * 1000) - st_m * (60 * 1000) - st_s * 1000);
    fprintf(sctx->file, "%02lu:%02lu:%02lu,%03lu --> ", st_h, st_m, st_s, st_ms);

    unsigned long et_h = entry->end_time / (60 * 60 * 1000),
            et_m = (entry->end_time - et_h * (60 * 60 * 1000)) / (60 * 1000),
            et_s = (entry->end_time - et_h * (60 * 60 * 1000) - et_m * (60 * 1000)) / 1000,
            et_ms = (entry->end_time - et_h * (60 * 60 * 1000) - et_m * (60 * 1000) - et_s * 1000);
    fprintf(sctx->file, "%02lu:%02lu:%02lu,%03lu\r\n", et_h, et_m, et_s, et_ms);

    for (unsigned int i = 0; i < entry->lines_count; i++) {
        fprintf(sctx->file, "%s\r\n", entry->lines[i]);
    }

    fprintf(sctx->file, "\r\n");
    fflush(sctx->file);

    return CCT_OK;
}

cct_status _cct_sink_subrip_close(cct_sink_ctx *ctx)
{
    cct_sink_subrip_ctx *sctx = (cct_sink_subrip_ctx *) ctx->ctx_data;

    free(sctx->filename);

    if (fclose(sctx->file)) {
        perror("_cct_sink_subrip_close: can't close output file");
        return CCT_FATAL;
    }

    sctx->file = NULL;
    sctx->filename = NULL;
    sctx->counter = 0;

    return CCT_OK;
}
