#include "core/assist/logger.h"
#include "femas/market/femas_book_sender.h"

namespace fh
{
namespace femas
{
namespace market
{
    CFemasBookSender::CFemasBookSender(const std::string &org_url, const std::string &book_url)
    : m_org_sender(org_url), m_book_sender(book_url)
    {
        // noop
    }

    CFemasBookSender::~CFemasBookSender()
    {
        // noop
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnMarketDisconnect(core::market::MarketI* market)
    {
        // noop
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnMarketReconnect(core::market::MarketI* market)
    {
        // noop
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnContractDefinition(const pb::dms::Contract &contract)
    {
        //
        LOG_INFO("send Definition: ", fh::core::assist::utility::Format_pb_message(contract));
        m_book_sender.Send("C" + contract.SerializeAsString());
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnBBO(const pb::dms::BBO &bbo)
    {//测试发送数据
       LOG_INFO("send BBO: ", fh::core::assist::utility::Format_pb_message(bbo));
	return;
        //
        LOG_INFO("send BBO: ", fh::core::assist::utility::Format_pb_message(bbo));
        m_book_sender.Send("B" + bbo.SerializeAsString());
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnBid(const pb::dms::Bid &bid)
    {//测试发送数据
        LOG_INFO("send Bid: ", fh::core::assist::utility::Format_pb_message(bid));
	return;
        //
        LOG_INFO("send Bid: ", fh::core::assist::utility::Format_pb_message(bid));
        m_book_sender.Send("D" + bid.SerializeAsString());
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnOffer(const pb::dms::Offer &offer)
    {//测试发送数据
        LOG_INFO("send Offer: ", fh::core::assist::utility::Format_pb_message(offer));
	return;
        //
        LOG_INFO("send Offer: ", fh::core::assist::utility::Format_pb_message(offer));
        m_book_sender.Send("O" + offer.SerializeAsString());
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnL2(const pb::dms::L2 &l2)
    {//测试发送数据
        LOG_INFO("send L2: ", fh::core::assist::utility::Format_pb_message(l2));
	return;
        //
        LOG_INFO("send L2: ", fh::core::assist::utility::Format_pb_message(l2));
        m_book_sender.Send("L" + l2.SerializeAsString());
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnL3()
    {
        // noop
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnTrade(const pb::dms::Trade &trade)
    {
        //
        LOG_INFO("send Trade: ", fh::core::assist::utility::Format_pb_message(trade));
        m_book_sender.Send("T" + trade.SerializeAsString());
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnContractAuctioning(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [Auction]");
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnContractNoTrading(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [NoTrading]");
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnContractTrading(const std::string &contract)
    {
		LOG_INFO("contract[", contract, "] status update [Trading]");
    }

    // implement of MarketListenerI
    void CFemasBookSender::OnOrginalMessage(const std::string &message)
    {
        LOG_INFO("send Original Message, size=", message.size());
        m_org_sender.Send(message);
    }

    	

} // namespace market
} // namespace femas
}// namespace fh