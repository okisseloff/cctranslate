//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/22/15
//

#ifndef CCTRANSLATE_CCT_SOURCE_EXTRACTOR_H
#define CCTRANSLATE_CCT_SOURCE_EXTRACTOR_H

#include "cct_source.h"

typedef struct cct_source_extractor_ctx { //TODO decide if params could be a part of this
	unsigned long counter;
	char *stream_name;
	char *url;
	int sock;
	int ep; //endpoint
	void *buf;
} cct_source_extractor_ctx;

typedef struct cct_source_extractor_params {
    char *url;
} cct_source_extractor_params;

cct_status cct_init_source_extractor(cct_source_ctx *);

cct_status _cct_source_extractor_open(cct_source_ctx *, void *);
cct_status _cct_source_extractor_read(cct_source_ctx *, cct_sub_entry **, unsigned int *);
cct_status _cct_source_extractor_close(cct_source_ctx *);

#endif //CCTRANSLATE_CCT_SOURCE_EXTRACTOR_H
