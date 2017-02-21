
#include "logger.h"
#include "message_parser_w.h"

namespace rczg
{

	MessageParserW::MessageParserW()
	{
		// noop
	}

	MessageParserW::~MessageParserW()
	{
		// noop
	}

	void MessageParserW::Parse(const rczg::MdpMessage &message, std::vector<Book> &books)
	{
		std::uint16_t template_id = message.template_id();
		void *body = message.message_body().get();

		LOG_DEBUG("parse recovery message to books");

		switch(template_id)
		{
			case 38:		// SnapshotFullRefresh38
				this->Parse_38((mktdata::SnapshotFullRefresh38 *)body, books);
				break;
			case 44:	// SnapshotFullRefreshOrderBook44
				this->Parse_44((mktdata::SnapshotFullRefreshOrderBook44 *)body, books);
				break;
			default:
				break;
		}

		std::for_each(books.begin(), books.end(), [&message, template_id](Book &b){
			b.template_id = template_id;
			b.type = 'W';
			LOG_DEBUG("  LastMsgSeqNumProcessed=", b.packet_seq_num);
		});

		LOG_DEBUG("message type is W, template id is ", template_id, " done.");
	}

	void MessageParserW::Parse_38(mktdata::SnapshotFullRefresh38 *message, std::vector<Book> &books)
	{
	    mktdata::SnapshotFullRefresh38::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	        b.mDEntryPx = noMDEntries.mDEntryPx().mantissa();
	        b.mDEntrySize = noMDEntries.mDEntrySize();
	        b.numberOfOrders = noMDEntries.numberOfOrders();
	        b.mDPriceLevel = noMDEntries.mDPriceLevel();
	        b.tradingReferenceDate = noMDEntries.tradingReferenceDate();
	        b.openCloseSettlFlag = mktdata::OpenCloseSettlFlag::get(noMDEntries.openCloseSettlFlag());

	        mktdata::SettlPriceType st = noMDEntries.settlPriceType();
	        b.settlPriceType.isFinal = st.finalrc();
	        b.settlPriceType.isActual = st.actual();
	        b.settlPriceType.isRounded = st.rounded();
	        b.settlPriceType.isIntraday = st.intraday();
	        b.settlPriceType.isReservedBits = st.reservedBits();
	        b.settlPriceType.isNullValue = st.nullValue();

	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType());

	        books.push_back(std::move(b));
	    }

	    std::uint32_t lastMsgSeqNumProcessed = message->lastMsgSeqNumProcessed();
	    std::uint32_t securityID = message->securityID();
	    std::uint32_t rptSeq = message->rptSeq();
	    std::uint64_t highLimitPrice = message->highLimitPrice().mantissa();
	    std::uint64_t lowLimitPrice = message->lowLimitPrice().mantissa();
	    std::uint64_t maxPriceVariation = message->maxPriceVariation().mantissa();

	    std::for_each(books.begin(), books.end(), [&](Book &b){
	    	b.packet_seq_num = lastMsgSeqNumProcessed;
	    	b.securityID = securityID;
	    	b.rptSeq = rptSeq;
	    	b.highLimitPrice = highLimitPrice;
	    	b.lowLimitPrice = lowLimitPrice;
	    	b.maxPriceVariation = maxPriceVariation;
	    });
	}

	void MessageParserW::Parse_44(mktdata::SnapshotFullRefreshOrderBook44 *message, std::vector<Book> &books)
	{
	    mktdata::SnapshotFullRefreshOrderBook44::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	    	b.orderID = noMDEntries.orderID();
	    	b.mDOrderPriority = noMDEntries.mDOrderPriority();
	        b.mDEntryPx = noMDEntries.mDEntryPx().mantissa();
	        b.mDDisplayQty = noMDEntries.mDDisplayQty();
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType());
	        books.push_back(std::move(b));
	    }

	    std::uint32_t lastMsgSeqNumProcessed = message->lastMsgSeqNumProcessed();
	    std::uint32_t securityID = message->securityID();

	    std::for_each(books.begin(), books.end(), [&](Book &b){
	    	b.packet_seq_num = lastMsgSeqNumProcessed;
	    	b.securityID = securityID;
	    });
	}

}
