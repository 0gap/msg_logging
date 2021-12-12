//
// Created by zerogap on 12/12/21.
//

#include "LogEntrySerializer.h"

LogEntrySerializer::LogEntrySerializer(Logger::LogLevel lvl,
                  std::string code_point,
                  std::string log_msg,
                  std::string srvc_name,
                  std::string host):builder(1024) {

    fb_lvl = lvl;
    fb_code_point = code_point;//builder.CreateString(code_point);
    fb_log_msg = log_msg;      //builder.CreateString(log_msg);
    fb_srvc_name = srvc_name;  //builder.CreateString(srvc_name);
    fb_host = host;            //builder.CreateString(host);
}

void LogEntrySerializer::export_fb(){
    auto code_point = builder.CreateString(fb_code_point);
    auto log_msg = builder.CreateString(fb_log_msg);
    auto srvc_name = builder.CreateString(fb_srvc_name);
    auto host = builder.CreateString(fb_host);

    Logger::LogEntryBuilder builder_(builder);
    builder_.add_lvl(fb_lvl);
    builder_.add_log_msg(log_msg);
    builder_.add_code_point(code_point);
    builder_.add_srvc_name(srvc_name);
    builder_.add_host(host);
    auto pce = builder_.Finish();
    builder.Finish(pce);
    buf = builder.GetBufferPointer();
    size = builder.GetSize();
}

std::string_view LogEntrySerializer::get_data() {
    return std::string_view{reinterpret_cast<char*>(builder.GetBufferPointer()),
                            builder.GetSize()};
}