//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/22/15
//

#include <stdlib.h>
#include <string.h>
#include "cct_source_extractor.h"
#include "ccx_sub_entry.pb_c.h"

#define CCT_SE_BUF_SIZE 1024
cct_sub_entry *__cct_source_extractor_pb_to_se(PbSubEntry *);

cct_status cct_init_source_extractor(cct_source_ctx *ctx)
{
    ctx->open = _cct_source_extractor_open;
    ctx->read = _cct_source_extractor_read;
    ctx->close = _cct_source_extractor_close;

    cct_source_extractor_ctx *ectx = malloc(sizeof(cct_source_extractor_ctx));
    if (!ectx) {
        perror("cct_init_source_extractor: can't allocate cct_source_extractor_ctx");
        return CCT_FATAL;
    }

    ctx->ctx_data = (void *) ectx;
    return CCT_OK;
}

cct_status _cct_source_extractor_open(cct_source_ctx *ctx, void *params)
{
    cct_source_extractor_params *p = (cct_source_extractor_params *) params;
    cct_source_extractor_ctx *ectx = (cct_source_extractor_ctx *) ctx->ctx_data;

    ectx->zmq = zmq_ctx_new();
    ectx->sock = zmq_socket(ectx->zmq, ZMQ_SUB);
    ectx->url = strdup(p->url);
    if (!ectx->url) {
        perror("_cct_source_extractor_open: cant strdup() url");
        return CCT_FATAL;
    }

    ectx->stream_name = NULL;
    ectx->counter = 0;
    ectx->buf = malloc(CCT_SE_BUF_SIZE * sizeof(char));

    int rc = zmq_connect(ectx->sock, ectx->url);
    if (rc) {
        fprintf(stderr, "_cct_source_extractor_open: can't connect to %s\n", ectx->url);
        return CCT_FATAL;
    }
    rc = zmq_setsockopt(ectx->sock, ZMQ_SUBSCRIBE, NULL, 0);
    if (rc) {
        fprintf(stderr, "_cct_source_extractor_open: can't setup filter\n");
        return CCT_FATAL;
    }

    return CCT_OK;
}

cct_status _cct_source_extractor_read(cct_source_ctx *ctx, cct_sub_entry **entry, unsigned int *eos)
{
    printf("[read]\n");
    *entry = NULL;
    cct_source_extractor_ctx *ectx = (cct_source_extractor_ctx *) ctx->ctx_data;
    int size = zmq_recv (ectx->sock, ectx->buf, CCT_SE_BUF_SIZE, 0);
    printf("[read] recieved %d\n", size);
    if (size == -1) {
        fprintf(stderr, "_cct_source_extractor_read: can't read socket\n");
        return CCT_FATAL;
    }
    if (size > CCT_SE_BUF_SIZE) {
        fprintf(stderr, "_cct_source_extractor_read: not enough mem allocated for buffer\n");
        return CCT_FATAL;
    }
    printf("[read] ok\n");
    PbSubEntry *msg = pb_sub_entry__unpack(NULL, size, ectx->buf);
    if (!msg) {
        fprintf(stderr, "_cct_source_extractor_read: can't unpack buffer\n");
        return CCT_FATAL;
    }
    printf("[read] continuing\n");
    if (msg->eos) {
        printf("[read] eos\n");
        *eos = 1;
        return CCT_OK;
    }

    *entry = __cct_source_extractor_pb_to_se(msg);
    printf("[read] parsed\n");
    if (!*entry) {
        fprintf(stderr, "_cct_source_extractor_read: can't parse buffer\n");
        return CCT_FATAL;
    }

    printf("[read] free\n");
    pb_sub_entry__free_unpacked(msg, NULL);
    printf("[read] done\n");

    return CCT_OK;
}

cct_status _cct_source_extractor_close(cct_source_ctx *ctx)
{
    cct_source_extractor_ctx *ectx = (cct_source_extractor_ctx *) ctx->ctx_data;
    free(ectx->stream_name);
    free(ectx->url);
    free(ectx->buf);
    zmq_close(ectx->sock);
    zmq_ctx_destroy(ectx->zmq);
    ectx->counter = 0;

    return CCT_OK;
}

cct_sub_entry *__cct_source_extractor_pb_to_se(PbSubEntry *msg)
{
    cct_sub_entry *entry = malloc(sizeof(cct_sub_entry));
    if (!entry) {
        perror("__cct_source_extractor_pb_to_se: malloc() entry failed");
        return NULL;
    }

    //TODO decide types for sub_entry
    entry->counter = msg->counter;
    entry->start_time = msg->start_time;
    entry->end_time = msg->end_time;
    entry->lines_count = msg->lines_count;
    entry->lines = (char **) malloc(entry->lines_count * sizeof(char*));

    for (unsigned int i = 0; i < entry->lines_count; i++) {
        entry->lines[i] = strdup(msg->lines[i]);
        if (!entry->lines[i]) {
            perror("__cct_source_extractor_pb_to_se: strdup() failed");
            return NULL;
        }
    }

    return entry;
}