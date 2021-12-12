//
// Created by zerogap on 12/12/21.
//

#ifndef MSG_LOGGING_LOGENTRYSERIALIZER_H
#define MSG_LOGGING_LOGENTRYSERIALIZER_H

#include "log_msg_generated.h"
#include "flatbuffers/flatbuffers.h"

struct LogEntrySerializer {
    Logger::LogLevel fb_lvl;
    std::string fb_code_point;
    std::string fb_log_msg;
    std::string fb_srvc_name;
    std::string fb_host;
    uint8_t *buf {nullptr};
    size_t size{ 0}; // Returns the size of the buffer that
    flatbuffers::FlatBufferBuilder builder;

    explicit LogEntrySerializer(Logger::LogLevel lvl,
                      std::string code_point,
                      std::string log_msg,
                      std::string srvc_name,
                      std::string host);

    void export_fb();

    std::string_view get_data();
};


#endif//MSG_LOGGING_LOGENTRYSERIALIZER_H
