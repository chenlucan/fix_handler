
#ifndef __FH_TMALPHA_REPLAY_REPLAY_DATA_CONSUMER_H__
#define __FH_TMALPHA_REPLAY_REPLAY_DATA_CONSUMER_H__

#include <unordered_map>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types.hpp>
#include "core/global.h"
#include "core/assist/logger.h"
#include "tmalpha/replay/replay_listener.h"

#define  JSON_ELEMENT bsoncxx::document::element
#define  GET_SUB_FROM_JSON(view, key) view[key]
#define  GET_STR_FROM_JSON(view, key) view[key].get_utf8().value.to_string()
#define  GET_ARR_FROM_JSON(view, key) view[key].get_array().value
#define  GET_INT_FROM_JSON(view, key) std::stol(GET_STR_FROM_JSON(view, key))
#define  GET_DOUBLE_FROM_JSON(view, key) std::stod(GET_STR_FROM_JSON(view, key))

namespace fh
{
namespace tmalpha
{
namespace replay
{
    class ReplayDataConsumer
    {
        public:
            ReplayDataConsumer() : m_listener(new ReplayListener())
            {
                // noop
            }

            virtual ~ReplayDataConsumer()
            {
                delete m_listener;
            }

        public:
            // L2 行情数据发生变化时通过这个 callback 通知外部（L2 行情，bid 成交数量，ask成交数量）：交易时需要使用
            void Add_l2_changed_callback(std::function<void(const pb::dms::L2 &, std::uint32_t, std::uint32_t)> on_l2_changed)
            {
                m_listener->Add_l2_changed_callback(on_l2_changed);
            }

            // 行情数据通过这个通知外部
            void Add_market_listener(fh::core::market::MarketListenerI *listener)
            {
                m_listener->Add_market_listener(listener);
            }

            void Consume(const std::string &message)
            {
                auto doc = bsoncxx::from_json(message);
                auto json = doc.view();
                std::string type = GET_STR_FROM_JSON(json, "type");    // Trade,BBO,Bid,Offer,L2,Contract
                std::string is = GET_STR_FROM_JSON(json, "insertTime");
                int volume_multiple = GET_INT_FROM_JSON(json, "VolumeMultiple");    // 合约数量乘数
                auto body = GET_SUB_FROM_JSON(json, "message");

                LOG_INFO("apply message: ", is, " type=", type);

                if(type == "trade")   Parse_trade(body);
                else if(type == "bbo")   Parse_bbo(body);
                else if(type == "bid")   Parse_bid(body);
                else if(type == "offer")   Parse_offer(body);
                else if(type == "l2")   Parse_l2(body, volume_multiple);
                else if(type == "contract")   Parse_contract(body);
                else if(type == "turnover")   Parse_turnover(body);
                else LOG_WARN("ignore type:", type);
            }

        private:
            void Parse_trade(const JSON_ELEMENT &body)
            {
                std::string contract = GET_STR_FROM_JSON(body, "contract");
                auto size_dp = GET_SUB_FROM_JSON(body, "last");
                double price = GET_DOUBLE_FROM_JSON(size_dp, "price");
                std::uint64_t size = GET_INT_FROM_JSON(size_dp, "size");
	         std::string time = GET_STR_FROM_JSON(body, "time");			

                pb::dms::Trade trade;
                trade.set_contract(contract);
                pb::dms::DataPoint *last = trade.mutable_last();
                last->set_price(price);
                last->set_size(size);
                trade.set_time(time);
                m_listener->OnTrade(trade);
            }

            void Parse_bbo(const JSON_ELEMENT &body)
            {
                std::string contract = GET_STR_FROM_JSON(body, "contract");
                auto bid_dp = GET_SUB_FROM_JSON(body, "bid");
                double bid_price = GET_DOUBLE_FROM_JSON(bid_dp, "price");
                std::uint64_t bid_size = GET_INT_FROM_JSON(bid_dp, "size");
                auto offer_dp = GET_SUB_FROM_JSON(body, "offer");
                double offer_price = GET_DOUBLE_FROM_JSON(offer_dp, "price");
                std::uint64_t offer_size = GET_INT_FROM_JSON(offer_dp, "size");

                pb::dms::BBO bbo;
                bbo.set_contract(contract);
                pb::dms::DataPoint *bid = bbo.mutable_bid();
                bid->set_price(bid_price);
                bid->set_size(bid_size);
                pb::dms::DataPoint *ask = bbo.mutable_offer();
                ask->set_price(offer_price);
                ask->set_size(offer_size);

                m_listener->OnBBO(bbo);
            }

            void Parse_bid(const JSON_ELEMENT &body)
            {
                std::string contract = GET_STR_FROM_JSON(body, "contract");
                auto bid_dp = GET_SUB_FROM_JSON(body, "bid");
                double bid_price = GET_DOUBLE_FROM_JSON(bid_dp, "price");
                std::uint64_t bid_size = GET_INT_FROM_JSON(bid_dp, "size");

                pb::dms::Bid bid;
                bid.set_contract(contract);
                pb::dms::DataPoint *b = bid.mutable_bid();
                b->set_price(bid_price);
                b->set_size(bid_size);

                m_listener->OnBid(bid);
            }

            void Parse_offer(const JSON_ELEMENT &body)
            {
                std::string contract = GET_STR_FROM_JSON(body, "contract");
                auto offer_dp = GET_SUB_FROM_JSON(body, "offer");
                double offer_price = GET_DOUBLE_FROM_JSON(offer_dp, "price");
                std::uint64_t offer_size = GET_INT_FROM_JSON(offer_dp, "size");

                pb::dms::Offer offer_info;
                offer_info.set_contract(contract);
                pb::dms::DataPoint *offer = offer_info.mutable_offer();
                offer->set_price(offer_price);
                offer->set_size(offer_size);

                m_listener->OnOffer(offer_info);
            }

            void Parse_l2(const JSON_ELEMENT &body, int volume_multiple)
            {
                pb::dms::L2 l2_info;
                l2_info.set_contract(GET_STR_FROM_JSON(body, "contract"));

                for(auto &bid : GET_ARR_FROM_JSON(body, "bid"))
                {
                    pb::dms::DataPoint *bid_dp = l2_info.add_bid();
                    bid_dp->set_price(GET_DOUBLE_FROM_JSON(bid, "price"));
                    bid_dp->set_size(GET_INT_FROM_JSON(bid, "size"));
                }

                for(auto &offer : GET_ARR_FROM_JSON(body, "offer"))
                {
                    pb::dms::DataPoint *offer_dp = l2_info.add_offer();
                    offer_dp->set_price(GET_DOUBLE_FROM_JSON(offer, "price"));
                    offer_dp->set_size(GET_INT_FROM_JSON(offer, "size"));
                }
                std::string time = GET_STR_FROM_JSON(body, "time");	
		  l2_info.set_time(time); 		
                m_listener->OnL2(l2_info, volume_multiple);
            }

            void Parse_contract(const JSON_ELEMENT &body)
            {
                std::string name = GET_STR_FROM_JSON(body, "name");
                std::string tick_size = GET_STR_FROM_JSON(body, "tick_size");
                std::string tick_value = GET_STR_FROM_JSON(body, "tick_value");
                std::string yesterday_close_price = GET_STR_FROM_JSON(body, "yesterday_close_price");
                std::string upper_limit = GET_STR_FROM_JSON(body, "upper_limit");
                std::string lower_limit = GET_STR_FROM_JSON(body, "lower_limit");

                pb::dms::Contract contract;
                contract.set_name(name);
                contract.set_tick_size(tick_size);
                contract.set_tick_value(tick_value);
                contract.set_yesterday_close_price(yesterday_close_price);
                contract.set_upper_limit(upper_limit);
                contract.set_lower_limit(lower_limit);
                contract.set_contract_type(pb::dms::ContractType::CT_Futures);      // 固定：期货
                // contract.set_lega(); // 暂不使用
                // contract.set_legb(); // 暂不使用

                m_listener->OnContractDefinition(contract);
            }

            void Parse_turnover(const JSON_ELEMENT &body)
            {
                pb::dms::Turnover turnover;
                turnover.set_contract(GET_STR_FROM_JSON(body, "contract"));
                turnover.set_total_volume(GET_INT_FROM_JSON(body, "total_volume"));
                turnover.set_turnover(GET_DOUBLE_FROM_JSON(body, "turnover"));

                m_listener->OnTurnover(turnover);
            }

        private:
            ReplayListener *m_listener;


        private:
            DISALLOW_COPY_AND_ASSIGN(ReplayDataConsumer);
    };
}   // namespace replay
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_REPLAY_REPLAY_DATA_CONSUMER_H__
