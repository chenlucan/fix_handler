//
//#include "book.h"
//
//namespace rczg
//{
//
//	Book::Book()
//	: packet_seq_num(0), matchEventIndicator(0), aggressorSide(0), applID(0), highLimitPrice(0), lowLimitPrice(0), mDDisplayQty(0),
//	  mDEntryPx(0), mDEntrySize(0), mDEntryType(0), mDOrderPriority(0), mDPriceLevel(0),
//	  mDTradeEntryID(0), mDUpdateAction(0), maxPriceVariation(0), numberOfOrders(0),
//	  openCloseSettlFlag(0), orderID(0), rptSeq(0), securityID(0), settlPriceType(0),tradeID(0),
//	  tradingReferenceDate(0)
//	{
//		// noop
//	}
//
//	Book::~Book()
//	{
//		// noop
//	}
//
//    std::string Book::Serialize() const
//    {
//    	// TODO
//    	std::ostringstream os;
//    	os << "book: mDEntryType=" << mDEntryType;
//
//    	return os.str();
//    }
//
//}
