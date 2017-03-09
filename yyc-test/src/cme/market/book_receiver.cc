
#include "cme/market/book_receiver.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"

namespace fh
{
namespace cme
{
namespace market
{

    BookReceiver::BookReceiver(const std::string &url) : fh::core::zmq::ZmqReceiver(url)
    {
        // noop
    }

    BookReceiver::~BookReceiver()
    {
        // noop
    }

    // TODO save it
    void BookReceiver::Save(char *data, size_t size)
    {
        LOG_TRACE("books: ", std::string(data, size));
    }

} // namespace market
} // namespace cme
} // namespace fh
