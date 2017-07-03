
#ifndef __FH_CORE_PERSIST_ORIGINAL_RECEIVER_H__
#define __FH_CORE_PERSIST_ORIGINAL_RECEIVER_H__

#include "core/global.h"
#include "core/zmq/zmq_receiver.h"

namespace fh
{
namespace core
{
namespace persist
{
    class OriginalReceiver : public fh::core::zmq::ZmqReceiver
    {
        public:
            OriginalReceiver(const std::string &url, std::function<void(char *, const size_t)> processor);
            virtual ~OriginalReceiver();

        public:
            void Save(char *data, size_t size) override;

        private:
            std::function<void(char *, const size_t)> m_processor;

        private:
            DISALLOW_COPY_AND_ASSIGN(OriginalReceiver);
    };
} // namespace persist
} // namespace core
} // namespace fh

#endif // __FH_CORE_PERSIST_ORIGINAL_RECEIVER_H__
