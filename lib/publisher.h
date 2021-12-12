//
// Created by zerogap on 12/12/21.
//

#ifndef MSG_TEST_PUBLISHER_H
#define MSG_TEST_PUBLISHER_H

#include <LogEntrySerializer.h>

#include <atomic>
#include <thread>
#include <zmq.hpp>
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define __CODE_POINT__ (__FILE__ "::" STR(__LINE__))

struct Publisher
{
  std::atomic_bool terminate;
  int id_;
  std::thread worker_thread_;
  zmq::context_t *ctx_;
  std::string binding_server_;

  Publisher(int id, std::string host)
      : terminate{false},
        id_{id},
        worker_thread_(&Publisher::run, this),
        binding_server_{host}
  {
  }

  ~Publisher()
  {
    stop();
    terminate.store(true);
    worker_thread_.join();
    ctx_->close();
    LOG(INFO) << "Publisher " << id_ << " terminating";
  }
  void stop()
  {
    if (!terminate)
    {
      terminate.store(true);
      ctx_->shutdown();
    }
  }
  void run()
  {
    LOG(INFO) << "Publisher starting";
    zmq::context_t ctx(1);
    ctx_ = &ctx;
    zmq::socket_t publisher_socket(ctx, zmq::socket_type::pub);
    // Remember, you cannot have multiple binds to a port
    std::this_thread::sleep_for(std::chrono::seconds(3));
    publisher_socket.bind(binding_server_);//"tcp://*:9999");
    LOG(INFO) << "Publisher " << id_ << " running";
    int c{0};
    try
    {
      while (!terminate)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LogEntrySerializer log_to_send(Logger::LogLevel::INSANE, __CODE_POINT__,
                                       "hello from msg " + std::to_string(c),
                                       "publisher " + std::to_string(id_),
                                       "hostname" + std::to_string(id_));
        log_to_send.export_fb();
        publisher_socket.send(zmq::const_buffer(zmq::buffer(log_to_send.get_data())),
                              zmq::send_flags::dontwait);
        ++c;
      }
    }
    catch (zmq::error_t &e)
    {
      LOG(INFO) << "error from publisher " << id_ << ", num: " << e.num()
                << " msg : " << e.what();
    }
    LOG(INFO) << "publisher " << id_ << " terminated work loop";
  }
};

#endif//MSG_TEST_PUBLISHER_H
