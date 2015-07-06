//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/22/15
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cct_source_extractor.h"
#include "ccx_sub_entry_message.pb-c.h"

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>


#define CCT_SE_BUF_SIZE 1024
cct_sub_entry *__cct_source_extractor_pb_to_se(CcxSubEntryMessage *);

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

	ectx->sock = nn_socket(AF_SP, NN_SUB);
	if (ectx->sock < 0) {
		perror("_cct_source_extractor_open: can't nn_socket()");
		return CCT_FATAL;
	}

	ectx->url = strdup(p->url);
	if (!ectx->url) {
		perror("_cct_source_extractor_open: cant strdup() url");
		return CCT_FATAL;
	}

	if (nn_setsockopt(ectx->sock, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) < 0) {
		perror("_cct_source_extractor_open: can't nn_setsockopt()");
		return CCT_FATAL;
	}

	ectx->ep = nn_connect(ectx->sock, ectx->url);
	if (ectx->ep < 0) {
		perror("_cct_source_extractor_open: can't nn_connect()");
		return CCT_FATAL;
	}

	ectx->stream_name = NULL;
	ectx->counter = 0;
	ectx->buf = malloc(CCT_SE_BUF_SIZE * sizeof(char));

	return CCT_OK;
}

cct_status _cct_source_extractor_read(cct_source_ctx *ctx, cct_sub_entry **entry, unsigned int *eos)
{
	printf("[read]\n");
	*entry = NULL;
	*eos = 0;
	cct_source_extractor_ctx *ectx = (cct_source_extractor_ctx *) ctx->ctx_data;
	
	ectx->buf = NULL;
	int size = nn_recv(ectx->sock, &ectx->buf, NN_MSG, 0);
	printf("[read] recieved %d bytes\n", size);
	if (size < 0) {
		fprintf(stderr, "_cct_source_extractor_read: can't read socket\n");
		return CCT_FATAL;
	}
	if (size > CCT_SE_BUF_SIZE) {
		fprintf(stderr, "_cct_source_extractor_read: not enough mem allocated for buffer\n");
		return CCT_FATAL;
	}
	printf("[read] ok\n");
	CcxSubEntryMessage *msg = ccx_sub_entry_message__unpack(NULL, size, ectx->buf);
	if (!msg) {
		fprintf(stderr, "_cct_source_extractor_read: can't unpack buffer\n");
		return CCT_FATAL;
	}
	printf("[read] continuing\n");
	if (msg->eos) {
		nn_freemsg (ectx->buf);
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
    ccx_sub_entry_message__free_unpacked(msg, NULL);
	printf("[read] done\n");
	nn_freemsg(ectx->buf);

	return CCT_OK;
}

cct_status _cct_source_extractor_close(cct_source_ctx *ctx)
{
	cct_source_extractor_ctx *ectx = (cct_source_extractor_ctx *) ctx->ctx_data;
	
	ectx->counter = 0;
	
	free(ectx->stream_name);
	free(ectx->url);
	
	nn_shutdown (ectx->sock, ectx->ep);
	
	return CCT_OK;
}

cct_sub_entry *__cct_source_extractor_pb_to_se(CcxSubEntryMessage *msg)
{
	cct_sub_entry *entry = malloc(sizeof(cct_sub_entry));
	if (!entry) {
		perror("__cct_source_extractor_pb_to_se: malloc() entry failed");
		return NULL;
	}

	entry->counter = (unsigned long) msg->counter;
	entry->start_time = (unsigned long) msg->start_time;
	entry->end_time = (unsigned long) msg->end_time;
	entry->lines_count = (int) msg->n_lines;
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