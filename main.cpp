#include "LogEntrySerializer.h"
#include <glog/logging.h>
#include <iostream>
#include <zmq_addon.hpp>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define __CODE_POINT__ (__FILE__ "::" STR(__LINE__))

int main() {

    for (int j = 0; j < 5; ++j) {

        zmq::context_t context(1);
        zmq::socket_t brokerA(context, zmq::socket_type::push);
        brokerA.connect("tcp://localhost:52236");
        LOG(INFO) << "Connected to socket";
        for (int i = 0; i < 5; ++i) {

            LogEntrySerializer log_to_send(Logger::LogLevel::INSANE, __CODE_POINT__, "hello from msg " + std::to_string(i), "this service", "hostname");
            log_to_send.export_fb();
            brokerA.send(zmq::const_buffer(zmq::buffer(log_to_send.get_data())), zmq::send_flags::dontwait);
        }
        brokerA.close();

    }
    return 0;
}