//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cct_translator_gt.h"
#include "../3rd_party/cJSON.h"
#include "../utils.h"

cct_status cct_init_translator_gt(cct_translator_ctx *ctx)
{
    ctx->open = _cct_translator_gt_open;
    ctx->translate = _cct_translator_gt_translate;
    ctx->is_available = _cct_translator_gt_is_api_available;
    ctx->list_available_langs = _cct_translator_gt_list_available_langs;
    ctx->list_target_langs = _cct_translator_gt_list_target_langs;
    ctx->close = _cct_translator_gt_close;

    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) malloc(sizeof(cct_translator_gt_ctx));
    if (!gt_ctx) {
        perror("init_cct_translator_gt_ctx: malloc() gt_ctx failed\n");
        return CCT_FATAL;
    }
    ctx->ctx_data = (void *) gt_ctx;

    gt_ctx->langs_count = 0;
    gt_ctx->langs = NULL;
    gt_ctx->langs_available_count = 0;
    gt_ctx->langs_available = NULL;
    gt_ctx->key = NULL;
    gt_ctx->curl = NULL;

    return CCT_OK;
}

cct_status _cct_translator_gt_open(cct_translator_ctx *ctx, void *data)
{
    cct_translator_gt_params *params = (cct_translator_gt_params *) data;
    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) ctx->ctx_data;

    if (!params->key) {
        fprintf(stderr, "_cct_translator_gt_open: no api key provided\n");
        return CCT_FATAL;
    }
    gt_ctx->key = strdup(params->key);

    gt_ctx->curl = curl_easy_init();
    if (!gt_ctx->curl) {
        fprintf(stderr, "_cct_translator_gt_open: curl_easy_init() failed\n");
        return CCT_FATAL;
    }
    CURLcode result = curl_easy_setopt(gt_ctx->curl, CURLOPT_WRITEFUNCTION, on_curl_data_recieved);
    if (result != CURLE_OK) {
        fprintf(stderr, "_cct_translator_gt_open: can't set curl writefunction\n");
        return CCT_FATAL;
    }

    char *langs_available = __cct_translator_gt_get_available_langs(ctx);
    if (!langs_available) {
        fprintf(stderr, "_cct_translator_gt_open: can't get available languages\n");
        return CCT_FATAL;
    }

    gt_ctx->langs_available = __cct_translator_gt_json_parse_langs(langs_available, &gt_ctx->langs_available_count);
    if (!gt_ctx->langs_available) {
        fprintf(stderr, "_cct_translator_gt_open: can't parse available languages\n");
        return CCT_FATAL;
    }

    if (!params->langs || strlen(params->langs) == 0) {
        gt_ctx->langs = (char **) malloc(sizeof(char*));
        if (!gt_ctx->langs) {
            perror("_cct_translator_gt_open: malloc() gt_ctx->langs failed\n");
            return CCT_FATAL;
        }
        gt_ctx->langs[0] = strdup(GOOGLE_TRANSLATE_DEFAULT_LANG);
        if (!gt_ctx->langs[0]) {
            perror("_cct_translator_gt_open: strdup() default lang failed\n");
            return CCT_FATAL;
        }
        gt_ctx->langs_count = 1;
    } else {
        gt_ctx->langs_count = 0;
        char *comma_ptr = strchr(params->langs, ',');
        if (!comma_ptr) {
            if (__cct_translator_gt_add_target_lang(ctx, params->langs)) {
                gt_ctx->langs_count = 1;
            } else {
                fprintf(stderr, "_cct_translator_gt_open: no output languages specified\n");
            }
        } else {
            char *tok = NULL;
            while ((tok = strtok(tok ? NULL : params->langs, ",")) != NULL) {
                __cct_translator_gt_add_target_lang(ctx, tok);
            }
        }
        if (!gt_ctx->langs_count) {
            fprintf(stderr, "_cct_translator_gt_open: no target language specified. check --list-langs option\n");
            return CCT_FATAL;
        }
    }

    return CCT_OK;
}

cct_status _cct_translator_gt_is_api_available(cct_translator_ctx *ctx, unsigned int *status)
{
    (void *) ctx;
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "_cct_translator_gt_is_api_available: curl_easy_init() failed\n");
        return CCT_FATAL;
    }

    CURLcode result = curl_easy_setopt(curl, CURLOPT_URL, GOOGLE_TRANSLATE_API_URL); //TODO same-same
    if (result != CURLE_OK) {
        fprintf(stderr, "_cct_translator_gt_is_api_available: can't set curl url\n");
        return CCT_FATAL;
    }

    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        fprintf(stderr, "_cct_translator_gt_is_api_available: curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        return CCT_FATAL;
    }

    curl_easy_cleanup(curl);

    *status = 1;

    return CCT_OK;
}

cct_status _cct_translator_gt_list_available_langs(cct_translator_ctx *ctx, cct_language **langs, unsigned int *count)
{
    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) ctx->ctx_data;
    *count = gt_ctx->langs_available_count;
    *langs = gt_ctx->langs_available;
    return CCT_OK;
}

cct_status _cct_translator_gt_list_target_langs(cct_translator_ctx *ctx, cct_language **langs, unsigned int *count)
{
    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) ctx->ctx_data;
    *count = gt_ctx->langs_count;
    if (!gt_ctx->langs_count) {
        *langs = NULL;
        return CCT_OK;
    }
    *langs = (cct_language *) malloc(gt_ctx->langs_count * sizeof(cct_language));
    if (!langs) {
        perror("_cct_translator_gt_list_target_langs: malloc() failed");
        return CCT_FATAL;
    }
    for (int i = 0; i < gt_ctx->langs_count; i++) {
        (*langs)[i].code = strdup(gt_ctx->langs[i]);
        (*langs)[i].name = NULL;
    }
    return CCT_OK;
}

cct_status _cct_translator_gt_close(cct_translator_ctx *ctx)
{
    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) ctx->ctx_data;

    curl_easy_cleanup(gt_ctx->curl);

    free(gt_ctx->key);
    for (int i = 0; i < gt_ctx->langs_count; i++) {
        free(gt_ctx->langs[i]);
    }
    free(gt_ctx->langs);

    for (int i = 0; i < gt_ctx->langs_available_count; i++) {
        free(gt_ctx->langs_available[i].code);
        free(gt_ctx->langs_available[i].name);
    }
    free(gt_ctx->langs_available);

    free(gt_ctx);
}

cct_status _cct_translator_gt_translate(cct_translator_ctx *ctx,
                                        char *source_lang, //if value is NULL, autodetection will be used
                                        char *target_lang,
                                        char *text,
                                        char **result)
{
    (void *) source_lang;
    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) ctx->ctx_data;
    char *response_text = __cct_translator_gt_request(ctx, target_lang, text);
    if (!response_text) {
        fprintf(stderr, "cct_translator_gt_translate: translation failed\n");
        return CCT_FATAL;
    }
    *result = __cct_translator_gt_get_translated_text(response_text);
    if (!*result) {
        fprintf(stderr, "cct_translator_gt_translate: json parsing failed\n");
        return CCT_FATAL;
    }
    free(response_text);
    return CCT_OK;
}

char *__cct_translator_gt_get_translated_text(char *json_text)
{
    cJSON *response = cJSON_Parse(json_text);
    if (!response) {
        fprintf(stderr, "__cct_translator_gt_get_translated_text: can't parse response\n");
        cJSON_Delete(response);
        return NULL;
    }
    cJSON *data = cJSON_GetObjectItem(response, "data");
    if (!data) {
        fprintf(stderr, "__cct_translator_gt_get_translated_text: \"data\" not found\n");
        cJSON_Delete(response);
        return NULL;
    }
    cJSON *translations = cJSON_GetObjectItem(data, "translations");
    if (!translations) {
        fprintf(stderr, "__cct_translator_gt_get_translated_text: \"translations\" not found\n");
        cJSON_Delete(response);
        return NULL;
    }
    cJSON *translation = cJSON_GetArrayItem(translations, 0);
    if (!translation) {
        fprintf(stderr, "__cct_translator_gt_get_translated_text: \"translations\" has no elements\n");
        cJSON_Delete(response);
        return NULL;
    }
    cJSON *translated_text = cJSON_GetObjectItem(translation, "translatedText");
    if (!translated_text) {
        fprintf(stderr, "__cct_translator_gt_get_translated_text: \"translatedText\" not found\n");
        cJSON_Delete(response);
        return NULL;
    }
    char *quoted_text = cJSON_Print(translated_text);
    char *text = cjson_remove_quotes(quoted_text);
    free(quoted_text);

    cJSON_Delete(response);

    char *unescaped_text = json_unescape(text);
    free(text);

    return unescaped_text;
}

char *__cct_translator_gt_get_available_langs(cct_translator_ctx *ctx)
{
    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) ctx->ctx_data;

    size_t key_len = strlen(gt_ctx->key);
    size_t baseurl_len = strlen(GOOGLE_TRANSLATE_API_LANGS);

    char *url = (char *) malloc(key_len + baseurl_len + 1);
    if (!url) {
        perror("_cct_translator_gt_get_available_langs: malloc() url failed");
        return NULL;
    }

    sprintf(url, GOOGLE_TRANSLATE_API_LANGS, gt_ctx->key);

    curl_response response;
    curl_response_init(&response);

    curl_easy_setopt(gt_ctx->curl, CURLOPT_URL, url);
    curl_easy_setopt(gt_ctx->curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(gt_ctx->curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "_cct_translator_gt_get_available_langs: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return NULL;
    }
    free(url);

    long response_code;
    res = curl_easy_getinfo(gt_ctx->curl, CURLINFO_RESPONSE_CODE, &response_code);

    if (res != CURLE_OK) {
        fprintf(stderr, "_cct_translator_gt_get_available_langs: curl_easy_getinfo() failed: %s\n", curl_easy_strerror(res));
        return NULL;
    }

    char *result = curl_response_content(&response);
    if (response_code != 200) {
        fprintf(stderr, "_cct_translator_gt_get_available_langs: HTTP Status code: %ld\n", response_code);
        fprintf(stderr, result);
        free(result);
        return NULL;
    }

    curl_response_cleanup(&response);

    return result;
}

char *__cct_translator_gt_request (cct_translator_ctx *ctx, char *lang, char *text)
{
    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) ctx->ctx_data;

    char *escaped_text = curl_easy_escape(gt_ctx->curl, text, 0);

    size_t text_len = strlen(escaped_text);
    size_t key_len = strlen(gt_ctx->key);
    size_t lang_len = strlen(lang);
    size_t baseurl_len = strlen(GOOGLE_TRANSLATE_API_QUERY);

    char *url = (char *) malloc(text_len + key_len + lang_len + baseurl_len + 1);
    if (!url) {
        perror("__cct_translator_gt_request: malloc() url failed");
        return NULL;
    }

    sprintf(url, GOOGLE_TRANSLATE_API_QUERY, gt_ctx->key, lang, escaped_text);

    curl_response response;
    curl_response_init(&response);

    curl_easy_setopt(gt_ctx->curl, CURLOPT_URL, url);
    curl_easy_setopt(gt_ctx->curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(gt_ctx->curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "__cct_translator_gt_request: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return NULL;
    }
    free(url);

    long response_code;
    res = curl_easy_getinfo(gt_ctx->curl, CURLINFO_RESPONSE_CODE, &response_code);

    if (res != CURLE_OK) {
        fprintf(stderr, "__cct_translator_gt_request: curl_easy_getinfo() failed: %s\n", curl_easy_strerror(res));
        return NULL;
    }

    char *result = curl_response_content(&response);
    if (response_code != 200) {
        fprintf(stderr, "__cct_translator_gt_request: HTTP Status code: %ld\n", response_code);
        fprintf(stderr, result);
        free(result);
        return NULL;
    }

    curl_free(escaped_text);
    curl_response_cleanup(&response);

    return result;
}

int __cct_translator_gt_is_lang_known(cct_translator_ctx *ctx, char *lang)
{
    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) ctx->ctx_data;
    for (int i = 0; i < gt_ctx->langs_available_count; i++) {
        if (!strcmp(lang, gt_ctx->langs_available[i].code)) {
            return 1;
        }
    }
    return 0;
}

int __cct_translator_gt_add_target_lang(cct_translator_ctx *ctx, char *lang)
{
    cct_translator_gt_ctx *gt_ctx = (cct_translator_gt_ctx *) ctx->ctx_data;
    if (!__cct_translator_gt_is_lang_known(ctx, lang)) {
        fprintf(stderr, "__cct_translator_gt_add_target_lang: unknown language \"%s\"\n", lang);
        return 0;
    }
    gt_ctx->langs = realloc(gt_ctx->langs, ++(gt_ctx->langs_count) * sizeof(char*));
    if (!gt_ctx->langs) {
        perror("__cct_translator_gt_add_target_lang: realloc() gt_ctx->langs failed\n");
        return 0;
    }
    gt_ctx->langs[gt_ctx->langs_count - 1] = strdup(lang);
    if (!gt_ctx->langs[gt_ctx->langs_count - 1]) {
        perror("__cct_translator_gt_add_target_lang: strdup() lang failed\n");
        return 0;
    }
    return 1;
}

cct_language *__cct_translator_gt_json_parse_langs(char *json_response, unsigned int *count)
{
    cJSON *response = cJSON_Parse(json_response);
    if (!response) {
        fprintf(stderr, "__cct_translator_gt_json_parse_langs: can't parse response\n");
        cJSON_Delete(response);
        return NULL;
    }
    cJSON *data = cJSON_GetObjectItem(response, "data");
    if (!data) {
        fprintf(stderr, "__cct_translator_gt_json_parse_langs: \"data\" not found\n");
        cJSON_Delete(response);
        return NULL;
    }
    cJSON *languages = cJSON_GetObjectItem(data, "languages");
    if (!languages) {
        fprintf(stderr, "__cct_translator_gt_json_parse_langs: \"languages\" not found\n");
        cJSON_Delete(response);
        return NULL;
    }

    cJSON *lang;
    cct_language *langs = NULL;
    unsigned int langs_count = 0;
    while ((lang = cJSON_GetArrayItem(languages, langs_count)) != NULL) {
        cJSON *lang_code = cJSON_GetObjectItem(lang, "language");
        if (!lang_code) {
            fprintf(stderr, "__cct_translator_gt_json_parse_langs: \"language\" not found\n");
            cJSON_Delete(response);
            return NULL;
        }
        cJSON *lang_name = cJSON_GetObjectItem(lang, "name");
        if (!lang_name) {
            fprintf(stderr, "__cct_translator_gt_json_parse_langs: \"name\" not found\n");
            cJSON_Delete(response);
            return NULL;
        }

        langs = realloc(langs, ++langs_count * sizeof(cct_language));
        char *quoted_code = cJSON_Print(lang_code);
        char *code = cjson_remove_quotes(quoted_code);
        langs[langs_count - 1].code = code;
        langs[langs_count - 1].name = cJSON_Print(lang_name); //we can leave quotes here

        free(quoted_code);
    }

    if (langs_count == 0) {
        fprintf(stderr, "__cct_translator_gt_json_parse_langs: \"languages\" has no elements\n");
        cJSON_Delete(response);
        return NULL;
    }

    cJSON_Delete(response);

    *count = langs_count;
    return langs;
}
