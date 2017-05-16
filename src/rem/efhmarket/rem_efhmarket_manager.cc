#include "rem_efhmarket_manager.h"
#include "core/assist/logger.h"



namespace fh
{
namespace rem
{
namespace efhmarket
{



CRemEfhMarkrtManager::CRemEfhMarkrtManager()
{
    memset(&m_mcinfo,0,sizeof(multicast_info));
}

CRemEfhMarkrtManager::~CRemEfhMarkrtManager()
{
    delete m_pFileConfig;
    delete m_book_sender;	
}

void CRemEfhMarkrtManager::on_receive_nomal(guava_udp_normal* data)
{
    if(NULL == data)
    {
        return ;
    }
    SendRemmarketData(data);
    StructToJSON(data);	
}

bool CRemEfhMarkrtManager::run()
{
    if(NULL == m_pFileConfig)
    {
        LOG_ERROR("CRemEfhMarkrtManager::run error  ");
	 return false;
    }
    std::string remote_ip = m_pFileConfig->Get("rem-efhmarket.remote_ip");
    int remote_port = std::atoi(m_pFileConfig->Get("rem-efhmarket.remote_port").c_str());	
    std::string local_ip = m_pFileConfig->Get("rem-efhmarket.local_ip");
    int local_port = std::atoi(m_pFileConfig->Get("rem-efhmarket.local_port").c_str());	

    strcpy(m_mcinfo.m_remote_ip, remote_ip.c_str());
    m_mcinfo.m_remote_port = remote_port;
    strcpy(m_mcinfo.m_local_ip, local_ip.c_str());
    m_mcinfo.m_local_port = remote_port;	
    bool ret = m_guava.init(m_mcinfo, this);	
    if(!ret)
    {
        LOG_ERROR("guava.init error ");
        return false;
    }
	
    return true;
}

void CRemEfhMarkrtManager::SetFileData(std::string &FileConfig)
{
      LOG_INFO("CRemEfhMarkrtManager::SetFileData  ",FileConfig.c_str());
      m_pFileConfig = new fh::core::assist::Settings(FileConfig);
      if(NULL == m_pFileConfig)
      {
          LOG_ERROR("Error m_pFileConfig is NULL ");
      }
      return;
}

void CRemEfhMarkrtManager::SendRemmarketData(guava_udp_normal *pMarketData)
{
    if(NULL == m_book_sender)
    {
        LOG_ERROR("Error m_book_sender is NULL ");
        return;     
    }
    pb::dms::L2 l2_info;
    l2_info.set_contract(pMarketData->m_symbol);
    pb::dms::DataPoint *bid;// = l2_info.add_bid();
    pb::dms::DataPoint *ask;// = l2_info.add_offer();	

    if (pMarketData->m_bid_px != DBL_MAX && pMarketData->m_bid_share > 0)
    {
        bid = l2_info.add_bid();
        bid->set_price(pMarketData->m_bid_px);
	 bid->set_size(pMarketData->m_bid_share);	  
    }
    if (pMarketData->m_ask_px != DBL_MAX && pMarketData->m_ask_share > 0)
    {
        ask = l2_info.add_offer();
        ask->set_price(pMarketData->m_ask_px);
	 ask->set_size(pMarketData->m_ask_share);	  
    }
    m_book_sender->OnL2(l2_info);
	
    //发送最优价
	if(pMarketData->m_bid_px == DBL_MAX && pMarketData->m_ask_px == DBL_MAX)
	{
           LOG_INFO("Bid and Offer NULL ");
	}
	else
	if(pMarketData->m_bid_px == DBL_MAX)	
	{
           pb::dms::Offer offer_info;
	    offer_info.set_contract(pMarketData->m_symbol);	   
	    pb::dms::DataPoint *offer = offer_info.mutable_offer();
	    offer->set_price(pMarketData->m_ask_px);
	    offer->set_size(pMarketData->m_ask_share);	
	    m_book_sender->OnOffer(offer_info);
	}
	else
	if(pMarketData->m_ask_px == DBL_MAX)	
	{
           pb::dms::Bid bid_info;
	    bid_info.set_contract(pMarketData->m_symbol);	   
	    pb::dms::DataPoint *bid = bid_info.mutable_bid();
	    bid->set_price(pMarketData->m_bid_px);
	    bid->set_size(pMarketData->m_bid_share);	
	    m_book_sender->OnBid(bid_info);
	}	
	else
	{
           pb::dms::BBO bbo_info;
	    bbo_info.set_contract(pMarketData->m_symbol);		
           pb::dms::DataPoint *bid = bbo_info.mutable_bid();
	    bid->set_price(pMarketData->m_bid_px);
	    bid->set_price(pMarketData->m_bid_share);	
           pb::dms::DataPoint *ask = bbo_info.mutable_offer();
           ask->set_price(pMarketData->m_ask_px);
           ask->set_size(pMarketData->m_ask_share);	   
           m_book_sender->OnBBO(bbo_info);
		
	} 
}

void CRemEfhMarkrtManager::SendRemToDB(const std::string &message)
{
    m_book_sender->OnOrginalMessage(message);
    return;	
}

void CRemEfhMarkrtManager::StructToJSON(guava_udp_normal *pMarketData)
{
    if(NULL == pMarketData)
    {
        return;
    }
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("sequence", T(pMarketData->m_sequence)));
    tmjson.append(bsoncxx::builder::basic::kvp("exchange_id", T(pMarketData->m_exchange_id)));	
    tmjson.append(bsoncxx::builder::basic::kvp("channel_id", T(pMarketData->m_channel_id)));
	
    tmjson.append(bsoncxx::builder::basic::kvp("quote_flag", T(pMarketData->m_quote_flag)));
    tmjson.append(bsoncxx::builder::basic::kvp("symbol", T(pMarketData->m_symbol)));
    tmjson.append(bsoncxx::builder::basic::kvp("update_time", T(pMarketData->m_update_time)));
    tmjson.append(bsoncxx::builder::basic::kvp("millisecond", T(pMarketData->m_millisecond)));
		
    tmjson.append(bsoncxx::builder::basic::kvp("last_px", T(pMarketData->m_last_px==DBL_MAX ? 0.0 : pMarketData->m_last_px)));	
	
    tmjson.append(bsoncxx::builder::basic::kvp("last_share", T(pMarketData->m_last_share>0 ? 0.0 : pMarketData->m_last_share)));
	
    tmjson.append(bsoncxx::builder::basic::kvp("total_value", T(pMarketData->m_total_value==DBL_MAX ? 0.0 : pMarketData->m_total_value)));
    tmjson.append(bsoncxx::builder::basic::kvp("total_pos", T(pMarketData->m_total_pos>DBL_MAX ? 0.0 : pMarketData->m_total_pos)));	
	
    tmjson.append(bsoncxx::builder::basic::kvp("bid_px", T(pMarketData->m_bid_px==DBL_MAX ? 0.0 : pMarketData->m_bid_px)));
    tmjson.append(bsoncxx::builder::basic::kvp("bid_share", T(pMarketData->m_bid_share>0 ? 0.0 : pMarketData->m_bid_share)));
    tmjson.append(bsoncxx::builder::basic::kvp("ask_px", T(pMarketData->m_ask_px==DBL_MAX ? 0.0 : pMarketData->m_ask_px)));	
    tmjson.append(bsoncxx::builder::basic::kvp("ask_share", T(pMarketData->m_ask_share>0 ? 0.0 : pMarketData->m_ask_share)));
    	
    RemDateToString(tmjson);	
    return;
}

void CRemEfhMarkrtManager::RemDateToString(bsoncxx::builder::basic::document& json)
{
    bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("market", T("REMEFH")));		  
    tmjson.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(fh::core::assist::utility::Current_time_str())));	
    tmjson.append(bsoncxx::builder::basic::kvp("receivedTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmjson.append(bsoncxx::builder::basic::kvp("message", json));	

    SendRemToDB(bsoncxx::to_json(tmjson.view()));
    return;
}

}
}
}