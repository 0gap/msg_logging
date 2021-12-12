//
// Created by zerogap on 12/12/21.
//

#ifndef MSG_LOGGING_MESSAGE_PROCESSOR_H
#define MSG_LOGGING_MESSAGE_PROCESSOR_H

#include <glog/logging.h>
#include <memory>
#include <vector>

#include <zmq.hpp>

namespace msg_logging::zmq_logger {
    class MessageHandler {
    public:
        virtual ~MessageHandler(){};
        virtual bool handle_message(zmq::message_t &header, zmq::message_t &body) { return false; };
    };
}// namespace msg_logging::zmq_logger
#endif// MSG_LOGGING_MESSAGE_PROCESSOR_H
