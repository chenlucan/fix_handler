
#include "cme/market/book_sender.h"

namespace fh
{
namespace cme
{
namespace market
{

    BookSender::BookSender(const std::string &url) : sender(url)
    {

    }

    BookSender::~BookSender()
    {

    }

    void BookSender::Send(const std::string &message)
    {
        sender.Send(message);
    }

    // implement of MarketListenerI
    void BookSender::OnMarketDisconnect(core::market::MarketI* market)
    {

    }

    // implement of MarketListenerI
    void BookSender::OnMarketReconnect(core::market::MarketI* market)
    {

    }

    // implement of MarketListenerI
    void BookSender::OnContractDefinition()
    {

    }

    // implement of MarketListenerI
    void BookSender::OnBBO()
    {

    }

    // implement of MarketListenerI
    void BookSender::OnBid()
    {

    }

    // implement of MarketListenerI
    void BookSender::OnOffer()
    {

    }

    // implement of MarketListenerI
    void BookSender::OnL2()
    {

    }

    // implement of MarketListenerI
    void BookSender::OnL3()
    {

    }

    // implement of MarketListenerI
    void BookSender::OnTrade()
    {

    }

} // namespace market
} // namespace cme
} // namespace fh
