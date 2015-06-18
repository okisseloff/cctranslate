//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_CCT_SINK_H
#define CCTRANSLATE_CCT_SINK_H

#include "../cct_sub_entry.h"
#include "../utils.h"

struct cct_sink_ctx;

typedef cct_status (*CCT_FP_SINK_OPEN)(struct cct_sink_ctx *, void *);
typedef cct_status (*CCT_FP_SINK_WRITE)(struct cct_sink_ctx *, cct_sub_entry *);
typedef cct_status (*CCT_FP_SINK_CLOSE)(struct cct_sink_ctx *);

typedef struct cct_sink_ctx {
    CCT_FP_SINK_OPEN open;
    CCT_FP_SINK_WRITE write;
    CCT_FP_SINK_CLOSE close;
    void *ctx_data;
} cct_sink_ctx;

#endif //CCTRANSLATE_CCT_SINK_H
