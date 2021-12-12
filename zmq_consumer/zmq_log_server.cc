//
// Created by zerogap on 12/12/21.
//

#include "glog/logging.h"
#include "log_msg_generated.h"
#include "zmq_consumer.h"
#include <csignal>
#include <thread>

class LogProcessor final : public msg_logging::zmq_logger::MessageHandler {
public:
    bool handle_message(zmq::message_t &cache_entry_key, zmq::message_t &body) override {
        auto log_entry = Logger::GetLogEntry(body.data());

        LOG(INFO) << "Received a log entry \n" << body.str() << "\n" << log_entry->log_msg()->str() << " from service " << log_entry->srvc_name()->str();
        return true;
    }

};

int main() {
    google::InstallFailureSignalHandler();
    msg_logging::zmq_logger::ZmqConsumer server("tcp://0.0.0.0:52236");
    LogProcessor proc;
    server.Add(proc);
    server.Start();

    // Block until we're signalled
    sigset_t set;
    int sig;

    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGINT);
    if (sigwait(&set, &sig) != 0) {
        PLOG(FATAL) << "sigwait failed";
    }

    LOG(INFO) << "Shutting down";
    server.stop();
    exit(EXIT_SUCCESS);
}