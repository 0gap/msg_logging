//
// Created by zerogap on 12/12/21.
//

#ifndef MSG_TEST_SUBSCRIBER_H
#define MSG_TEST_SUBSCRIBER_H

#include <LogEntrySerializer.h>

#include <atomic>
#include <thread>
#include <utility>
#include <zmq.hpp>

struct Subscriber
{
  std::atomic_bool terminate;
  int id_;
  std::thread consuming_thread_;
  zmq::socket_ref subscriber_socket_;
  zmq::context_t *ctx_;
  std::string connecting_server_;

  Subscriber(int id, std::string host)
      : terminate{ false },
        id_{ id },
        consuming_thread_(&Subscriber::run, this),
        connecting_server_{ std::move(host) }
  {
  }
  ~Subscriber()
  {
    stop();
    LOG(INFO) << "Subscriber " << id_ << " deleted";
    terminate.store(true);
    consuming_thread_.join();
    ctx_->close();
  }
  void stop()
  {
    if (!terminate)
    {
      terminate.store(true);
      subscriber_socket_.disconnect(connecting_server_);
      ctx_->shutdown();
    }
  }
  void run()
  {
    zmq::context_t ctx(1);
    try
    {
      zmq::socket_t subscriber_socket(ctx, zmq::socket_type::sub);
      ctx_ = &ctx;
      subscriber_socket_ = zmq::socket_ref(subscriber_socket);
      std::this_thread::sleep_for(std::chrono::seconds(2));

      subscriber_socket.connect(connecting_server_);
      subscriber_socket.set(zmq::sockopt::subscribe, "");
      LOG(INFO) << "Subscriber " << id_ << " running";
      zmq::message_t body;
      while (!terminate)
      {
        auto s = subscriber_socket.recv(body);
        if (s)
        {
          auto log_entry = Logger::GetLogEntry(body.data());
          LOG(INFO) << "Subscriber " << id_ << " received a log entry \n\t"
                    << log_entry->log_msg()->str() << " from service "
                    << log_entry->srvc_name()->str();
        }
      }
    }
    catch (zmq::error_t &e)
    {
      LOG(INFO) << "error from subscriber " << id_ << ", num: " << e.num()
                << " msg : " << e.what();
    }
    LOG(INFO) << "subscriber " << id_ << " terminated work loop";
  }
};

#endif//MSG_TEST_SUBSCRIBER_H
