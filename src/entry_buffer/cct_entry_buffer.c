//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#include <stdlib.h>
#include <stdio.h>
#include "cct_entry_buffer.h"


cct_status cct_init_entry_buffer(cct_entry_buffer_ctx *ctx, cct_translator_ctx *translator_ctx)
{
    ctx->flush = _cct_entry_buffer_flush;
    ctx->push = _cct_entry_buffer_push;
    ctx->translator = translator_ctx;
    ctx->entry = NULL;

    return CCT_OK;
}

cct_status _cct_entry_buffer_push(cct_entry_buffer_ctx *ctx,
                                  cct_sub_entry *in,
                                  cct_sub_entry **out,
                                  unsigned int *count)
{
    *count = 0;
    cct_language *langs;
    unsigned int langs_count;
    cct_status status = ctx->translator->list_target_langs(ctx->translator, &langs, &langs_count);
    if (status != CCT_OK) {
        fprintf(stderr, "_cct_entry_buffer_push: list_target_langs() failed");
        return CCT_FATAL;
    }

    *out = malloc(langs_count * sizeof(cct_sub_entry));
    if (!*out) {
        perror("_cct_entry_buffer_push: malloc() *out failed");
        return CCT_FATAL;
    }

    for (unsigned int i = 0; i < langs_count; i++) {
        (*out)[i].start_time = in->start_time;
        (*out)[i].end_time = in->end_time;
        (*out)[i].counter = in->counter;
        (*out)[i].lines_count = in->lines_count;
        (*out)[i].lines = malloc((*out)[i].lines_count * sizeof(char*));
        if (!(*out)[i].lines) {
            perror("_cct_entry_buffer_push: malloc() (*out[i])->lines failed");
            return CCT_FATAL;
        }
    }

    for (unsigned int i = 0; i < langs_count; i++) {
        for (unsigned int j = 0; j < in->lines_count; j++) {
            status = ctx->translator->translate(ctx->translator, NULL, langs[i].code, in->lines[j], &(*out)[i].lines[j]);
            if (status != CCT_OK) {
                fprintf(stderr, "_cct_entry_buffer_push: translate() failed");
                return CCT_FATAL;
            }
        }
    }

    *count = langs_count;

    return CCT_OK;
}

cct_status _cct_entry_buffer_flush(cct_entry_buffer_ctx *ctx)
{
    return CCT_OK;
}
