#include <iostream>
#include <fstream>
#include <unordered_map>
#include "custom_manager.h"
#include "custom_md_spi.h"
#include "core/assist/logger.h"

namespace fh
{
namespace ctp
{
namespace market
{
	
template <typename IntType>
inline std::string T(IntType v){return std::to_string(v);}
inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
inline std::string T(const char *v){return std::string(v);}
inline std::string T(char *v){return std::string(v);}	
	
// ---- ctp_api回调函数 ---- //
// 连接成功应答
void CustomMdSpi::OnFrontConnected()
{
	LOG_INFO("=====Create a network connection successfully=====");
	conn = true;
}

// 断开连接通知
void CustomMdSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====The network connection is disconnected=====" << std::endl;
	std::cerr << "error code:" << nReason << std::endl;
}

// 心跳超时警告
void CustomMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	LOG_INFO("Distance from last connection time: ");
}

// 登录应答
void CustomMdSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		LOG_INFO("=====Account login successful=====",
		"\nTrading Day: ", pRspUserLogin->TradingDay,
		"\nLogin Time: ", pRspUserLogin->LoginTime,
		"\nBroker ID: ", pRspUserLogin->BrokerID,
		"\nUser ID: ", pRspUserLogin->UserID);
		mdable = true;
	}
	else
		LOG_INFO("Returns an error--->>> ErrorID=", pRspInfo->ErrorID, " ErrorMsg=", pRspInfo->ErrorMsg);
}

// 登出应答
void CustomMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====Account to log out successfully=====" << std::endl;
		std::cout << "Broker ID: " << pUserLogout->BrokerID << std::endl;
		std::cout << "User ID: " << pUserLogout->UserID << std::endl;
	}
	else
		LOG_INFO("Returns an error--->>> ErrorID=", pRspInfo->ErrorID, ", ErrorMsg=", pRspInfo->ErrorMsg);
}

// 错误通知
void CustomMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		LOG_INFO("Returns an error--->>> ErrorID=", pRspInfo->ErrorID, " ErrorMsg=", pRspInfo->ErrorMsg);
}

// 订阅行情应答
void CustomMdSpi::OnRspSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====Subscribe to the market success=====" << std::endl;
		std::cout << "Instrument ID " << pSpecificInstrument->InstrumentID << std::endl;
		// 如果需要存入文件或者数据库，在这里创建表头,不同的合约单独存储
		char filePath[100] = {'\0'};
		sprintf(filePath, "%s_market_data.csv", pSpecificInstrument->InstrumentID);
		std::ofstream outFile;
		outFile.open(filePath, std::ios::out); // 新开文件
		outFile << "合约代码" << ","
			<< "更新时间" << ","
			<< "最新价" << ","
			<< "成交量" << ","
			<< "买价一" << ","
			<< "买量一" << ","
			<< "卖价一" << ","
			<< "卖量一" << ","
			<< "持仓量" << ","
			<< "换手率"
			<< std::endl;
		outFile.close();
	}
	else
		LOG_INFO("Returns an error--->>> ErrorID=", pRspInfo->ErrorID, " ErrorMsg=", pRspInfo->ErrorMsg);
}

// 取消订阅行情应答
void CustomMdSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		LOG_INFO("=====Unsubscribe market success=====\nInstrument ID :", pSpecificInstrument->InstrumentID);
	}
	else
		LOG_INFO("Returns an error--->>> ErrorID=", pRspInfo->ErrorID, " ErrorMsg=", pRspInfo->ErrorMsg);
}

// 订阅询价应答
void CustomMdSpi::OnRspSubForQuoteRsp(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		LOG_INFO("=====Subscribe to the inquiry success=====\nInstrument ID :", pSpecificInstrument->InstrumentID);
	}
	else
		LOG_INFO("Returns an error--->>> ErrorID=", pRspInfo->ErrorID, " ErrorMsg=", pRspInfo->ErrorMsg);
}

// 取消订阅询价应答
void CustomMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		LOG_INFO("=====Unsubscribe inquiry successfully=====\nInstrument ID :",  pSpecificInstrument->InstrumentID);
	}
	else
		LOG_INFO("Returns an error--->>> ErrorID=", pRspInfo->ErrorID, " ErrorMsg=", pRspInfo->ErrorMsg);
}

// 行情详情通知
void CustomMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	// 打印行情，字段较多，截取部分
	LOG_INFO("=====Get deep market=====\n",
	"Trading day: ", pDepthMarketData->TradingDay,
	"\nExchange ID: ", pDepthMarketData->ExchangeID,
	"\nInstrument ID: ", pDepthMarketData->InstrumentID,
	"\nExchange Inst ID: ", pDepthMarketData->ExchangeInstID,
	"\nLast Price: ", pDepthMarketData->LastPrice,
	"\nVolume: ", pDepthMarketData->Volume);
	// 如果只获取某一个合约行情，可以逐tick地存入文件或数据库
	char filePath[100] = {'\0'};
	sprintf(filePath, "%s_market_data.csv", pDepthMarketData->InstrumentID);
	std::ofstream outFile;
	outFile.open(filePath, std::ios::app); // 文件追加写入 
	outFile << pDepthMarketData->InstrumentID << "," 
		<< pDepthMarketData->UpdateTime << "." << pDepthMarketData->UpdateMillisec << "," 
		<< pDepthMarketData->LastPrice << "," 
		<< pDepthMarketData->Volume << "," 
		<< pDepthMarketData->BidPrice1 << "," 
		<< pDepthMarketData->BidVolume1 << "," 
		<< pDepthMarketData->AskPrice1 << "," 
		<< pDepthMarketData->AskVolume1 << "," 
		<< pDepthMarketData->OpenInterest << "," 
		<< pDepthMarketData->Turnover << std::endl;
	outFile.close();

	SendDepthMarketData(pDepthMarketData);
	StructToJSON(pDepthMarketData);
	   
	// 取消订阅行情
	//int rt = m_pMdUserApi->UnSubscribeMarketData(g_pInstrumentID, instrumentNum);
	//if (!rt)
	//	std::cout << ">>>>>>发送取消订阅行情请求成功" << std::endl;
	//else
	//	std::cerr << "--->>>发送取消订阅行情请求失败" << std::endl;
}

// 询价详情通知
void CustomMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	// 部分询价结果
	LOG_INFO("=====Get inquiry results=====\nTrading Day: ", pForQuoteRsp->TradingDay,
	"\nExchange ID: ", pForQuoteRsp->ExchangeID,
	"\nInstrument ID： ", pForQuoteRsp->InstrumentID,
	"\nFor Quote SysID: ", pForQuoteRsp->ForQuoteSysID);
}

// ---- 自定义函数 ---- //
void CustomMdSpi::SendDepthMarketData(CThostFtdcDepthMarketDataField *pMarketData)
{
	LOG_INFO("CCtpBookManager::SendCtpmarketData ");     
	if(NULL == pMarketData)
	{
		LOG_INFO("Error pMarketData is NULL ");
		return;
	}
	pb::dms::L2 l2_info;

	l2_info.set_contract(pMarketData->InstrumentID);


	pb::dms::DataPoint *bid;// = l2_info.add_bid();
	pb::dms::DataPoint *ask;// = l2_info.add_offer();

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

	// 发送 trade 数据
	pb::dms::Trade trade;
	pb::dms::DataPoint *last = trade.mutable_last();
	last->set_price(pMarketData->LastPrice);
	last->set_size(pMarketData->Volume - m_last_volume);
	m_book_sender->OnTrade(trade);	
	m_last_volume = pMarketData->Volume;
}

void CustomMdSpi::StructToJSON(CThostFtdcDepthMarketDataField *pMarketData)
{
    if(NULL == pMarketData)
    {
        return;
    }
    bsoncxx::builder::basic::document json;
    json.append(bsoncxx::builder::basic::kvp("TradingDay", T(pMarketData->TradingDay)));	
    json.append(bsoncxx::builder::basic::kvp("PreSettlementPrice", T(pMarketData->PreSettlementPrice)));	
    json.append(bsoncxx::builder::basic::kvp("PreClosePrice", T(pMarketData->PreClosePrice)));
    json.append(bsoncxx::builder::basic::kvp("PreOpenInterest", T(pMarketData->PreOpenInterest)));	
    json.append(bsoncxx::builder::basic::kvp("PreDelta", T(pMarketData->PreDelta)));
    json.append(bsoncxx::builder::basic::kvp("OpenPrice", T(pMarketData->OpenPrice)));
    json.append(bsoncxx::builder::basic::kvp("HighestPrice", T(pMarketData->HighestPrice)));	

    json.append(bsoncxx::builder::basic::kvp("LowestPrice", T(pMarketData->LowestPrice)));
    json.append(bsoncxx::builder::basic::kvp("ClosePrice", T(pMarketData->ClosePrice)));
    json.append(bsoncxx::builder::basic::kvp("UpperLimitPrice", T(pMarketData->UpperLimitPrice)));	

    json.append(bsoncxx::builder::basic::kvp("LowerLimitPrice", T(pMarketData->LowerLimitPrice)));
    json.append(bsoncxx::builder::basic::kvp("SettlementPrice", T(pMarketData->SettlementPrice)));
    json.append(bsoncxx::builder::basic::kvp("CurrDelta", T(pMarketData->CurrDelta)));
    json.append(bsoncxx::builder::basic::kvp("LastPrice", T(pMarketData->LastPrice)));
    json.append(bsoncxx::builder::basic::kvp("Volume", T(pMarketData->Volume)));
    json.append(bsoncxx::builder::basic::kvp("Turnover", T(pMarketData->Turnover)));
    json.append(bsoncxx::builder::basic::kvp("OpenInterest", T(pMarketData->OpenInterest)));
    json.append(bsoncxx::builder::basic::kvp("BidPrice1", T(pMarketData->BidPrice1)));
    json.append(bsoncxx::builder::basic::kvp("BidVolume1", T(pMarketData->BidVolume1)));
    json.append(bsoncxx::builder::basic::kvp("AskPrice1", T(pMarketData->AskPrice1)));
    json.append(bsoncxx::builder::basic::kvp("AskVolume1", T(pMarketData->AskVolume1)));
    json.append(bsoncxx::builder::basic::kvp("BidPrice2", T(pMarketData->BidPrice2)));	


    json.append(bsoncxx::builder::basic::kvp("BidVolume2", T(pMarketData->BidVolume2)));
    json.append(bsoncxx::builder::basic::kvp("BidPrice3", T(pMarketData->BidPrice3)));
    json.append(bsoncxx::builder::basic::kvp("BidVolume3", T(pMarketData->BidVolume3)));
    json.append(bsoncxx::builder::basic::kvp("AskPrice2", T(pMarketData->AskPrice2)));
    json.append(bsoncxx::builder::basic::kvp("AskVolume2", T(pMarketData->AskVolume2)));
    json.append(bsoncxx::builder::basic::kvp("AskPrice3", T(pMarketData->AskPrice3)));
    json.append(bsoncxx::builder::basic::kvp("AskVolume3", T(pMarketData->AskVolume3)));
    json.append(bsoncxx::builder::basic::kvp("BidPrice4", T(pMarketData->BidPrice4)));
    json.append(bsoncxx::builder::basic::kvp("BidVolume4", T(pMarketData->BidVolume4)));
    json.append(bsoncxx::builder::basic::kvp("BidPrice5", T(pMarketData->BidPrice5)));
    json.append(bsoncxx::builder::basic::kvp("BidVolume5", T(pMarketData->BidVolume5)));
    json.append(bsoncxx::builder::basic::kvp("AskPrice4", T(pMarketData->AskPrice4)));	


    json.append(bsoncxx::builder::basic::kvp("AskVolume4", T(pMarketData->AskVolume4)));
    json.append(bsoncxx::builder::basic::kvp("AskPrice5", T(pMarketData->AskPrice5)));
    json.append(bsoncxx::builder::basic::kvp("AskVolume5", T(pMarketData->AskVolume5)));
    json.append(bsoncxx::builder::basic::kvp("InstrumentID", T(pMarketData->InstrumentID)));
    json.append(bsoncxx::builder::basic::kvp("UpdateTime", T(pMarketData->UpdateTime)));
    json.append(bsoncxx::builder::basic::kvp("UpdateMillisec", T(pMarketData->UpdateMillisec)));
    json.append(bsoncxx::builder::basic::kvp("ActionDay", T(pMarketData->ActionDay)));
	
	bsoncxx::builder::basic::document tmjson;
	tmjson.append(bsoncxx::builder::basic::kvp("market", T("ctp")));		  
	tmjson.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
	tmjson.append(bsoncxx::builder::basic::kvp("sendingTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
	tmjson.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(fh::core::assist::utility::Current_time_str())));	
	tmjson.append(bsoncxx::builder::basic::kvp("receivedTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
	tmjson.append(bsoncxx::builder::basic::kvp("message", json));	
    
    m_book_sender->OnOrginalMessage(bsoncxx::to_json(tmjson.view()));	
}

}
}
}