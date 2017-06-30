#ifndef __FH_CORE_AUTOTEST_BOOK_BOOK_SENDER_H__
#define __FH_CORE_AUTOTEST_BOOK_BOOK_SENDER_H__

#include <unordered_map>
#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/typeof/typeof.hpp>  
#include <boost/foreach.hpp>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>

#include "core/global.h"
#include "core/market/marketlisteneri.h"

#include "../../core/assist/mut_common.h"

namespace fh
{
namespace core
{
namespace book
{    
    class AutoTestBookSender : public fh::core::market::MarketListenerI
    {
        public:
            AutoTestBookSender();
            virtual ~AutoTestBookSender();

        public:
            // implement of MarketListenerI
            void OnMarketDisconnect(core::market::MarketI* market) override;
            // implement of MarketListenerI
            void OnMarketReconnect(core::market::MarketI* market) override;
            // implement of MarketListenerI
            void OnContractDefinition(const pb::dms::Contract &contract) override;
            // implement of MarketListenerI
            void OnBBO(const pb::dms::BBO &bbo) override;
            // implement of MarketListenerI
            void OnBid(const pb::dms::Bid &bid) override;
            // implement of MarketListenerI
            void OnOffer(const pb::dms::Offer &offer) override;
            // implement of MarketListenerI
            void OnL2(const pb::dms::L2 &l2) override;
            // implement of MarketListenerI
            void OnL3() override;
            // implement of MarketListenerI
            void OnTrade(const pb::dms::Trade &trade) override;
            // implement of MarketListenerI
            void OnContractAuctioning(const std::string &contract) override;
            // implement of MarketListenerI
            void OnContractNoTrading(const std::string &contract)  override;
            // implement of MarketListenerI
            void OnContractTrading(const std::string &contract)    override;
            // implement of MarketListenerI
            void OnOrginalMessage(const std::string &message) override;
            // implement of MarketListenerI
            void OnTurnover(const pb::dms::Turnover &turnover) override;

        private:
            int m_current_caseid;
            std::string m_sendL2;  // save the value of the last L2
            std::unordered_map<std::string, std::string> m_L2ValueMap; // save the value of the last L2            
            std::map<std::string, fh::core::assist::common::DefineMsg_Compare> m_DefValueMap; // save the value of the last Define message
        public:            
            void SetCaseId(const int &caseId);
            int  GetCaseId();
            
            void CheckResult(const std::string &contract);

        private:
            DISALLOW_COPY_AND_ASSIGN(AutoTestBookSender);
    };
} // namespace book
} // namespace core
} // namespace fh

#endif // __FH_CORE_AUTOTEST_BOOK_BOOK_SENDER_H__
