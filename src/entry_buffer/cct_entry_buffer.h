//
// Created by Oleg Kisselef (olegkisselef at gmail dot com) on 6/17/15
//

#ifndef CCTRANSLATE_CCT_ENTRY_BUFFER_H
#define CCTRANSLATE_CCT_ENTRY_BUFFER_H

#include "../utils.h"
#include "../cct_sub_entry.h"
#include "../translator/cct_translator.h"

//TODO entry-buffer should implement an event-driven model
struct cct_entry_buffer_ctx;

/**
 * WARNING:
 * Since entry-buffer should raise events when translation is done, this implementation is temporary
 * It will have only one cct_sub_entry* argument soon
 */
typedef cct_status (*CCT_FP_ENTRY_BUFFER_PUSH)(struct cct_entry_buffer_ctx *,
                                               cct_sub_entry *in_entry,
                                               cct_sub_entry **out_entries,
                                               unsigned int *count);

typedef cct_status (*CCT_FP_ENTRY_BUFFER_FLUSH)(struct cct_entry_buffer_ctx *);

typedef struct cct_entry_buffer_ctx {
    CCT_FP_ENTRY_BUFFER_FLUSH flush;
    CCT_FP_ENTRY_BUFFER_PUSH push;
    cct_sub_entry *entry; //TODO queue should be implemented here instead of a single item
    cct_translator_ctx *translator;
} cct_entry_buffer_ctx;

cct_status _cct_entry_buffer_push(cct_entry_buffer_ctx *,
                                  cct_sub_entry *,
                                  cct_sub_entry **,
                                  unsigned int *);
cct_status _cct_entry_buffer_flush(cct_entry_buffer_ctx *);

cct_status cct_init_entry_buffer(cct_entry_buffer_ctx *, cct_translator_ctx *);

#endif //CCTRANSLATE_CCT_ENTRY_BUFFER_H
