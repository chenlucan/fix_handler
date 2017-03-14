
#ifndef ORDERMATCH_APPLICATION_H
#define ORDERMATCH_APPLICATION_H

#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>
#include <quickfix/fix42/OrderStatusRequest.h>
#include "server_id_generator.h"
#include "server_order_matcher.h"
#include "server_order.h"

class Application
      : public FIX::Application,
        public FIX::MessageCracker
{
  // Application overloads
  void onCreate( const FIX::SessionID& ) {}
  void onLogon( const FIX::SessionID& sessionID );
  void onLogout( const FIX::SessionID& sessionID );
  void toAdmin( FIX::Message&, const FIX::SessionID& ) {}
  void toApp( FIX::Message&, const FIX::SessionID& )
  throw( FIX::DoNotSend ) {}
  void fromAdmin( const FIX::Message&, const FIX::SessionID& )
  throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon );
  void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
  throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );

  // MessageCracker overloads
  void onMessage( const FIX42::NewOrderSingle&, const FIX::SessionID& );
  void onMessage( const FIX42::OrderCancelRequest&, const FIX::SessionID& );
  void onMessage( const FIX42::OrderCancelReplaceRequest&, const FIX::SessionID& );
  void onMessage( const FIX42::OrderStatusRequest&, const FIX::SessionID& );
  //void onMessage( const FIX42::Logon& message, const FIX::SessionID& );
  //void onMessage( const FIX42::MarketDataRequest&, const FIX::SessionID& );
  //void onMessage( const FIX43::MarketDataRequest&, const FIX::SessionID& );

  // Order functionality
  void processOrder( const Order& );
  void processCancel( const std::string& id, const std::string& symbol, Order::Side );

  void updateOrder( const Order&, char status );
  void rejectOrder( const Order& order )
  { updateOrder( order, FIX::OrdStatus_REJECTED ); }
  void acceptOrder( const Order& order )
  { updateOrder( order, FIX::OrdStatus_NEW ); }
  void fillOrder( const Order& order )
  {
    updateOrder( order,
                 order.isFilled() ? FIX::OrdStatus_FILLED
                 : FIX::OrdStatus_PARTIALLY_FILLED );
  }
  void cancelOrder( const Order& order )
  { updateOrder( order, FIX::OrdStatus_CANCELED ); }

  void rejectOrder( const FIX::SenderCompID&, const FIX::TargetCompID&,
                    const FIX::ClOrdID& clOrdID, const FIX::Symbol& symbol,
                    const FIX::Side& side, const std::string& message );

  // Type conversions
  Order::Side convert( const FIX::Side& );
  Order::Type convert( const FIX::OrdType& );
  FIX::Side convert( Order::Side );
  FIX::OrdType convert( Order::Type );

  OrderMatcher m_orderMatcher;
  IDGenerator m_generator;

public:
  const OrderMatcher& orderMatcher() { return m_orderMatcher; }
};

#endif
