
#include "core/assist/logger.h"
#include "core/persist/original_receiver.h"

namespace fh
{
namespace core
{
namespace persist
{

    OriginalReceiver::OriginalReceiver(
            const std::string &url, std::function<void(char *, const size_t)> processor)
    : fh::core::zmq::ZmqReceiver(url, fh::core::zmq::ZmqReceiver::Mode::SUBSCRIBE), m_processor(processor)
    {
        // noop
    }

    OriginalReceiver::~OriginalReceiver()
    {
        // noop
    }

    void OriginalReceiver::Save(char *data, size_t size)
    {
        m_processor(data, size);
    }

} // namespace persist
} // namespace core
} // namespace fh

