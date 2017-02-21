
#include "logger.h"
#include "message_parser_x.h"

namespace rczg
{

	MessageParserX::MessageParserX()
	{
		// noop
	}

	MessageParserX::~MessageParserX()
	{
		// noop
	}

	void MessageParserX::Parse(const rczg::MdpMessage &message, std::vector<Book> &books)
	{
		std::uint16_t template_id = message.template_id();
		void *body = message.message_body().get();

		LOG_DEBUG("parse increment message to books");

		switch(template_id)
		{
			case 4:		// ChannelReset4
				this->Parse_4((mktdata::ChannelReset4 *)body, books);
				break;
			case 32:	// MDIncrementalRefreshBook32
				this->Parse_32((mktdata::MDIncrementalRefreshBook32 *)body, books);
				break;
			case 33:	// MDIncrementalRefreshDailyStatistics33
				this->Parse_33((mktdata::MDIncrementalRefreshDailyStatistics33 *)body, books);
				break;
			case 34:	// MDIncrementalRefreshLimitsBanding34
				this->Parse_34((mktdata::MDIncrementalRefreshLimitsBanding34 *)body, books);
				break;
			case 35:	// MDIncrementalRefreshSessionStatistics35
				this->Parse_35((mktdata::MDIncrementalRefreshSessionStatistics35 *)body, books);
				break;
			case 36:	// MDIncrementalRefreshTrade36
				this->Parse_36((mktdata::MDIncrementalRefreshTrade36 *)body, books);
				break;
			case 37:	// MDIncrementalRefreshVolume37
				this->Parse_37((mktdata::MDIncrementalRefreshVolume37 *)body, books);
				break;
			case 42:	// MDIncrementalRefreshTradeSummary42
				this->Parse_42((mktdata::MDIncrementalRefreshTradeSummary42 *)body, books);
				break;
			case 43:	// MDIncrementalRefreshOrderBook43
				this->Parse_43((mktdata::MDIncrementalRefreshOrderBook43 *)body, books);
				break;
			default:
				break;
		}

		// set each book's packet_seq_num
		std::for_each(books.begin(), books.end(), [&message, template_id](Book &b){
			b.packet_seq_num = message.packet_seq_num();
			b.template_id = template_id;
			b.type = 'X';
		});

		LOG_DEBUG("message type is X, template id is ", template_id, " done.");
	}

	void MessageParserX::Parse_4(mktdata::ChannelReset4 *message, std::vector<Book> &books)
	{
	    mktdata::ChannelReset4::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	        b.mDUpdateAction = mktdata::MDUpdateAction::get(noMDEntries.mDUpdateAction());
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType(0));
	        b.applID = noMDEntries.applID();
	        books.push_back(std::move(b));
	    }

	    mktdata::MatchEventIndicator& matchEventIndicator = message->matchEventIndicator();
	    this->Set_match_event_indicator(matchEventIndicator, books);
	}

	void MessageParserX::Parse_32(mktdata::MDIncrementalRefreshBook32 *message, std::vector<Book> &books)
	{
	    mktdata::MDIncrementalRefreshBook32::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	        b.mDEntryPx = noMDEntries.mDEntryPx().mantissa();
	        b.mDEntrySize = noMDEntries.mDEntrySize();
	        b.securityID = noMDEntries.securityID();
	        b.rptSeq = noMDEntries.rptSeq();
	        b.numberOfOrders = noMDEntries.numberOfOrders();
	        b.mDPriceLevel = noMDEntries.mDPriceLevel();
	        b.mDUpdateAction = mktdata::MDUpdateAction::get(noMDEntries.mDUpdateAction());
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType());
	        books.push_back(std::move(b));
	    }

	    mktdata::MatchEventIndicator& matchEventIndicator = message->matchEventIndicator();
	    this->Set_match_event_indicator(matchEventIndicator, books);
	}

	void MessageParserX::Parse_33(mktdata::MDIncrementalRefreshDailyStatistics33 *message, std::vector<Book> &books)
	{
	    mktdata::MDIncrementalRefreshDailyStatistics33::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	        b.mDEntryPx = noMDEntries.mDEntryPx().mantissa();
	        b.mDEntrySize = noMDEntries.mDEntrySize();
	        b.securityID = noMDEntries.securityID();
	        b.rptSeq = noMDEntries.rptSeq();
	        b.tradingReferenceDate = noMDEntries.tradingReferenceDate();
	        b.mDUpdateAction = mktdata::MDUpdateAction::get(noMDEntries.mDUpdateAction());
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType());

	        mktdata::SettlPriceType st = noMDEntries.settlPriceType();
	        b.settlPriceType.isFinal = st.finalrc();
	        b.settlPriceType.isActual = st.actual();
	        b.settlPriceType.isRounded = st.rounded();
	        b.settlPriceType.isIntraday = st.intraday();
	        b.settlPriceType.isReservedBits = st.reservedBits();
	        b.settlPriceType.isNullValue = st.nullValue();

	        books.push_back(std::move(b));
	    }

	    mktdata::MatchEventIndicator& matchEventIndicator = message->matchEventIndicator();
	    this->Set_match_event_indicator(matchEventIndicator, books);
	}

	void MessageParserX::Parse_34(mktdata::MDIncrementalRefreshLimitsBanding34 *message, std::vector<Book> &books)
	{
	    mktdata::MDIncrementalRefreshLimitsBanding34::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	        b.highLimitPrice = noMDEntries.highLimitPrice().mantissa();
	        b.lowLimitPrice = noMDEntries.lowLimitPrice().mantissa();
	        b.maxPriceVariation = noMDEntries.maxPriceVariation().mantissa();
	        b.securityID = noMDEntries.securityID();
	        b.rptSeq = noMDEntries.rptSeq();
	        b.mDUpdateAction = mktdata::MDUpdateAction::get(noMDEntries.mDUpdateAction());
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType(0));
	        books.push_back(std::move(b));
	    }

	    mktdata::MatchEventIndicator& matchEventIndicator = message->matchEventIndicator();
	    this->Set_match_event_indicator(matchEventIndicator, books);
	}

	void MessageParserX::Parse_35(mktdata::MDIncrementalRefreshSessionStatistics35 *message, std::vector<Book> &books)
	{
	    mktdata::MDIncrementalRefreshSessionStatistics35::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	        b.mDEntryPx = noMDEntries.mDEntryPx().mantissa();
	        b.securityID = noMDEntries.securityID();
	        b.rptSeq = noMDEntries.rptSeq();
	        b.openCloseSettlFlag = mktdata::OpenCloseSettlFlag::get(noMDEntries.openCloseSettlFlag());
	        b.mDUpdateAction = mktdata::MDUpdateAction::get(noMDEntries.mDUpdateAction());
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType());
	        b.mDEntrySize = noMDEntries.mDEntrySize();
	        books.push_back(std::move(b));
	    }

	    mktdata::MatchEventIndicator& matchEventIndicator = message->matchEventIndicator();
	    this->Set_match_event_indicator(matchEventIndicator, books);
	}

	void MessageParserX::Parse_36(mktdata::MDIncrementalRefreshTrade36 *message, std::vector<Book> &books)
	{
	    mktdata::MDIncrementalRefreshTrade36::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	        b.mDEntryPx = noMDEntries.mDEntryPx().mantissa();
	        b.mDEntrySize = noMDEntries.mDEntrySize();
	        b.securityID = noMDEntries.securityID();
	        b.rptSeq = noMDEntries.rptSeq();
	        b.numberOfOrders = noMDEntries.numberOfOrders();
	        b.tradeID = noMDEntries.tradeID();
	        b.aggressorSide = mktdata::AggressorSide::get(noMDEntries.aggressorSide());
	        b.mDUpdateAction = mktdata::MDUpdateAction::get(noMDEntries.mDUpdateAction());
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType(0));
	        books.push_back(std::move(b));
	    }

	    mktdata::MatchEventIndicator& matchEventIndicator = message->matchEventIndicator();
	    this->Set_match_event_indicator(matchEventIndicator, books);
	}

	void MessageParserX::Parse_37(mktdata::MDIncrementalRefreshVolume37 *message, std::vector<Book> &books)
	{
	    mktdata::MDIncrementalRefreshVolume37::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	        b.mDEntrySize = noMDEntries.mDEntrySize();
	        b.securityID = noMDEntries.securityID();
	        b.rptSeq = noMDEntries.rptSeq();
	        b.mDUpdateAction = mktdata::MDUpdateAction::get(noMDEntries.mDUpdateAction());
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType(0));
	        books.push_back(std::move(b));
	    }

	    mktdata::MatchEventIndicator& matchEventIndicator = message->matchEventIndicator();
	    this->Set_match_event_indicator(matchEventIndicator, books);
	}

	void MessageParserX::Parse_42(mktdata::MDIncrementalRefreshTradeSummary42 *message, std::vector<Book> &books)
	{
	    mktdata::MDIncrementalRefreshTradeSummary42::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	        b.mDEntryPx = noMDEntries.mDEntryPx().mantissa();
	        b.mDEntrySize = noMDEntries.mDEntrySize();
	        b.securityID = noMDEntries.securityID();
	        b.rptSeq = noMDEntries.rptSeq();
	        b.numberOfOrders = noMDEntries.numberOfOrders();
	        b.aggressorSide = mktdata::AggressorSide::get(noMDEntries.aggressorSide());
	        b.mDUpdateAction = mktdata::MDUpdateAction::get(noMDEntries.mDUpdateAction());
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType(0));
	        b.mDTradeEntryID = noMDEntries.mDTradeEntryID();
	        books.push_back(std::move(b));
	    }

	    mktdata::MatchEventIndicator& matchEventIndicator = message->matchEventIndicator();
	    this->Set_match_event_indicator(matchEventIndicator, books);
	}

	void MessageParserX::Parse_43(mktdata::MDIncrementalRefreshOrderBook43 *message, std::vector<Book> &books)
	{
	    mktdata::MDIncrementalRefreshOrderBook43::NoMDEntries& noMDEntries = message->noMDEntries();
	    while (noMDEntries.hasNext())
	    {
	        noMDEntries.next();
	    	Book b;
	    	b.orderID = noMDEntries.orderID();
	    	b.mDOrderPriority = noMDEntries.mDOrderPriority();
	        b.mDEntryPx = noMDEntries.mDEntryPx().mantissa();
	        b.mDDisplayQty = noMDEntries.mDDisplayQty();
	        b.securityID = noMDEntries.securityID();
	        b.mDUpdateAction = mktdata::MDUpdateAction::get(noMDEntries.mDUpdateAction());
	        b.mDEntryType = mktdata::MDEntryType::get(noMDEntries.mDEntryType());
	        books.push_back(std::move(b));
	    }

	    mktdata::MatchEventIndicator& matchEventIndicator = message->matchEventIndicator();
	    this->Set_match_event_indicator(matchEventIndicator, books);
	}

	void MessageParserX::Set_match_event_indicator(const mktdata::MatchEventIndicator& matchEventIndicator, std::vector<Book> &books)
	{
		if(books.empty())
		{
			// 如果 message 中没有 noMDEntries，那么需要做一条空的 book 出来，里面放下面的 MatchEventIndicator
			books.resize(1);
		}

	    bool isLastTradeMsg = matchEventIndicator.lastTradeMsg();
		bool isLastVolumeMsg = matchEventIndicator.lastVolumeMsg();
		bool isLastQuoteMsg = matchEventIndicator.lastQuoteMsg();
		bool isLastStatsMsg = matchEventIndicator.lastStatsMsg();
		bool isLastImpliedMsg = matchEventIndicator.lastImpliedMsg();
		bool isRecoveryMsg = matchEventIndicator.recoveryMsg();
		bool isReserved = matchEventIndicator.reserved();
		bool isEndOfEvent = matchEventIndicator.endOfEvent();

		std::for_each(books.begin(), books.end(), [&](Book &b){
			b.matchEventIndicator.isLastTradeMsg = isLastTradeMsg;
			b.matchEventIndicator.isLastVolumeMsg = isLastVolumeMsg;
			b.matchEventIndicator.isLastQuoteMsg = isLastQuoteMsg;
			b.matchEventIndicator.isLastStatsMsg = isLastStatsMsg;
			b.matchEventIndicator.isLastImpliedMsg = isLastImpliedMsg;
			b.matchEventIndicator.isRecoveryMsg = isRecoveryMsg;
			b.matchEventIndicator.isReserved = isReserved;
			b.matchEventIndicator.isEndOfEvent = isEndOfEvent;
		});
	}
}

