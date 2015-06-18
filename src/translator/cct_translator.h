//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_CCT_TRANSLATOR_H
#define CCTRANSLATE_CCT_TRANSLATOR_H

#include "../utils.h"

struct cct_translator_ctx;

typedef cct_status (*CCT_FP_TRANSLATOR_OPEN)(struct cct_translator_ctx *, void *);
typedef cct_status (*CCT_FP_TRANSLATOR_IS_AVAILABLE)(struct cct_translator_ctx *, unsigned int *);
typedef cct_status (*CCT_FP_TRANSLATOR_TRANSLATE)(struct cct_translator_ctx *, char*, char*, char *, char **);
typedef cct_status (*CCT_FP_TRANSLATOR_CLOSE)(struct cct_translator_ctx *);
typedef cct_status (*CCT_FP_TRANSLATOR_LIST_AVAILABLE_LANGS)(struct cct_translator_ctx *, cct_language **, unsigned int *);
typedef cct_status (*CCT_FP_TRANSLATOR_LIST_TARGET_LANGS)(struct cct_translator_ctx *, cct_language **, unsigned int *);

typedef struct cct_translator_ctx {
    CCT_FP_TRANSLATOR_OPEN open;
    CCT_FP_TRANSLATOR_IS_AVAILABLE is_available;
    CCT_FP_TRANSLATOR_TRANSLATE translate;
    CCT_FP_TRANSLATOR_CLOSE close;
    CCT_FP_TRANSLATOR_LIST_AVAILABLE_LANGS list_available_langs;
    CCT_FP_TRANSLATOR_LIST_TARGET_LANGS list_target_langs;
    void *ctx_data;
} cct_translator_ctx;

#endif //CCTRANSLATE_CCT_TRANSLATOR_H
