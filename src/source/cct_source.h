//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_CCT_SOURCE_H
#define CCTRANSLATE_CCT_SOURCE_H

#include "../cct_sub_entry.h"
#include "../utils.h"

//TODO source should implement an event-driven model
struct cct_source_ctx;

typedef cct_status (*CCT_FP_SOURCE_OPEN)(struct cct_source_ctx *, void *);
typedef cct_status (*CCT_FP_SOURCE_READ)(struct cct_source_ctx *, cct_sub_entry **, unsigned int *);
typedef cct_status (*CCT_FP_SOURCE_CLOSE)(struct cct_source_ctx *);

typedef struct cct_source_ctx {
    CCT_FP_SOURCE_OPEN open;
    CCT_FP_SOURCE_READ read;
    CCT_FP_SOURCE_CLOSE close;
    void *ctx_data;
} cct_source_ctx;

#endif //CCTRANSLATE_CCT_SOURCE_H
