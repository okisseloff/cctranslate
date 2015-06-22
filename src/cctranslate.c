#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "config.h"
#include "cct_sub_entry.h"


#include "source/cct_source.h"
#include "source/cct_source_subrip.h"
#include "source/cct_source_extractor.h"

#include "sink/cct_sink.h"
#include "sink/cct_sink_subrip.h"

#include "translator/cct_translator.h"
#include "translator/cct_translator_gt.h"

#include "entry_buffer/cct_entry_buffer.h"

int main(int argc, char *const argv[])
{
    cct_config config;
    cct_config_init(&config);

    if (cct_parse_cl_args(&config, argc, argv) != CCT_OK) {
        fprintf(stderr, "main: can't parse command line options\n");
        return EXIT_FAILURE;
    }

    if (config.show_help) {
        usage();
        return EXIT_SUCCESS;
    }

    cct_translator_ctx translator_ctx;

    switch (config.translator_type) {
        case CCT_TRANSLATOR_GOOGLE:
            cct_init_translator_gt(&translator_ctx);
            cct_translator_gt_params params;
            if (!config.google_api_key) {
                fprintf(stderr, "main: google api requires an auth key\n");
                return EXIT_FAILURE;
            }
            unsigned int api_is_available = 0;
            if (translator_ctx.is_available(&translator_ctx, &api_is_available) != CCT_OK) {
                fprintf(stderr, "main: Google Translate Api is not available\n");
                return EXIT_FAILURE;
            }
            params.key = strdup(config.google_api_key);
            if (!config.langs) {
                config.langs = strdup("en");
            }
            params.langs = strdup(config.langs);
            if (translator_ctx.open(&translator_ctx, &params) != CCT_OK) {
                fprintf(stderr, "main: can't open google translator_ctx\n");
                return EXIT_FAILURE;
            }
            free(params.key);
            free(params.langs);
            break;

        default:
            fprintf(stderr, "main: unhandled translator type\n");
            return EXIT_FAILURE;
    }

    if (config.list_langs) {
        unsigned int langs_count;
        printf("available languages:\n");
        cct_language *langs;
        if (translator_ctx.list_available_langs(&translator_ctx, &langs, &langs_count) != CCT_OK) {
            fprintf(stderr, "main: Google can't list available languages\n");
        }
        for (int i = 0; i < langs_count; i++) {
            printf("%s: %s\n", langs[i].code, langs[i].name);
        }
        return EXIT_SUCCESS;
    }

    cct_source_ctx source_ctx;

    switch(config.source_type) {
        case CCT_SOURCE_SUBRIP:
        {
            if (cct_init_source_subrip(&source_ctx) != CCT_OK) {
                fprintf(stderr, "main: can't init subrip source\n");
                return EXIT_FAILURE;
            }
            cct_source_subrip_params params;
            params.filename = strdup(config.input_filename);
            if (source_ctx.open(&source_ctx, &params) != CCT_OK) {
                fprintf(stderr, "main: failed to open subrip source_ctx\n");
                return EXIT_FAILURE;
            }
            free(params.filename);
        }
            break;
        case CCT_SOURCE_CCEXTRACTOR:
        {
            if (cct_init_source_extractor(&source_ctx) != CCT_OK) {
                fprintf(stderr, "main: can't init ccextractor source\n");
                return EXIT_FAILURE;
            }
            if (!config.ccextractor_url) {
                fprintf(stdout, "main: ccextractor url was not set, using default \"%s\"\n",
                        CCT_DEFAULT_CCEXTRACTOR_URL);
                config.ccextractor_url = strdup(CCT_DEFAULT_CCEXTRACTOR_URL);
            }
            cct_source_extractor_params params;
            params.url = strdup(config.ccextractor_url);
            if (source_ctx.open(&source_ctx, &params) != CCT_OK) {
                fprintf(stderr, "main: failed to open ccextractor source_ctx\n");
                return EXIT_FAILURE;
            } else {
                fprintf(stderr, "main: connected to ccextractor\n");
            }
            free(params.url);
        }
            break;
    }

    unsigned int target_langs_count;
    cct_language *target_langs;
    if (translator_ctx.list_target_langs(&translator_ctx, &target_langs, &target_langs_count) != CCT_OK) {
        fprintf(stderr, "main: can't list_target_langs\n");
    }
    if (!target_langs) {
        fprintf(stderr, "main: no target langs specified\n");
        return EXIT_FAILURE;
    }

    cct_sink_ctx *sink_ctx = malloc(target_langs_count * sizeof(cct_sink_ctx));
    if (!sink_ctx) {
        perror("main: can't allocate sink_ctx");
        return EXIT_FAILURE;
    }
    for (unsigned int i = 0; i < target_langs_count; i++) {
        if (config.sink_type == CCT_SINK_SUBRIP) {
            if (cct_init_sink_subrip(&sink_ctx[i]) != CCT_OK) {
                fprintf(stderr, "main: cant init subrip sink\n");
                return EXIT_FAILURE;
            }
            cct_sink_subrip_params params;
            char *output_filename = config.output_filename ? config.output_filename : CCT_DEFAULT_OUTPUT_FILENAME;
            params.filename = compose_filename(output_filename, target_langs[i].code);
            if (sink_ctx[i].open(&sink_ctx[i], &params) != CCT_OK) {
                fprintf(stderr, "main: can't open subrip sink");
                return EXIT_FAILURE;
            }
            free(params.filename);
        } else {
            fprintf(stderr, "main: unhandled sink type\n");
            return EXIT_FAILURE;
        }
    }

    cct_entry_buffer_ctx entry_buffer_ctx;
    if (cct_init_entry_buffer(&entry_buffer_ctx, &translator_ctx) != CCT_OK) {
        fprintf(stderr, "main: can't init entry buffer\n");
        return EXIT_FAILURE;
    }

    cct_sub_entry *entry;
    unsigned int eos;
    while (source_ctx.read(&source_ctx, &entry, &eos) == CCT_OK) {
        if (eos) {
            printf("end of stream reached\n");
            break;
        }
        printf("entry recieved:\n");
        cct_sub_entry_print(entry);
        cct_sub_entry *entries;
        unsigned int entries_count;
        if (entry_buffer_ctx.push(&entry_buffer_ctx, entry, &entries, &entries_count) != CCT_OK) {
            printf("main: pushing to buffer failed\n");
            return EXIT_FAILURE;
        }
        for (int i = 0; i < entries_count; i++) {
            if (sink_ctx[i].write(&sink_ctx[i], &entries[i]) != CCT_OK) {
                fprintf(stderr, "main: can't write output for \"%s\"\n", target_langs[i]);
            }
            //printf("entry %d:\n", i);
            //cct_sub_entry_print(&entries[i]);
        }
        free(entries);
        free(entry);
    }

    source_ctx.close(&source_ctx);
    translator_ctx.close(&translator_ctx);
    for (int i = 0; i < target_langs_count; i++) {
        sink_ctx[i].close(&sink_ctx[i]);
    }

    free(sink_ctx);
    cct_config_cleanup(&config);

    return EXIT_SUCCESS;
}
