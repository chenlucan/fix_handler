
#include "core/assist/logger.h"
#include "cme/market/book_sender.h"

namespace fh
{
namespace cme
{
namespace market
{

    BookSender::BookSender(const std::string &url) : m_sender(url)
    {
        // noop
    }

    BookSender::~BookSender()
    {
        // noop
    }

    // implement of MarketListenerI
    void BookSender::OnMarketDisconnect(core::market::MarketI* market)
    {
        // noop
    }

    // implement of MarketListenerI
    void BookSender::OnMarketReconnect(core::market::MarketI* market)
    {
        // noop
    }

    // implement of MarketListenerI
    void BookSender::OnContractDefinition(const pb::dms::Contract &contract)
    {
        // 前面加个 C 标记是 definition 数据
        LOG_INFO("send Definition: ", fh::core::assist::utility::Format_pb_message(contract));
        m_sender.Send("C" + contract.SerializeAsString());
    }

    // implement of MarketListenerI
    void BookSender::OnBBO(const pb::dms::BBO &bbo)
    {
        // 前面加个 B 标记是 BBO 数据
        LOG_INFO("send BBO: ", fh::core::assist::utility::Format_pb_message(bbo));
        m_sender.Send("B" + bbo.SerializeAsString());
    }

    // implement of MarketListenerI
    void BookSender::OnBid(const pb::dms::Bid &bid)
    {
        // 前面加个 D 标记是 bid 数据
        LOG_INFO("send Bid: ", fh::core::assist::utility::Format_pb_message(bid));
        m_sender.Send("D" + bid.SerializeAsString());
    }

    // implement of MarketListenerI
    void BookSender::OnOffer(const pb::dms::Offer &offer)
    {
        // 前面加个 O 标记是 offer 数据
        LOG_INFO("send Offer: ", fh::core::assist::utility::Format_pb_message(offer));
        m_sender.Send("O" + offer.SerializeAsString());
    }

    // implement of MarketListenerI
    void BookSender::OnL2(const pb::dms::L2 &l2)
    {
        // 前面加个 L 标记是 L2 数据
        LOG_INFO("send L2: ", fh::core::assist::utility::Format_pb_message(l2));
        m_sender.Send("L" + l2.SerializeAsString());
    }

    // implement of MarketListenerI
    void BookSender::OnL3()
    {
        // noop
    }

    // implement of MarketListenerI
    void BookSender::OnTrade(const pb::dms::Trade &trade)
    {
        // 前面加个 T 标记是 trade 数据
        LOG_INFO("send Trade: ", fh::core::assist::utility::Format_pb_message(trade));
        m_sender.Send("T" + trade.SerializeAsString());
    }
    void BookSender::OnContractAuctioning(std::string contract)
    {
      LOG_INFO("contract status update [Auction]");
    }
    void BookSender::OnContractNoTrading(std::string contract)
    {
      LOG_INFO("contract status update [NoTrading]");
    }
    void BookSender::OnContractTrading(std::string contract)
    {
      LOG_INFO("contract status update [Trading]");
    }
} // namespace market
} // namespace cme
} // namespace fh
