
#ifndef __FH_TMALPHA_MARKET_CME_DATA_CONSUMER_H__
#define __FH_TMALPHA_MARKET_CME_DATA_CONSUMER_H__

#include <unordered_map>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include "core/global.h"
#include "core/assist/logger.h"
#include "cme/market/playback/book_replayer.h"
#include "cme/market/message/message_utility.h"
#include "tmalpha/market/data_consumer.h"


namespace fh
{
namespace tmalpha
{
namespace market
{
    class CmeDataConsumer : public DataConsumer
    {
        public:
            CmeDataConsumer() : m_replayer()
            {
                // noop
            }

            virtual ~CmeDataConsumer()
            {
                // noop
            }

        public:
            void Consume(const std::string &message) override
            {
                m_replayer.Apply_message(message);
            }

            std::unordered_map<std::string , pb::dms::L2> Get_state() override
            {
                std::unordered_map<std::string , pb::dms::L2> result;
                std::unordered_map<std::uint32_t , fh::cme::market::BookState> &states = m_replayer.Get_all_states();
                for(const auto &s : states)
                {
                    pb::dms::L2 l2 = CmeDataConsumer::State_to_L2(s.second);
                    result[l2.contract()] = l2;
                }
                return result;
            }

        private:
            static pb::dms::L2 State_to_L2(const fh::cme::market::BookState &state)
            {
                pb::dms::L2 l2_info;

                l2_info.set_contract(state.symbol);
                for(const auto &b : state.bid)
                {
                    pb::dms::DataPoint *bid = l2_info.add_bid();
                    bid->set_price(fh::cme::market::message::utility::Get_price(b.mDEntryPx));
                    bid->set_size(b.mDEntrySize);
                }
                for(const auto &a : state.ask)
                {
                    pb::dms::DataPoint *ask = l2_info.add_offer();
                    ask->set_price(fh::cme::market::message::utility::Get_price(a.mDEntryPx));
                    ask->set_size(a.mDEntrySize);
                }

                return l2_info;
            }

        private:
            fh::cme::market::playback::BookReplayer m_replayer;

        private:
            DISALLOW_COPY_AND_ASSIGN(CmeDataConsumer);
    };
}   // namespace market
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_MARKET_CME_DATA_CONSUMER_H__
