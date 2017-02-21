#ifndef __MESSAGE_PARSER_D_H__
#define __MESSAGE_PARSER_D_H__

#include "global.h"
#include "instrument.h"
#include "mdp_message.h"

namespace rczg
{
	class MessageParserD
	{
		public:
			MessageParserD();
			virtual ~MessageParserD();

		public:
			void Parse(const rczg::MdpMessage &message, std::vector<Instrument> &instruments);

		private:
			void Parse_27(mktdata::MDInstrumentDefinitionFuture27 *message, std::vector<Instrument> &instruments);
			void Parse_29(mktdata::MDInstrumentDefinitionSpread29 *message, std::vector<Instrument> &instruments);
			void Parse_41(mktdata::MDInstrumentDefinitionOption41 *message, std::vector<Instrument> &instruments);
			template <typename T> Instrument Parse_X(T *message);

		private:
			DISALLOW_COPY_AND_ASSIGN(MessageParserD);
	};
}

#endif // __MESSAGE_PARSER_D_H__
