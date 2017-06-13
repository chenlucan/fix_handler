

#include <stdio.h>
#ifdef __unix__
#include <execinfo.h>
#endif
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <signal.h>

#include "femas/market/femas_market_manager.h"
#include "femas/market/femas_market_application.h"
#include "femas/market/convert/Femas_book_convert.h"
#include "core/assist/logger.h"
template <typename IntType>
inline std::string T(IntType v){return std::to_string(v);}
inline std::string T(const std::string &v){return fh::core::assist::utility::Trim_null(v);}
inline std::string T(const char *v){return std::string(v);}
inline std::string T(char *v){return std::string(v);}

bool stop_all=false;

void handler(int sig) 
{
#ifdef __unix__
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
#endif
}


void onsignal(int sig)
{
    switch (sig)
    {
        case SIGINT:
            stop_all = true;
            break;

        case SIGTRAP:
            //printf("\r\n SIGTRAP\n");
            break;

        case SIGPIPE:
            //printf("\r\n SIGPIPE\n");
            break;
    }

    return;
}

int set_SignalProc()
{
    signal(SIGINT,  &onsignal);
    signal(SIGTRAP, &onsignal);
    signal(SIGPIPE, &onsignal);
    return 0;
}

int main_loop()
{
        while(!stop_all)
       {
           LOG_INFO("FemasMarket  runing  main_loop");
	    sleep(10);	 
       }
	return 0;
}

int main(int argc, char* argv[])
{

     //signal(SIGSEGV, handler);
     set_SignalProc();
     LOG_INFO("main() start");

//========================================================================================================
     fh::femas::market::convert::FemasBookConvert* pFemasBookConvert = new fh::femas::market::convert::FemasBookConvert();
//--
bsoncxx::builder::basic::document tmjson;
    tmjson.append(bsoncxx::builder::basic::kvp("TradingDay", T("20170613")));
    tmjson.append(bsoncxx::builder::basic::kvp("SettlementGroupID", T("00000001")));	
    tmjson.append(bsoncxx::builder::basic::kvp("SettlementID", T(1)));	
    tmjson.append(bsoncxx::builder::basic::kvp("PreSettlementPrice", T(20990.000000)));	
    tmjson.append(bsoncxx::builder::basic::kvp("PreClosePrice", T(21045.000000)));
    tmjson.append(bsoncxx::builder::basic::kvp("PreOpenInterest", T(119024.000000)));	
    tmjson.append(bsoncxx::builder::basic::kvp("PreDelta", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("OpenPrice", T(21000.000000)));
    tmjson.append(bsoncxx::builder::basic::kvp("HighestPrice", T(21100.000000)));	

    tmjson.append(bsoncxx::builder::basic::kvp("LowestPrice", T(20475.000000)));
    tmjson.append(bsoncxx::builder::basic::kvp("ClosePrice", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("UpperLimitPrice", T(22245.000000)));	

    tmjson.append(bsoncxx::builder::basic::kvp("LowerLimitPrice", T(19730.000000)));
    tmjson.append(bsoncxx::builder::basic::kvp("SettlementPrice", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("CurrDelta", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("LastPrice", T(20570.000000)));
    tmjson.append(bsoncxx::builder::basic::kvp("Volume", T(79956)));
    tmjson.append(bsoncxx::builder::basic::kvp("Turnover", T(8315253950.000000)));
    tmjson.append(bsoncxx::builder::basic::kvp("OpenInterest", T(125114.000000)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice1", T(20565.000000)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume1", T(4)));

    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice1", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume1", T(0)));	
    //tmjson.append(bsoncxx::builder::basic::kvp("AskPrice1", T(20570.000000)));
    //tmjson.append(bsoncxx::builder::basic::kvp("AskVolume1", T(5)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice2", T(0)));	


    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume2", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice3", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume3", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice2", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume2", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice3", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume3", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice4", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume4", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidPrice5", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("BidVolume5", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice4", T(0)));	


    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume4", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskPrice5", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskVolume5", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID", T("zn1709")));
    tmjson.append(bsoncxx::builder::basic::kvp("UpdateTime", T("13:44:09")));
    tmjson.append(bsoncxx::builder::basic::kvp("UpdateMillisec", T(500)));
    tmjson.append(bsoncxx::builder::basic::kvp("ActionDay", T("20170613")));
    tmjson.append(bsoncxx::builder::basic::kvp("HisHighestPrice", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("HisLowestPrice", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("LatestVolume", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("InitVolume", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("ChangeVolume", T(0)));	


    tmjson.append(bsoncxx::builder::basic::kvp("BidImplyVolume", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AskImplyVolume", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("AvgPrice", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("ArbiType", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID_1", T("")));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentID_2", T("")));
    tmjson.append(bsoncxx::builder::basic::kvp("InstrumentName", T("")));
    tmjson.append(bsoncxx::builder::basic::kvp("TotalBidVolume", T(0)));
    tmjson.append(bsoncxx::builder::basic::kvp("TotalAskVolume", T(0)));


     bsoncxx::builder::basic::document tmpjson;
    tmpjson.append(bsoncxx::builder::basic::kvp("market", T("FEMAS")));		  
    tmpjson.append(bsoncxx::builder::basic::kvp("insertTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));		
    tmpjson.append(bsoncxx::builder::basic::kvp("sendingTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmpjson.append(bsoncxx::builder::basic::kvp("sendingTimeStr", T(fh::core::assist::utility::Current_time_str())));	
    tmpjson.append(bsoncxx::builder::basic::kvp("receivedTime", T(std::to_string(fh::core::assist::utility::Current_time_ns()))));	
    tmpjson.append(bsoncxx::builder::basic::kvp("InstrumentID", T("zn1709")));	
    tmpjson.append(bsoncxx::builder::basic::kvp("VolumeMultiple", T(1)));	
    tmpjson.append(bsoncxx::builder::basic::kvp("message", tmjson));
//	
     std::string message = bsoncxx::to_json(tmpjson.view());
     MessMap tmpmap = pFemasBookConvert->Apply_message(message);
     if(tmpmap.count("l2") != 0)
     {
         LOG_INFO("wwwwwwwwwwwwww = ",tmpmap["l2"]);
     }
     if(tmpmap.count("bid") != 0)
     {
         LOG_INFO("qqqqqqqqqqqqqqqqq = ",tmpmap["bid"]);
     }	 
     if(tmpmap.count("bbo") != 0)
     {
         LOG_INFO("zzzzzzzzzzzzzzzzzzz = ",tmpmap["bbo"]);
     }	
     if(tmpmap.count("offer") != 0)
     {
         LOG_INFO("ooooooooooooooooo = ",tmpmap["offer"]);
     }		 
     main_loop();
     return 0;
//========================================================================================================
	 
     //read my cfg
     std::string FileConfigstr= "femas_config.ini";
  
     LOG_INFO("FileConfigstr : ",FileConfigstr.c_str());
	     
      fh::core::assist::Settings *pFileConfig = new fh::core::assist::Settings(FileConfigstr);
      std::string save_url_f = pFileConfig->Get("zeromq.org_url");
      std::string save_url_s = pFileConfig->Get("zeromq.book_url");
      fh::femas::market::CFemasMarketApp *pFemasMarletApp = new fh::femas::market::CFemasMarketApp(save_url_f,save_url_s);
      	  
      pFemasMarletApp->SetFileConfigData(FileConfigstr);
      std::vector<std::string> Depthstruments;
      std::vector<std::string> Subminstruments;
      Depthstruments.clear();
      Subminstruments.clear();	  
      //Depthstruments.push_back(pFileConfig->Get("femas-DepthTopicID.TopicID"));
      //Subminstruments.push_back("*");	  

      Depthstruments.push_back("*");
      pFemasMarletApp->Initialize(Depthstruments);

      //pFemasMarletApp->Subscribe(Subminstruments);	   
	  
      if(!pFemasMarletApp->Start())
      {
          LOG_ERROR("FemasMarletApp start  Error!");
	   pFemasMarletApp->Stop();	 
	   delete pFemasMarletApp;
	   return 0;	  
      }

      main_loop();
	  
      pFemasMarletApp->Stop();
      delete pFileConfig;
      delete pFemasMarletApp;

      LOG_INFO("femas-market pross stop!");
      return 0;
}
