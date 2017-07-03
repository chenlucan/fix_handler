#ifndef __FH_REM_CRemEfhMarketManager_H__
#define __FH_REM_CRemEfhMarketManager_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#pragma once
#include <vector>
#include "core/global.h"
#include "core/zmq/zmq_sender.h"
#include "core/market/marketlisteneri.h"
#include "rem_guava_quote.h"
#include "core/assist/settings.h"
#include "core/assist/utility.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include "core/global.h"
#include<time.h>
#include <ctime>
#include <stdlib.h>
typedef struct strade
{
    int mvolume;
    std::string mtime;
    strade()
    {
        mvolume = 0;
	 mtime = "";	
    }
} mstrade;

typedef std::map <std::string,mstrade*> TradeMap;

typedef unsigned long long  ullong;

using std::vector;

namespace fh
{
namespace rem
{
namespace efhmarket
{

template <typename IntType>
inline std::string T(IntType v){return std::to_string(v);}
inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
inline std::string T(const char *v){return std::string(v);}
inline std::string T(char *v){return std::string(v);}

class CRemEfhMarkrtManager : public guava_quote_event
{
    public:
		CRemEfhMarkrtManager(fh::core::market::MarketListenerI *book_sender);
		virtual ~CRemEfhMarkrtManager();

		virtual void on_receive_nomal(guava_udp_normal* data);

		bool run();
		void SetFileData(std::string &FileConfig);

		void SendRemmarketData(guava_udp_normal *pMarketData);
	       void SendRemToDB(const std::string &message);

		void StructToJSON(guava_udp_normal *pMarketData);
		void RemDateToString(bsoncxx::builder::basic::document& json,char* InstrumentID,std::string updatetime,ullong tmp_time);   
		std::string GetUpdateTimeStr(guava_udp_normal *pMarketData);
		ullong str2stmp(const char *strTime);
		ullong GetUpdateTimeInt(guava_udp_normal *pMarketData);

		int MakePriceVolume(guava_udp_normal *pMarketData);	
	       //void ClearMap();	
	       void CheckTime(guava_udp_normal *pMarketData);	

     public:
		fh::core::market::MarketListenerI *m_book_sender;
		fh::core::assist::Settings *m_pFileConfig;
		multicast_info m_mcinfo;
		guava_quote m_guava;
		TradeMap m_trademap;	 

};








}
}
}

#endif