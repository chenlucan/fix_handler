#ifndef __FH_FEMAS_MARKET_REPLAYER_FEMAS_BOOK_CONVERT_H__
#define __FH_FEMAS_MARKET_REPLAYER_FEMAS_BOOK_CONVERT_H__

#include <vector>
#include <unordered_map>
#include <bsoncxx/document/view.hpp>
#include "core/global.h"
#include "core/market/marketlisteneri.h"
#include "USTPFtdcMduserApi.h"
#include "femas/market/femas_book_manager.h"


#define JSON_ELEMENT bsoncxx::document::element


namespace fh
{
namespace femas
{
namespace market
{
namespace convert
{
    class FemasConvertListenerI : public fh::core::market::MarketListenerI 
    {
        public:
            FemasConvertListenerI();
            virtual ~FemasConvertListenerI();

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
            virtual void OnOrginalMessage(const std::string &message);

        private:
            //fh::core::zmq::ZmqSender m_org_sender;
            //fh::core::zmq::ZmqSender m_book_sender;

        private:
            DISALLOW_COPY_AND_ASSIGN(FemasConvertListenerI);
			
    };

    class FemasBookConvert
    {
        public:
            FemasBookConvert();
            virtual ~FemasBookConvert();
        public:	
	     void Add_listener(fh::core::market::MarketListenerI *listener);
	     void Apply_message(const std::string &message);
	     void FemasmarketData(const JSON_ELEMENT &message,int volumeMultiple=0);	
	 private:
	     fh::core::market::MarketListenerI *m_listener;
	     fh::femas::market::CFemasBookManager *m_femas_book_manager;	 
    };


} // namespace convert
} // namespace market
} // namespace femas
} // namespace fh

#endif // __FH_FEMAS_MARKET_REPLAYER_FEMAS_BOOK_CONVERT_H__