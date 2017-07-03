
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
            enum class Mode
            {
                PULL,                   // 采用 PULL 模式接受数据，对应发送端是 PUSH 模式
                SUBSCRIBE         // 采用 SUB 模式接受数据，对应发送端是 PUB 模式
            };

        public:
            explicit ZmqReceiver(const std::string &url, Mode mode = Mode::PULL);
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
