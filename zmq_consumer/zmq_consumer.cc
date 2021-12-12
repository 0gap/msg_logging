//
// Created by zerogap on 12/12/21.
//
#include "zmq_consumer.h"
#include <atomic>
#include <iostream>
#include <thread>

bool operator==(const std::string &expected, const zmq::message_t &message) {
    return expected.size() == message.size() && 0 == std::memcmp(expected.data(), message.data(), expected.size());
}

namespace {
    int messages_accepted{0};
    int messages_dropped{0};
}// namespace

namespace msg_logging::zmq_logger {

    class ZmqConsumer::Impl {
        using Receivers = decltype(ZmqConsumer::receivers_);
        using Address = decltype(ZmqConsumer::listen_address_);

    public:
        Impl(const Address &a, Receivers &r) : address_{a}, receivers_{r},
                                               // startup
                                               running_{true}, thread_{&Impl::Run, this} {}
        ~Impl() {
            running_ = false;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            LOG(INFO) << "Consumed " << messages_accepted << " messages";
            thread_.join();
        }
        void stop() {
            running_ = false;
        }
        void Run() {
            LOG(INFO) << "Starting ZMQ on " << address_;

            zmq::context_t context(1);
            zmq::socket_t socket(context, ZMQ_PULL);
            std::this_thread::sleep_for(std::chrono::seconds(2));

            int timeout_milliseconds = 500;
            socket.set(zmq::sockopt::rcvtimeo, timeout_milliseconds);
            socket.bind(address_.c_str());
            zmq::message_t header(std::string("header"));
            zmq::message_t body;
            while (running_) {

                if (socket.recv(body)) {
                    if (handle_message(header, body)) {
                        ++messages_accepted;
                    } else {
                        ++messages_dropped;
                        LOG(INFO) << "Messages dropped" << messages_dropped;
                    }
                }
            }
            // stop accepting new messages, but forward any we have
            // received to the dispatcher
            socket.unbind(address_.c_str());
            while (socket.recv(header) && socket.recv(body)) {
                if (handle_message(header, body)) {
                    LOG_EVERY_N(INFO, 10) << "Still have messages in queue";
                    ++messages_accepted;
                } else {
                    ++messages_dropped;
                }
            }
        }

    private:
        bool handle_message(zmq::message_t &header, zmq::message_t &body) {
            VLOG(5) << "Receiving header of size " << header.size();
            if (header.size() > 64 || body.size() > 64 * 1024) {
                return false;
            }
            //TODO: better handling implementation. Some form of chaining is required here.
            // Perhaps have an iterator object so that you can loop here.
            // Alternatively, a proxy that implements the MessageHandler objects.
            // The proxy would implement the Chain Of Repsonsibility and MessazeHandlers would pass the massege to the next
            // handler.
            for (auto &processor : receivers_) {
                if (!processor->handle_message(header, body))
                    return false;
            }
            VLOG(5) << "All ok";
            return true;
        }

    private:
        const Address &address_;
        Receivers &receivers_;

    private:
        std::atomic<bool> running_{true};
        std::thread thread_;
    };
    ZmqConsumer::ZmqConsumer(std::string addr) : listen_address_(std::move(addr)) {}
    ZmqConsumer::~ZmqConsumer() = default;

    void ZmqConsumer::Add(MessageHandler &p) {
        receivers_.push_back(&p);
    }


    void ZmqConsumer::stop() {
        CHECK(!receivers_.empty());
        impl_->stop();
    }

    void ZmqConsumer::Start() {
        CHECK(!receivers_.empty());
        impl_ = std::make_unique<Impl>(listen_address_, receivers_);
    }
}// namespace msg_logging::zmq_logger
