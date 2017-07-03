#ifndef __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_D_H__
#define __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_D_H__

#include "core/global.h"
#include "cme/market/message/instrument.h"
#include "cme/market/message/mdp_message.h"
#include "cme/market/message/mktdata.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace message
{
    class MessageParserD
    {
        public:
            MessageParserD();
            virtual ~MessageParserD();

        public:
            void Parse(const MdpMessage &message, std::vector<Instrument> &instruments);

        private:
            void Parse_27(mktdata::MDInstrumentDefinitionFuture27 *message, std::vector<Instrument> &instruments);
            void Parse_29(mktdata::MDInstrumentDefinitionSpread29 *message, std::vector<Instrument> &instruments);
            void Parse_41(mktdata::MDInstrumentDefinitionOption41 *message, std::vector<Instrument> &instruments);
            template <typename T> Instrument Parse_X(T *message);

        private:
            DISALLOW_COPY_AND_ASSIGN(MessageParserD);
    };
} // namespace message
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MESSAGE_MESSAGE_PARSER_D_H__
