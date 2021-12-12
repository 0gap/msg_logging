//
// Created by zerogap on 12/12/21.
//
#include <benchmark/benchmark.h>

#include <thread>
#include <zmq_addon.hpp>

#include "LogEntrySerializer.h"
#include "log_client.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define __CODE_POINT__ (__FILE__ "::" STR(__LINE__))

static void log_creation(benchmark::State &state)
{
  for (auto _ : state)
  {
    LogEntrySerializer new_log(Logger::LogLevel::DEBUG, __CODE_POINT__, "hello",
                               "this servec", "hostname");
    new_log.export_fb();
    LogEntrySerializer new_log2(Logger::LogLevel::INSANE, __CODE_POINT__,
                                "hello from msg2", "this service", "hostname");
    new_log2.export_fb();
  }
}
BENCHMARK(log_creation)->Iterations(20000);

zmq::context_t ctx(1);
zmq::socket_t sock1(ctx, zmq::socket_type::push);
zmq::socket_t sock2(ctx, zmq::socket_type::push);
zmq::socket_t sock3(ctx, zmq::socket_type::push);
uint64_t msg_count{ 0 };
static void bench_simple_zmq_pull_server(benchmark::State &state)
{
  for (auto _ : state)
  {
    ++msg_count;
    LogEntrySerializer new_log(Logger::LogLevel::DEBUG, __CODE_POINT__,
                               "hello from msg " + std::to_string(msg_count),
                               "this servec", "hostname");
    new_log.export_fb();

    ++msg_count;
    LogEntrySerializer new_log2(Logger::LogLevel::INSANE, __CODE_POINT__,
                                "hello from msg " + std::to_string(msg_count),
                                "this "
                                "service",
                                "hostname");
    new_log2.export_fb();

    ++msg_count;
    LogEntrySerializer new_log3(Logger::LogLevel::INSANE, __CODE_POINT__,
                                "hello from msg " + std::to_string(msg_count),
                                "this "
                                "service",
                                "hostname");
    new_log3.export_fb();

    sock1.send(zmq::const_buffer(zmq::buffer(new_log.get_data())),
               zmq::send_flags::dontwait);
    sock2.send(zmq::const_buffer(zmq::buffer(new_log2.get_data())),
               zmq::send_flags::dontwait);
    sock3.send(zmq::const_buffer(zmq::buffer(new_log3.get_data())),
               zmq::send_flags::dontwait);
  }
}

BENCHMARK(bench_simple_zmq_pull_server)->Iterations(2000000);

// You need to have nginx load balance compose
// Messages will be load balanced between all grpc servers
uint64_t grpc_lb_sent{0};
static void rungrpc(benchmark::State &state)
{
  std::string server_address("localhost:50052");

  auto channel = grpc::CreateChannel(server_address,
                                     grpc::InsecureChannelCredentials());
  LogClientImpl greeter(channel);

  for (auto _ : state)
  {
    ++grpc_lb_sent;
    greeter.send_log(Logger::LogLevel::INSANE, __CODE_POINT__,
                     "hello from msg "+ std::to_string(grpc_lb_sent), "this service",
                     "hostname");
    ++grpc_lb_sent;
    greeter.send_log(Logger::LogLevel::INSANE, __CODE_POINT__,
                     "hello from msg "+ std::to_string(grpc_lb_sent), "this service", "hostname");
    ++grpc_lb_sent;
    greeter.send_log(Logger::LogLevel::INSANE, __CODE_POINT__,
                     "hello from msg "+ std::to_string(grpc_lb_sent), "this service", "hostname");
  }
}
BENCHMARK(rungrpc)->Iterations(3000);

// Run the benchmark

void setup_socks(){

  // Need to have the ha load balancer running first
  sock1.connect(
          "tcp://127.0.0.1:12404");
  sock1.set(zmq::sockopt::immediate, 1);
  sock1.set(zmq::sockopt::sndhwm, 1024);
  sock1.set(zmq::sockopt::reconnect_ivl_max, 2500);
  sock1.set(zmq::sockopt::linger, 500);

  sock2.connect(
          "tcp://127.0.0.1:12404");
  sock2.set(zmq::sockopt::immediate, 1);
  sock2.set(zmq::sockopt::sndhwm, 1024);
  sock2.set(zmq::sockopt::reconnect_ivl_max, 2500);
  sock2.set(zmq::sockopt::linger, 500);

  sock3.connect(
          "tcp://127.0.0.1:12404");
  sock3.set(zmq::sockopt::immediate, 1);
  sock3.set(zmq::sockopt::sndhwm, 1024);
  sock3.set(zmq::sockopt::reconnect_ivl_max, 2500);
  sock3.set(zmq::sockopt::linger, 500);
}

int main(int argc, char **argv)
{
  FLAGS_log_dir = ".";
  FLAGS_logbufsecs = 0;
  google::InitGoogleLogging("benchmark.log");
  setup_socks();
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
  ::benchmark::RunSpecifiedBenchmarks();
  std::cout << "msgs sent from zmq: " << msg_count << std::endl;
  std::cout << "msgs sent to load balancer: " << grpc_lb_sent << std::endl;
  ctx.shutdown();
  std::cout << "zmq ctx shutdown\n";
  sock1.close();
  std::cout << "Socket disconnected\n";
  google::ShutdownGoogleLogging();
  std::cout << "Google logging shut down\n";
  ::benchmark::Shutdown();
  std::cout << "benchmark shutdown\n";
  return 0;
}
