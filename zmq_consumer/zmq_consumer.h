//
// Created by zerogap on 12/12/21.
//

#ifndef MSG_LOGGING_ZMQ_CONSUMER_H
#define MSG_LOGGING_ZMQ_CONSUMER_H
#include "message_processor.h"
#include <memory>
#include <vector>
#include <zmq.hpp>

namespace msg_logging
{
    namespace zmq_logger {

        /** Consumes concomming messages */
        class ZmqConsumer {

        public:
            ZmqConsumer(std::string zmq_address);
            ~ZmqConsumer();

            // must be called before start
            void Add(MessageHandler &);
            void Start();
            void stop();
        private:
            std::string listen_address_;
            std::vector<MessageHandler *> receivers_;

        private:
            class Impl;
            std::unique_ptr<Impl> impl_;
        };
    }// namespace zmq_logger
}// namespace msg_logging
#endif// MSG_LOGGING_ZMQ_CONSUMER_H
