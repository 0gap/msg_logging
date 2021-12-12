//
// Created by zerogap on 12/12/21.
//

#include <glog/logging.h>

#include <thread>
#include <zmq.hpp>

#include "publisher.h"
#include "subscriber.h"
#include "pub_sub_proxy.h"

int main()
{
  zmq::context_t ctx(1);
  PubSubProxy proxy(Binding("tcp://127.0.0.1:9999"),
                    Binding("tcp://127.0.0.1:8888"),
                    Binding("tcp://127.0.0.1:8890"), &ctx);
  std::this_thread::sleep_for(std::chrono::seconds (1));
  Subscriber sub1(1, "tcp://127.0.0.1:8888");
  Subscriber sub2(2, "tcp://127.0.0.1:8888");

  // Just, give everyone a bit of time to settle until you start publishing
  std::this_thread::sleep_for(std::chrono::seconds(2));
  {
    Publisher pub(1, "tcp://*:9999");
    std::this_thread::sleep_for(std::chrono::seconds(6));
    pub.stop();
  }
  sub1.stop();
  sub2.stop();
  std::this_thread::sleep_for(std::chrono::seconds(6));
  LOG(INFO) << "Shutting down";
  proxy.stop();
}
