//
// Created by zerogap on 12/12/21.
//

#ifndef MSG_TEST_LOG_SERVER_H
#define MSG_TEST_LOG_SERVER_H

#include "LogEntrySerializer.h"
#include "log_msg.grpc.fb.h"

//#include <grpc++/grpc++.h>
#include <grpcpp/grpcpp.h>
#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <string>

using namespace Logger;

class LogComsImpl final : public Logger::LogComs::Service {
    virtual grpc::Status Send(
            grpc::ServerContext *context,
            const flatbuffers::grpc::Message<Logger::LogEntry> *request_msg,
            flatbuffers::grpc::Message<LogSendResponse> *response_msg) override {

        // We call GetRoot to "parse" the message. Verification is already
        // performed by default. See the notes below for more details.
        const LogEntry *request = request_msg->GetRoot();

        // Fields are retrieved as usual with FlatBuffers
        const std::string &msg_string = request->log_msg()->str();
        DLOG(INFO) << "server: someone sent me a log with msg:\n\t" << msg_string;
        // `flatbuffers::grpc::MessageBuilder` is a `FlatBufferBuilder` with a
        // special allocator for efficient gRPC buffer transfer, but otherwise
        // usage is the same as usual.
        flatbuffers::grpc::MessageBuilder mb_;
        auto msg_offset = mb_.CreateString("All is well");

        auto hello_offset = CreateLogSendResponse(mb_, true, msg_offset);
        mb_.Finish(hello_offset);

        // The `ReleaseMessage<T>()` function detaches the message from the
        // builder, so we can transfer the resopnse to gRPC while simultaneously
        // detaching that memory buffer from the builer.
        *response_msg = mb_.ReleaseMessage<LogSendResponse>();
        assert(response_msg->Verify());

        // Return an OK status.
        return grpc::Status::OK;
    }
    virtual ::grpc::Status Retrieve(::grpc::ServerContext *context, const flatbuffers::grpc::Message<LogId> *request_msg,
                                    flatbuffers::grpc::Message<LogEntry> *response)
            override {
        // The streaming usage below is simply a combination of standard gRPC
        // streaming with the FlatBuffers usage shown above.
//        const LogId *request = request_msg->GetRoot();
//        const int64_t &log_id = request->id();
//        LOG(INFO) << "server: requested log id " << log_id;
//        char s[100];
//        auto code_point = mb_.CreateString(sprintf(s, "code point %d ", log_id));
//        auto log_msg = mb_.CreateString(sprintf(s, "log msg %d ", log_id));
//        auto srvc_name = mb_.CreateString(sprintf(s, "service name %d ", log_id));
//        auto host = mb_.CreateString(sprintf(s, "service host %d ", log_id));
//        auto log_offset = CreateLogEntry(mb_, LogLevel::INFO, code_point, log_msg, srvc_name, host);
//        mb_.Finish(log_offset);
//        *response = mb_.ReleaseMessage<LogEntry>();
//        //
//        //        for (int i = 0; i < num_greetings; i++) {
//        //            auto msg_offset = mb_.CreateString("Many hellos, " + name);
//        //            auto hello_offset = CreateHelloReply(mb_, msg_offset);
//        //            mb_.Finish(hello_offset);
//        //            writer->Write(mb_.ReleaseMessage<HelloReply>());
//        //        }

        return grpc::Status::OK;
    }

    flatbuffers::grpc::MessageBuilder mb_;
};

void RunServer(std::unique_ptr<grpc::Server> &server) {
    std::string server_address("0.0.0.0:50051");
    LogComsImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> new_server(builder.BuildAndStart());
    server.reset(new_server.release());
    std::cerr << "Server listening on " << server_address << std::endl;
    server->Wait();
}

//int main(int argc, const char *argv[]) {
//    RunServer();
//    return 0;
//}

#endif//MSG_TEST_LOG_SERVER_H
