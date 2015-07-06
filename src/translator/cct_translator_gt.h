//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_CCT_TRANSLATOR_GT_H
#define CCTRANSLATE_CCT_TRANSLATOR_GT_H

#include <curl/curl.h>
#include "cct_translator.h"

#define GOOGLE_TRANSLATE_DEFAULT_LANG "en"
#define GOOGLE_TRANSLATE_API_QUERY "https://www.googleapis.com/language/translate/v2?format=text&key=%s&target=%s&q=%s"
#define GOOGLE_TRANSLATE_API_LANGS "https://www.googleapis.com/language/translate/v2/languages?target=en&key=%s"
#define GOOGLE_TRANSLATE_API_URL "https://www.googleapis.com/language/translate/v2"

typedef struct cct_translator_gt_ctx {
    char *key;
    unsigned int langs_count;
    char **langs;
    CURL *curl;
    cct_language *langs_available;
    unsigned int langs_available_count;
} cct_translator_gt_ctx;

typedef struct cct_translator_gt_params {
    char *key; //API key
    char *langs; //comma-separated languages
} cct_translator_gt_params;

cct_status cct_init_translator_gt(cct_translator_ctx *);

cct_status _cct_translator_gt_open(cct_translator_ctx *, void *);
cct_status _cct_translator_gt_translate(cct_translator_ctx *, char *, char *, char *, char**);
cct_status _cct_translator_gt_is_api_available(cct_translator_ctx *, unsigned int *);
cct_status _cct_translator_gt_close(cct_translator_ctx *);
cct_status _cct_translator_gt_list_available_langs(cct_translator_ctx *, cct_language **, unsigned int *);
cct_status _cct_translator_gt_list_target_langs(cct_translator_ctx *, cct_language **, unsigned int *);

char *__cct_translator_gt_get_translated_text(char *);
char *__cct_translator_gt_request (cct_translator_ctx *, char *, char *);
char *__cct_translator_gt_get_available_langs(cct_translator_ctx *);
int __cct_translator_gt_is_lang_known(cct_translator_ctx *, char *);
int __cct_translator_gt_add_target_lang(cct_translator_ctx *, char *);
cct_language *__cct_translator_gt_json_parse_langs(char *, unsigned int *);

#endif //CCTRANSLATE_CCT_TRANSLATOR_GT_H
