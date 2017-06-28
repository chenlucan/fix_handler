
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
            enum class Mode
            {
                PUSH,                   // 采用 PUSH 模式发送数据，对应接受端是 PULL 模式
                PUBLISH         // 采用 PUBLISH 模式发送数据，对应接受端是 SUBSCRIBE 模式
            };

        public:
            explicit ZmqSender(const std::string &url, Mode mode = Mode::PUBLISH);
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
