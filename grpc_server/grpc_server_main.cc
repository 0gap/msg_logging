//
// Created by zerogap on 12/12/21.
//

#include "glog/logging.h"
#include <csignal>
#include "log_server.h"
#include <thread>

int main() {
    google::InstallFailureSignalHandler();
    std::unique_ptr<grpc::Server> grpc_server;
    std::thread t(RunServer, std::ref(grpc_server));
    std::string server_address("0.0.0.0:50051");

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
    grpc_server->Shutdown();
    t.join();
    exit(EXIT_SUCCESS);
}