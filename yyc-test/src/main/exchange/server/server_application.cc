
#include <boost/algorithm/string.hpp>
#include <quickfix/fix42/TestRequest.h>
#include <quickfix/fix42/ExecutionReport.h>
#include "server_application.h"

void Application::onLogon( const FIX::SessionID& sessionID )
{
    std::cout << "on logon" << std::endl;

    // send test request
    auto id = FIX::TestReqID("Test-request-id-100");
    FIX42::TestRequest testRequest(id);
    FIX::Session::sendToTarget( testRequest, sessionID.getSenderCompID(), sessionID.getTargetCompID() );

//    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//    // send gap fill message
//    auto no = FIX::NewSeqNo(50);
//    auto f = FIX::GapFillFlag("Y");
//    FIX42::SequenceReset reset(no);
//    reset.set(f);
//    FIX::Session::sendToTarget( reset, sessionID.getSenderCompID(), sessionID.getTargetCompID() );
}

void Application::onLogout( const FIX::SessionID& sessionID ) {}

void Application::fromApp( const FIX::Message& message,
                           const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
  crack( message, sessionID );
  std::cout << "from app: " << boost::replace_all_copy(message.toString(), "\001", " ") << std::endl;
}

void Application::fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID)
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon )
{
    crack( message, sessionID );
    std::cout << "from admin: " << boost::replace_all_copy(message.toString(), "\001", " ") << std::endl;
}

void Application::onMessage( const FIX42::NewOrderSingle& message, const FIX::SessionID& )
{
  FIX::SenderCompID senderCompID;
  FIX::TargetCompID targetCompID;
  FIX::ClOrdID clOrdID;
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::Price price;
  FIX::OrderQty orderQty;
  FIX::TimeInForce timeInForce( FIX::TimeInForce_DAY );

  message.getHeader().get( senderCompID );
  message.getHeader().get( targetCompID );
  message.get( clOrdID );
  message.get( symbol );
  message.get( side );
  message.get( ordType );
  if ( ordType == FIX::OrdType_LIMIT )
    message.get( price );
  message.get( orderQty );
  message.getFieldIfSet( timeInForce );

  try
  {
    if ( timeInForce != FIX::TimeInForce_DAY )
      throw std::logic_error( "Unsupported TIF, use Day" );

    Order order( clOrdID, symbol, senderCompID, targetCompID,
                 convert( side ), convert( ordType ),
                 price, (long)orderQty );

    processOrder( order );
  }
  catch ( std::exception & e )
  {
    rejectOrder( senderCompID, targetCompID, clOrdID, symbol, side, e.what() );
  }
}

void Application::onMessage( const FIX42::OrderCancelRequest& message, const FIX::SessionID& )
{
  FIX::OrigClOrdID origClOrdID;
  FIX::Symbol symbol;
  FIX::Side side;

  message.get( origClOrdID );
  message.get( symbol );
  message.get( side );

  try
  {
    processCancel( origClOrdID, symbol, convert( side ) );
  }
  catch ( std::exception& ) {}
}

void Application::onMessage( const FIX42::OrderCancelReplaceRequest& message, const FIX::SessionID& )
{
    std::cout << "OrderCancelReplaceRequest: " << message << std::endl;
}

void Application::onMessage( const FIX42::OrderStatusRequest& message, const FIX::SessionID& )
{
    std::cout << "OrderStatusRequest: " << message << std::endl;
}

//void Application::onMessage( const FIX42::Logon& message, const FIX::SessionID& )
//{
//    std::cout << "Logon: " << message << std::endl;
//}

//void Application::onMessage( const FIX42::MarketDataRequest& message, const FIX::SessionID& )
//{
//  FIX::MDReqID mdReqID;
//  FIX::SubscriptionRequestType subscriptionRequestType;
//  FIX::MarketDepth marketDepth;
//  FIX::NoRelatedSym noRelatedSym;
//  FIX42::MarketDataRequest::NoRelatedSym noRelatedSymGroup;
//
//  message.get( mdReqID );
//  message.get( subscriptionRequestType );
//  if ( subscriptionRequestType != FIX::SubscriptionRequestType_SNAPSHOT )
//    throw( FIX::IncorrectTagValue( subscriptionRequestType.getField() ) );
//  message.get( marketDepth );
//  message.get( noRelatedSym );
//
//  for ( int i = 1; i <= noRelatedSym; ++i )
//  {
//    FIX::Symbol symbol;
//    message.getGroup( i, noRelatedSymGroup );
//    noRelatedSymGroup.get( symbol );
//  }
//}

//void Application::onMessage( const FIX43::MarketDataRequest& message, const FIX::SessionID& )
//{
//    std::cout << message.toXML() << std::endl;
//}

void Application::updateOrder( const Order& order, char status )
{
  FIX::TargetCompID targetCompID( order.getOwner() );
  FIX::SenderCompID senderCompID( order.getTarget() );

  FIX42::ExecutionReport fixOrder
  ( FIX::OrderID ( order.getClientID() ),
    FIX::ExecID ( m_generator.genExecutionID() ),
    FIX::ExecTransType ( FIX::ExecTransType_NEW ),
    FIX::ExecType ( status ),
    FIX::OrdStatus ( status ),
    FIX::Symbol ( order.getSymbol() ),
    FIX::Side ( convert( order.getSide() ) ),
    FIX::LeavesQty ( order.getOpenQuantity() ),
    FIX::CumQty ( order.getExecutedQuantity() ),
    FIX::AvgPx ( order.getAvgExecutedPrice() ) );

  fixOrder.set( FIX::ClOrdID( order.getClientID() ) );
  fixOrder.set( FIX::OrderQty( order.getQuantity() ) );

  if ( status == FIX::OrdStatus_FILLED ||
       status == FIX::OrdStatus_PARTIALLY_FILLED )
  {
    fixOrder.set( FIX::LastShares( order.getLastExecutedQuantity() ) );
    fixOrder.set( FIX::LastPx( order.getLastExecutedPrice() ) );
  }

  try
  {
    FIX::Session::sendToTarget( fixOrder, senderCompID, targetCompID );
  }
  catch ( FIX::SessionNotFound& ) {}}

void Application::rejectOrder
( const FIX::SenderCompID& sender, const FIX::TargetCompID& target,
  const FIX::ClOrdID& clOrdID, const FIX::Symbol& symbol,
  const FIX::Side& side, const std::string& message )
{
  FIX::TargetCompID targetCompID( sender.getValue() );
  FIX::SenderCompID senderCompID( target.getValue() );

  FIX42::ExecutionReport fixOrder
  ( FIX::OrderID ( clOrdID.getValue() ),
    FIX::ExecID ( m_generator.genExecutionID() ),
    FIX::ExecTransType ( FIX::ExecTransType_NEW ),
    FIX::ExecType ( FIX::ExecType_REJECTED ),
    FIX::OrdStatus ( FIX::ExecType_REJECTED ),
    symbol, side, FIX::LeavesQty( 0 ), FIX::CumQty( 0 ), FIX::AvgPx( 0 ) );

  fixOrder.set( clOrdID );
  fixOrder.set( FIX::Text( message ) );

  try
  {
    FIX::Session::sendToTarget( fixOrder, senderCompID, targetCompID );
  }
  catch ( FIX::SessionNotFound& ) {}}

void Application::processOrder( const Order& order )
{
  if ( m_orderMatcher.insert( order ) )
  {
    acceptOrder( order );

    std::queue < Order > orders;
    m_orderMatcher.match( order.getSymbol(), orders );

    while ( orders.size() )
    {
      fillOrder( orders.front() );
      orders.pop();
    }
  }
  else
    rejectOrder( order );
}

void Application::processCancel( const std::string& id,
                                 const std::string& symbol, Order::Side side )
{
  Order & order = m_orderMatcher.find( symbol, side, id );
  order.cancel();
  cancelOrder( order );
  m_orderMatcher.erase( order );
}

Order::Side Application::convert( const FIX::Side& side )
{
  switch ( side )
  {
    case FIX::Side_BUY: return Order::buy;
    case FIX::Side_SELL: return Order::sell;
    default: throw std::logic_error( "Unsupported Side, use buy or sell" );
  }
}

Order::Type Application::convert( const FIX::OrdType& ordType )
{
  switch ( ordType )
  {
    case FIX::OrdType_LIMIT: return Order::limit;
    default: throw std::logic_error( "Unsupported Order Type, use limit" );
  }
}

FIX::Side Application::convert( Order::Side side )
{
  switch ( side )
  {
    case Order::buy: return FIX::Side( FIX::Side_BUY );
    case Order::sell: return FIX::Side( FIX::Side_SELL );
    default: throw std::logic_error( "Unsupported Side, use buy or sell" );
  }
}

FIX::OrdType Application::convert( Order::Type type )
{
  switch ( type )
  {
    case Order::limit: return FIX::OrdType( FIX::OrdType_LIMIT );
    default: throw std::logic_error( "Unsupported Order Type, use limit" );
  }
}
