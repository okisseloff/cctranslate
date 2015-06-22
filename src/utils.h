//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_UTILS_H
#define CCTRANSLATE_UTILS_H

#define CCTRANSLATE_VERSION_MAJOR 0
#define CCTRANSLATE_VERSION_MINOR 0
#define CCTRANSLATE_VERSION_MAINT 1

typedef enum cct_status {
    CCT_OK,
    CCT_FAILURE, //something went wrong, but could be fixed by caller. callee has to cleanup
    CCT_FATAL //couldn't be fixed by caller, app should be terminated
} cct_status;

typedef struct cct_language {
    char *code;
    char *name;
} cct_language;

typedef struct curl_response {
    char *buf;
    size_t len;
} curl_response;

cct_status curl_response_init(curl_response *);
cct_status curl_response_cleanup(curl_response *);
char *curl_response_content(curl_response *);
size_t on_curl_data_recieved(char *ptr, size_t size, size_t nmemb, curl_response *response);
char *cjson_remove_quotes(char *);

int has_extension(char *filename, char *ext);
char *compose_filename(char *filename, char *lang);

#endif //CCTRANSLATE_UTILS_H
