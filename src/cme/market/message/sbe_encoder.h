
#ifndef __FH_CME_MARKET_MESSAGE_SBE_ENCODER_H__
#define __FH_CME_MARKET_MESSAGE_SBE_ENCODER_H__

#include "core/global.h"
#include "cme/market/message/mktdata.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{
    class SBEEncoder
    {
        public:
            SBEEncoder();
            virtual ~SBEEncoder();

        public:
            void Start_encode(std::uint16_t templateId);
            std::pair<char*, std::size_t> Encoded_buffer();
            std::string Encoded_hex_str() const;

        private:
            template <typename MessageType> 
            std::size_t Encode_header(mktdata::MessageHeader &header);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshVolume37 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::SecurityStatus30 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshBook32 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDInstrumentDefinitionFuture27 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::SnapshotFullRefresh38 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::AdminLogin15 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::AdminLogout16 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::ChannelReset4 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshTrade36 &message);
			std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshDailyStatistics33 &message);
			std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshLimitsBanding34 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshSessionStatistics35 &message);
			std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshTradeSummary42 &message);
			std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshOrderBook43 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::SnapshotFullRefreshOrderBook44 &message);
			std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDInstrumentDefinitionOption41 &message);
			std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDInstrumentDefinitionSpread29 &message);			
			
        private:
            std::size_t m_encoded_length;
            char m_buffer[BUFFER_MAX_LENGTH];

        private:
            DISALLOW_COPY_AND_ASSIGN(SBEEncoder);
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_SBE_ENCODER_H__
