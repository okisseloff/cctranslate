cmake_minimum_required(VERSION 3.2)
project(cctranslate)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES cctranslate.c)
add_executable(cctranslate ${SOURCE_FILES}
    config.h config.c
    cct_sub_entry.h cct_sub_entry.c
    utils.h utils.c
    sink/cct_sink.h sink/cct_sink_subrip.h sink/cct_sink_subrip.c
    source/cct_source.h source/cct_source_subrip.h source/cct_source_subrip.c
    entry_buffer/cct_entry_buffer.h entry_buffer/cct_entry_buffer.c
    translator/cct_translator.h translator/cct_translator_gt.h translator/cct_translator_gt.c
    3rd_party/cJSON.h 3rd_party/cJSON.c
    source/cct_source_extractor.h source/cct_source_extractor.c
    source/ccx_sub_entry.pb_c.h source/ccx_sub_entry.pb_c.c
    sink/cct_sink_transcript.h sink/cct_sink_transcript.c)
target_link_libraries(cctranslate m curl nanomsg protobuf-c)
