#ifndef __FH_REM_CRemEfhMarket_H__
#define __FH_REM_CRemEfhMarket_H__

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "core/market/marketi.h"
#include "core/assist/settings.h"
#pragma once
#include <vector>
#include "rem_efhmarket_manager.h"

using std::vector;

namespace fh
{
namespace rem
{
namespace efhmarket
{



class CRemEfhMarket : public fh::core::market::MarketI
{
       public:
             CRemEfhMarket(fh::core::market::MarketListenerI *listener);
            virtual ~CRemEfhMarket();

        public:
            // implement of MarketI
            virtual bool Start();
            // implement of MarketI
            virtual void Initialize(std::vector<std::string> insts);
            // implement of MarketI
            virtual void Stop();
            // implement of MarketI
            virtual void Subscribe(std::vector<std::string> instruments);
            // implement of MarketI
            virtual void UnSubscribe(std::vector<std::string> instruments);
            // implement of MarketI
            virtual void ReqDefinitions(std::vector<std::string> instruments);   

	     void SetFileConfigData(std::string &FileConfig);
		 
	     	 
		 
        public:
	     fh::core::assist::Settings *m_pFileConfig;	
             
	     CRemEfhMarkrtManager* pRemEfhMarkrtManager;		
    
	     int m_itimeout;
	     std::vector<std::string> m_insts;		 
		 
};






}
}
}

#endif