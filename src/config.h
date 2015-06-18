//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_CONFIG_H
#define CCTRANSLATE_CONFIG_H

#include "utils.h"

#define CCT_DEFAULT_OUTPUT_FILENAME "out.srt"

typedef enum cct_source_type {
    CCT_SOURCE_SUBRIP,
    CCT_SOURCE_CCEXTRACTOR
} cct_source_type;

typedef enum cct_sink_type {
    CCT_SINK_SUBRIP
} cct_sink_type;

typedef enum cct_translator_type {
    CCT_TRANSLATOR_GOOGLE
} cct_translator_type;

typedef struct cct_config {
    char *input_filename;
    char *output_filename;
    char *google_api_key;
    char *langs;
    char *source_lang;

    cct_source_type source_type;
    cct_sink_type sink_type;
    cct_translator_type translator_type;

    int list_langs;
    int show_help;
} cct_config;

void cct_config_init(cct_config *);
void cct_config_cleanup(cct_config *);

cct_status cct_parse_cl_args(cct_config *, int, char * const[]);

#endif //CCTRANSLATE_CONFIG_H
