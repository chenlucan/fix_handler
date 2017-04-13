
#ifndef __FH_CME_MARKET_MESSAGE_SBE_TO_JSON_H__
#define __FH_CME_MARKET_MESSAGE_SBE_TO_JSON_H__

#include <memory>
#include <boost/property_tree/ptree.hpp>
#include "core/global.h"
#include "cme/market/message/mktdata.h"
#include "cme/market/message/mdp_message.h"


#define JSON_OBJ_TYPE boost::property_tree::ptree

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{
    class SBEtoJSON
    {
        public:
            explicit SBEtoJSON(const MdpMessage *sbe_message);
            virtual ~SBEtoJSON();

        public:
            std::string To_json();

        private:
            std::string To_json(mktdata::ChannelReset4 *m);
            std::string To_json(mktdata::MDInstrumentDefinitionFuture27 *m);
            std::string To_json(mktdata::MDInstrumentDefinitionSpread29 *m);
            std::string To_json(mktdata::SecurityStatus30 *m);
            std::string To_json(mktdata::MDIncrementalRefreshBook32 *m);
            std::string To_json(mktdata::MDIncrementalRefreshDailyStatistics33 *m);
            std::string To_json(mktdata::MDIncrementalRefreshLimitsBanding34 *m);
            std::string To_json(mktdata::MDIncrementalRefreshSessionStatistics35 *m);
            std::string To_json(mktdata::MDIncrementalRefreshTrade36 *m);
            std::string To_json(mktdata::MDIncrementalRefreshVolume37 *m);
            std::string To_json(mktdata::SnapshotFullRefresh38 *m);
            std::string To_json(mktdata::QuoteRequest39 *m);
            std::string To_json(mktdata::MDInstrumentDefinitionOption41 *m);
            std::string To_json(mktdata::MDIncrementalRefreshTradeSummary42 *m);
            std::string To_json(mktdata::MDIncrementalRefreshOrderBook43 *m);
            std::string To_json(mktdata::SnapshotFullRefreshOrderBook44 *m);
            std::string To_string(const JSON_OBJ_TYPE &json_message);

        private:
            const MdpMessage *m_sbe_message;

        private:
            DISALLOW_COPY_AND_ASSIGN(SBEtoJSON);
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_SBE_TO_JSON_H__
