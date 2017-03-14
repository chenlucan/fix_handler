
#ifndef __FH_CORE_ZMQ_ZMQ_RECEIVER_H__
#define __FH_CORE_ZMQ_ZMQ_RECEIVER_H__

#include <zmq.hpp>
#include <string>
#include "core/global.h"

namespace fh
{
namespace core
{
namespace zmq
{
    class ZmqReceiver
    {
        public:
            explicit ZmqReceiver(const std::string &url);
            virtual ~ZmqReceiver();

        public:
            void Start_receive();
            virtual void Save(char *data, size_t size) = 0;

        private:
            void Process_message(::zmq::message_t &message);

        private:
            ::zmq::context_t m_context;
            ::zmq::socket_t m_receiver;
    };
} // namespace zmq
} // namespace core
} // namespace fh

#endif // __FH_CORE_ZMQ_ZMQ_RECEIVER_H__
