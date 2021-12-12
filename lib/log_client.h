//
// Created by zerogap on 12/12/21.
//
#ifndef MSG_TEST_LOG_CLIENT_H
#define MSG_TEST_LOG_CLIENT_H


#include "LogEntrySerializer.h"
#include "log_msg.grpc.fb.h"
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>


class LogClientImpl {
public:
    LogClientImpl(std::shared_ptr<grpc::Channel> channel) : stub_{Logger::LogComs::NewStub(channel)} {
    }
    void send_log(const Logger::LogLevel lvl, std::string_view code_point, std::string_view log_msg, std::string_view srvc_name, std::string_view host) {
        auto log_offset = Logger::CreateLogEntryDirect(mb_, lvl, code_point.data(), log_msg.data(), srvc_name.data(), host.data());
        mb_.Finish(log_offset);
        auto request = mb_.ReleaseMessage<Logger::LogEntry>();
        flatbuffers::grpc::Message<Logger::LogSendResponse> response_msg;
        grpc::ClientContext context;
        grpc::Status stat = stub_->Send(&context, request, &response_msg);
        if (stat.ok()) {
            DLOG(INFO) << "The message was sent";
        } else {
            DLOG(INFO) << "Got an error when sending " << stat.error_details();
            DLOG(INFO) << "Got an error when sending " << stat.error_code();
            DLOG(INFO) << "Got an error when sending " << stat.error_message();
        }
    }
    void SendLog(const LogEntrySerializer &ser) {
        auto code_point = mb_.CreateString(ser.fb_code_point);
        auto log_msg = mb_.CreateString(ser.fb_log_msg);
        auto srvc_name = mb_.CreateString(ser.fb_srvc_name);
        auto host = mb_.CreateString(ser.fb_host);
        auto log_offset = CreateLogEntry(mb_, ser.fb_lvl, code_point, log_msg, srvc_name, host);

        mb_.Finish(log_offset);
        auto request = mb_.ReleaseMessage<Logger::LogEntry>();
        flatbuffers::grpc::Message<Logger::LogSendResponse> response_msg;

        grpc::ClientContext context;
        grpc::Status stat = stub_->Send(&context, request, &response_msg);
        if (stat.ok()) {
            LOG(INFO) << "The message was sent";
        } else {
            LOG(INFO) << "Got an error when sending " << stat.error_details();
            LOG(INFO) << "Got an error when sending " << stat.error_code();
            LOG(INFO) << "Got an error when sending " << stat.error_message();
        }
    }
    void RetrieveLog(uint64_t log_id) {
    }

private:
    flatbuffers::grpc::MessageBuilder mb_;
    std::unique_ptr<Logger::LogComs::Stub> stub_;
};

#endif//MSG_TEST_LOG_CLIENT_H
