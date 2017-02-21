
#include "logger.h"
#include "message_parser_f.h"

namespace rczg
{

	MessageParserF::MessageParserF()
	{
		// noop
	}

	MessageParserF::~MessageParserF()
	{
		// noop
	}

	void MessageParserF::Parse(const rczg::MdpMessage &message, std::vector<Book> &books)
	{
		// TODO  SecurityStatus30
		LOG_DEBUG("message type is f, TODO");
	}

}
