
#include "logger.h"
#include "message_parser_r.h"

namespace rczg
{

	MessageParserR::MessageParserR()
	{
		// noop
	}

	MessageParserR::~MessageParserR()
	{
		// noop
	}

	void MessageParserR::Parse(const rczg::MdpMessage &message, std::vector<Book> &books)
	{
		// TODO  QuoteRequest39
		LOG_DEBUG("message type is R, TODO");
	}

}
