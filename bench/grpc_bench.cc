//
// Created by zerogap on 12/12/21.
//
#include "log_client.h"
#include "LogEntrySerializer.h"
#include "log_server.h"
#include <benchmark/benchmark.h>
#include <thread>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define __CODE_POINT__ (__FILE__ "::" STR(__LINE__))

static void rungrpc(benchmark::State &state) {
    std::unique_ptr<grpc::Server> server;
    std::thread t(RunServer, std::ref(server));
    std::string server_address("localhost:50051");

    auto channel =
            grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    LogClientImpl greeter(channel);

    for (auto _ : state) {
        LogEntrySerializer new_log(Logger::LogLevel::DEBUG, __CODE_POINT__, "hello", "this servec", "hostname");
        new_log.export_fb();
        LogEntrySerializer new_log2(Logger::LogLevel::INSANE, __CODE_POINT__, "hello from msg2", "this service", "hostname");
        new_log2.export_fb();
        greeter.SendLog(new_log);
        greeter.SendLog(new_log2);
    }
    server->Shutdown();
    t.join();
}


BENCHMARK(rungrpc);

// Run the benchmark
BENCHMARK_MAIN();
