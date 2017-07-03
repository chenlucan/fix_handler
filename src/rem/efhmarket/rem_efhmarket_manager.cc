#include "rem_efhmarket_manager.h"
#include "core/assist/logger.h"



namespace fh
{
namespace rem
{
namespace efhmarket
{



CRemEfhMarkrtManager::CRemEfhMarkrtManager(fh::core::market::MarketListenerI *book_sender)
{
    memset(&m_mcinfo,0,sizeof(multicast_info));
    m_book_sender = book_sender;	
    m_trademap.clear();	
}

CRemEfhMarkrtManager::~CRemEfhMarkrtManager()
{
    delete m_pFileConfig;
    //delete m_book_sender;	
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
    m_mcinfo.m_local_port = local_port;	
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
    //m_book_sender->OnL2(l2_info);
	
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
	//发送teade行情
	int tmpvolume = MakePriceVolume(pMarketData);
	LOG_INFO("CFemasBookManager::MakePriceVolume = ",tmpvolume); 
	if(tmpvolume > 0)
	{
            pb::dms::Trade trade_info;
	     trade_info.set_contract(pMarketData->m_symbol);	
	     pb::dms::DataPoint *trade_id = trade_info.mutable_last();	
	     trade_id->set_price(pMarketData->m_last_px);
	     trade_id->set_size(tmpvolume);	
	     m_book_sender->OnTrade(trade_info);	 
	}

	pb::dms::Turnover Turnoverinfo;
       Turnoverinfo.set_contract(pMarketData->m_symbol);
	Turnoverinfo.set_total_volume(pMarketData->m_last_share);
	Turnoverinfo.set_turnover(pMarketData->m_total_value);
	m_book_sender->OnTurnover(Turnoverinfo);

	m_book_sender->OnL2(l2_info);
}

void CRemEfhMarkrtManager::CheckTime(guava_udp_normal *pMarketData)
{
    LOG_INFO("CRemEfhMarkrtManager::CheckTime "); 
    char ctmpf[3]={0};
    char ctmps[3]={0};	
    strncpy(ctmpf,pMarketData->m_update_time,2);	
    strncpy(ctmps,(m_trademap[pMarketData->m_symbol]->mtime).c_str(),2);		
    if(std::atoi(ctmpf) > 18 && std::atoi(ctmps) < 18)
    {
        LOG_INFO("CRemEfhMarkrtManager::clear  Instrument map");
        m_trademap[pMarketData->m_symbol]->mvolume=pMarketData->m_last_share;
	 m_trademap[pMarketData->m_symbol]->mtime=pMarketData->m_update_time;	
    }
}

//void CFemasBookManager::ClearMap()
//{
//    m_trademap.clear();
//}

int CRemEfhMarkrtManager::MakePriceVolume(guava_udp_normal *pMarketData)
{
    LOG_INFO("CRemEfhMarkrtManager::SendFemasmarketData "); 
    if(m_trademap.count(pMarketData->m_symbol) == 0)
    {
        
        m_trademap[pMarketData->m_symbol] = new mstrade();
	 m_trademap[pMarketData->m_symbol]->mvolume=pMarketData->m_last_share;
	 m_trademap[pMarketData->m_symbol]->mtime=pMarketData->m_update_time;
        return 0;
    }
    else
    {
        CheckTime(pMarketData);
        int tmpVolume =  pMarketData->m_last_share - m_trademap[pMarketData->m_symbol]->mvolume;
	 m_trademap[pMarketData->m_symbol]->mvolume=pMarketData->m_last_share;
	 m_trademap[pMarketData->m_symbol]->mtime=pMarketData->m_update_time;	
        return (tmpVolume > 0 ? tmpVolume : 0);     
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
		
    tmjson.append(bsoncxx::builder::basic::kvp("last_px", T(pMarketData->m_last_px)));	
	
    tmjson.append(bsoncxx::builder::basic::kvp("last_share", T(pMarketData->m_last_share<0 ? 0.0 : pMarketData->m_last_share)));
	
    tmjson.append(bsoncxx::builder::basic::kvp("total_value", T(pMarketData->m_total_value)));
    tmjson.append(bsoncxx::builder::basic::kvp("total_pos", T(pMarketData->m_total_pos)));	
	
    tmjson.append(bsoncxx::builder::basic::kvp("bid_px", T(pMarketData->m_bid_px)));
    tmjson.append(bsoncxx::builder::basic::kvp("bid_share", T(pMarketData->m_bid_share<0 ? 0.0 : pMarketData->m_bid_share)));
    tmjson.append(bsoncxx::builder::basic::kvp("ask_px", T(pMarketData->m_ask_px)));	
    tmjson.append(bsoncxx::builder::basic::kvp("ask_share", T(pMarketData->m_ask_share<0 ? 0.0 : pMarketData->m_ask_share)));
    	
    RemDateToString(tmjson,pMarketData->m_symbol,GetUpdateTimeStr(pMarketData),GetUpdateTimeInt(pMarketData));	
    return;
}

void CRemEfhMarkrtManager::RemDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID,std::string updatetime,ullong tmp_time)
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

ullong CRemEfhMarkrtManager::str2stmp(const char *strTime)
{
     if (strTime != NULL)
     {
         struct tm sTime;
 #ifdef __GNUC__
         strptime(strTime, "%Y-%m-%d %H:%M:%S", &sTime);
 #else
         sscanf(strTime, "%d-%d-%d %d:%d:%d", &sTime.tm_year, &sTime.tm_mon, &sTime.tm_mday, &sTime.tm_hour, &sTime.tm_min, &sTime.tm_sec);
         sTime.tm_year -= 1900;
         sTime.tm_mon -= 1;
 #endif
         ullong ft = mktime(&sTime);
         return ft;
     }
     else {
         return time(0);
     }
}

ullong CRemEfhMarkrtManager::GetUpdateTimeInt(guava_udp_normal *pMarketData)
{
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数
    struct tm nowTime;
    localtime_r(&time.tv_sec, &nowTime);
    char current[1024]={0};
    sprintf(current, "%04d%02d%02d", nowTime.tm_year + 1900, nowTime.tm_mon+1, nowTime.tm_mday);

    std::string timestr="";
    char ctmp[20]={0};	
    strncpy(ctmp,current,4);
    ctmp[4] = '-';
    strncpy(ctmp+5,current+4,2);	
    ctmp[7] = '-';
    strncpy(ctmp+8,current+6,2);
    ctmp[10] = ' ';
    timestr = ctmp;	
    timestr+=pMarketData->m_update_time;
    ullong tmp_time = 0;
    tmp_time = str2stmp(timestr.c_str());	
    tmp_time *= 1000;
    tmp_time += pMarketData->m_millisecond;
    tmp_time *= 1000;
    tmp_time *= 1000;	
    return tmp_time;	
}

std::string CRemEfhMarkrtManager::GetUpdateTimeStr(guava_udp_normal *pMarketData)
{
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数
    struct tm nowTime;
    localtime_r(&time.tv_sec, &nowTime);
    char current[1024]={0};
    sprintf(current, "%04d%02d%02d", nowTime.tm_year + 1900, nowTime.tm_mon+1, nowTime.tm_mday);

    std::string timestr="";
    char ctmp[20]={0};	
    strncpy(ctmp,current,4);
    ctmp[4] = '-';
    strncpy(ctmp+5,current+4,2);	
    ctmp[7] = '-';
    strncpy(ctmp+8,current+6,2);	
    ctmp[10] = ' ';
    timestr = ctmp;	
    timestr+=pMarketData->m_update_time;
    timestr+=".";	 
    std::string tmp = std::to_string(pMarketData->m_millisecond);
    tmp += "000";
    timestr += tmp;	
    	
    return timestr;
}

}
}
}