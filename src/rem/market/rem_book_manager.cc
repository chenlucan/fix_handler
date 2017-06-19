#include "rem/market/rem_book_manager.h"
#include "core/assist/logger.h"
namespace fh
{
namespace rem
{
namespace market
{



CRemBookManager::CRemBookManager(fh::core::market::MarketListenerI *sender)
{
        // noop
        m_book_sender = sender;
}      


CRemBookManager::~CRemBookManager()
{
        // noop
}   




void CRemBookManager::SendRemmarketData(EESMarketDepthQuoteData *pMarketData)
{
	LOG_INFO("CRemBookManager::SendFemasmarketData ");     
	if(NULL == pMarketData)
	{
       	        LOG_INFO("Error pMarketData is NULL ");
		return;
	}
	pb::dms::L2 l2_info;

       l2_info.set_contract(pMarketData->InstrumentID);

	   
	pb::dms::DataPoint *bid;
	pb::dms::DataPoint *ask;
	
	if (pMarketData->BidPrice1==DBL_MAX || pMarketData->BidVolume1 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice1);
	    bid->set_size(pMarketData->BidVolume1);	   
	}	
		
	if (pMarketData->AskPrice1==DBL_MAX || pMarketData->AskVolume1 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice1);
	    ask->set_size(pMarketData->AskVolume1);	   
	}
	     	
	if (pMarketData->BidPrice2==DBL_MAX || pMarketData->BidVolume2 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice2);
	    bid->set_size(pMarketData->BidVolume2);	   
	}	
	
	if (pMarketData->AskPrice2==DBL_MAX || pMarketData->AskVolume2 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice2);
	    ask->set_size(pMarketData->AskVolume2);	   
	}
	       	
	if (pMarketData->BidPrice3==DBL_MAX || pMarketData->BidVolume3 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice3);
	    bid->set_size(pMarketData->BidVolume3);	   
	}	
	
	if (pMarketData->AskPrice3==DBL_MAX || pMarketData->AskVolume3 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice3);
	    ask->set_size(pMarketData->AskVolume3);	   
	}
	       	
	if (pMarketData->BidPrice4==DBL_MAX || pMarketData->BidVolume4 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice4);
	    bid->set_size(pMarketData->BidVolume4);	   
	}	
	
	if (pMarketData->AskPrice4==DBL_MAX || pMarketData->AskVolume4 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice4);
	    ask->set_size(pMarketData->AskVolume4);	   
	}
		
	if (pMarketData->BidPrice5==DBL_MAX || pMarketData->BidVolume5 <= 0)
	{
           //bid->set_price(0.0);
	}
	else
	{
	    bid = l2_info.add_bid();
           bid->set_price(pMarketData->BidPrice5);
	    bid->set_size(pMarketData->BidVolume5);	   
	}	
	
	if (pMarketData->AskPrice5==DBL_MAX || pMarketData->AskVolume5 <= 0)
	{
           //ask->set_price(0.0);
	}
	else
	{
	    ask = l2_info.add_offer();
           ask->set_price(pMarketData->AskPrice5);
	    ask->set_size(pMarketData->AskVolume5);	   
	}
	
	m_book_sender->OnL2(l2_info);

	//以上发送L2 行情

	//发送最优价
	if((pMarketData->BidPrice1 == DBL_MAX || pMarketData->BidVolume1 <= 0) && (pMarketData->AskVolume1 <= 0 || pMarketData->AskPrice1 == DBL_MAX))
	{
           LOG_INFO("Bid and Offer NULL ");
	}
	else
	if(pMarketData->BidPrice1 == DBL_MAX || pMarketData->BidVolume1 <= 0)	
	{
           pb::dms::Offer offer_info;
	    offer_info.set_contract(pMarketData->InstrumentID);	   
	    pb::dms::DataPoint *offer = offer_info.mutable_offer();
	    offer->set_price(pMarketData->AskPrice1);
	    offer->set_size(pMarketData->AskVolume1);	
	    m_book_sender->OnOffer(offer_info);
	}
	else
	if(pMarketData->AskPrice1 == DBL_MAX || pMarketData->AskVolume1 <= 0)	
	{
           pb::dms::Bid bid_info;
	    bid_info.set_contract(pMarketData->InstrumentID);	   
	    pb::dms::DataPoint *bid = bid_info.mutable_bid();
	    bid->set_price(pMarketData->BidPrice1);
	    bid->set_size(pMarketData->BidVolume1);	
	    m_book_sender->OnBid(bid_info);
	}	
	else
	{
           pb::dms::BBO bbo_info;
	    bbo_info.set_contract(pMarketData->InstrumentID);		
           pb::dms::DataPoint *bid = bbo_info.mutable_bid();
	    bid->set_price(pMarketData->BidPrice1);
	    bid->set_price(pMarketData->BidVolume1);	
           pb::dms::DataPoint *ask = bbo_info.mutable_offer();
           ask->set_price(pMarketData->AskPrice1);
           ask->set_size(pMarketData->AskVolume1);	   
           m_book_sender->OnBBO(bbo_info);
		
	}
	
}

void CRemBookManager::SendRemToDB(const std::string &message)
{	
    m_book_sender->OnOrginalMessage(message);
    return;	
}






} // namespace market
} // namespace rem
} // namespace fh

