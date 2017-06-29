
#ifndef __FH_CORE_ZMQ_ZMQ_SENDER_H__
#define __FH_CORE_ZMQ_ZMQ_SENDER_H__

#include <string>
#include <zmq.hpp>
#include "core/global.h"

namespace fh
{
namespace core
{
namespace zmq
{
    class ZmqSender
    {
        public:
            explicit ZmqSender(const std::string &url);
            virtual ~ZmqSender();

        public:
            void Send(const char *message, size_t length);
            void Send(const std::string &message);

        private:
            ::zmq::context_t m_context;
            ::zmq::socket_t m_sender;
    };
} // namespace zmq
} // namespace core
} // namespace fh

#endif // __FH_CORE_ZMQ_ZMQ_SENDER_H__
