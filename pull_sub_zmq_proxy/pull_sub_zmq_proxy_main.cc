//
// Created by zerogap on 12/12/21.
//

#include <glog/logging.h>

#include <csignal>
#include <map>
#include <thread>
#include <zmq.hpp>

#include "lib/pull_sub_proxy.h"
#include "lib/subscriber.h"

int main(int argc, char** argv)
{
//  FLAGS_log_dir = "/var/log/pull_sub_proxy";
//  FLAGS_logbufsecs = 0;
//  gflags::ParseCommandLineFlags(&argc, &argv, true);
//  google::InstallFailureSignalHandler();
//  google::InitGoogleLogging("pull_sub_proxy");

  zmq::context_t ctx(1);
  PullSubProxy proxy(std::move(Binding("tcp://127.0.0.1:19999")),
                     Binding("tcp://127.0.0.1:38888"),
                     Binding("tcp://127.0.0.1:18890"), &ctx);
  //  PullSubProxy proxy(std::move(Binding("ipc:///var/log/pull_sub_proxy/0")),
  //                     Binding("tcp://127.0.0.1:38888"),
  //                     Binding("tcp://127.0.0.1:18890"), &ctx);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  Subscriber sub1(1, "tcp://127.0.0.1:38888");
  Subscriber sub2(2, "tcp://127.0.0.1:38888");

  // Block until we're signalled
  sigset_t set;
  int sig;

  sigemptyset(&set);
  sigaddset(&set, SIGTERM);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGKILL);
  sigaddset(&set, SIGABRT);
  sigaddset(&set, SIGQUIT);
  if (sigwait(&set, &sig) != 0) { PLOG(FATAL) << "sigwait failed"; }

  LOG(INFO) << "Shutting down proxy";
  sub1.stop();
  sub2.stop();
  std::this_thread::sleep_for(std::chrono::seconds(6));
  LOG(INFO) << "Shutting down";
  proxy.stop();
}
