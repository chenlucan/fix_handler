
#ifndef __FH_CME_MARKET_BOOK_RECEIVER_H__
#define __FH_CME_MARKET_BOOK_RECEIVER_H__

#include "core/global.h"
#include "core/zmq/zmq_receiver.h"

namespace fh
{
namespace cme
{
namespace market
{
    class BookReceiver : public fh::core::zmq::ZmqReceiver
    {
        public:
            explicit BookReceiver(const std::string &url);
            virtual ~BookReceiver();

        public:
            virtual void Save(char *data, size_t size);

        private:
            DISALLOW_COPY_AND_ASSIGN(BookReceiver);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_BOOK_RECEIVER_H__
