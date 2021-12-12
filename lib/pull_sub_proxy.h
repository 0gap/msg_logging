//
// Created by zerogap on 12/12/21.
//

#ifndef MSG_TEST_PUSH_SUB_PROXY_H
#define MSG_TEST_PUSH_SUB_PROXY_H

#include "binding.h"

struct PullSubProxy
{
  const Binding frontend_binding_;
  const Binding backend_binding_;
  const Binding control_binding_;

  zmq::context_t *ctx_;

  std::thread work_thread_;

  PullSubProxy(Binding frontend_connection, Binding backend_binding,
               Binding control_bindind, zmq::context_t *ctx)
      : frontend_binding_(frontend_connection),
        backend_binding_(backend_binding),
        control_binding_(control_bindind),
        ctx_(ctx),
        work_thread_(&PullSubProxy::run, this)
  {
  }
  ~PullSubProxy()
  {
    ctx_->shutdown();
    work_thread_.join();
  }
  void stop()
  {
    // Using member context-the same with the other sockets is not an option.
    // Does not work
    zmq::context_t ctx(1);
    zmq::socket_t control_sock(ctx, zmq::socket_type::push);
    control_sock.connect(control_binding_);

    const std::string terminate_com = "TERMINATE";
    if (control_sock.send(zmq::const_buffer(zmq::buffer(terminate_com)),
                          zmq::send_flags::dontwait))
    {
      LOG(INFO) << "Control socket in proxy stop was sent correctly";
    }
    else
    {
      LOG(INFO) << "Control socket in proxy stop was NOT sent";
    }
  }
  void run()
  {
    zmq::socket_t frontend(*ctx_, zmq::socket_type::pull);
    zmq::socket_t backend(*ctx_, zmq::socket_type::pub);
    zmq::socket_t control_sock(*ctx_, zmq::socket_type::pull);
    try
    {
//      LOG(INFO) << "Binding frontend to " << "ipc:///var/log/pull_sub_proxy/0";
//      frontend.bind("ipc:///var/log/pull_sub_proxy/0");
      LOG(INFO) << "Binding frontend to " << frontend_binding_.star_host();
      frontend.bind(frontend_binding_.star_host());
//      frontend.bind(frontend_binding_);
      LOG(INFO) << "Binding backend to " << backend_binding_.star_host();
      backend.bind(backend_binding_.star_host());
      LOG(INFO) << "Binding control to " << control_binding_.star_host();
      control_sock.bind(control_binding_.star_host());
//      zmq::proxy(zmq::socket_ref(frontend), zmq::socket_ref(backend), nullptr);
      zmq::proxy_steerable(zmq::socket_ref(frontend), zmq::socket_ref(backend),
                           nullptr, zmq::socket_ref(control_sock));
    }
    catch (const zmq::error_t &e)
    {
      LOG(INFO) << "error from proxy, num: " << e.num()
                << " msg : " << e.what();
    }
    ctx_->shutdown();
    LOG(INFO) << "proxy terminated";
  }
};
#endif//MSG_TEST_PUSH_SUB_PROXY_H
