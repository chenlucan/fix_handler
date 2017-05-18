
#include <chrono>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <quickfix/Session.h>

#include "core/assist/logger.h"

#include "cme/market/message/message_utility.h"

#include "cme/exchange/order_manager.h"


#include "mut_common.h"

namespace fh
{
namespace core
{
namespace assist
{
namespace common
{
    void getAbsolutePath(std::string &path)
    {
        char current_absolute_path[256] = {0};
        //获取当前程序绝对路径
        int cnt = readlink("/proc/self/exe", current_absolute_path, 256);
        if (cnt < 0 || cnt >= 256)
        {
            printf("***Error***\n");
            return;
        }
        //获取当前目录绝对路径，即去掉程序名
        int i;
        for (i = cnt; i >=0; --i)
        {
            if (current_absolute_path[i] == '/')
            {
                current_absolute_path[i+1] = '\0';
                break;
            }
        }
        path = current_absolute_path;
        printf("--- current absolute path:[%s] ---\n", path.c_str());
    }
    
    void show_message(const std::string &hex_message)
    {
        std::vector<std::string> tokens;
        std::istringstream iss(hex_message);
        std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

        std::vector<char> bytes;
        int index = 0;
        std::ostringstream byte_line;
        std::for_each(tokens.begin(), tokens.end(), [&bytes, &index, &byte_line](std::string &s){
            boost::trim_right(s);
            boost::trim_left(s);
            if(s != "")
            {
                char x = (char) strtol(s.c_str(), nullptr, 16);
                bytes.push_back(x);

                index ++;
                std::string dec;
                if(index % 40 == 0) dec = "\n";
                else if(index % 20 == 0) dec = "    ";
                else if(index % 10 == 0) dec = "  ";
                else dec = " ";
                byte_line << s << dec;
            }
        });

        //LOG_INFO("hex message: size=", bytes.size(), "\n", byte_line.str());

        std::vector<fh::cme::market::message::MdpMessage> message;
        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(bytes.data(), bytes.size(), message);

        LOG_INFO("seq=", seq, ", message count=", message.size());

        std::for_each(message.cbegin(), message.cend(), [](const fh::cme::market::message::MdpMessage &m){
            std::string  s = m.Serialize();
            LOG_INFO(s);
        });
    }
    
    // read mdp packets from file(one packet per line) for test
    void Read_packets(std::vector<std::string> &packets, const std::string &filename, const std::string &packet_from, const std::string &packet_start_indicate)
    {
        //std::vector<std::string> packets;
        packets.clear();
        std::ifstream input(filename);
        std::string line;
        while (std::getline(input, line))
        {
            auto pos = line.find(packet_from);
            if(pos != std::string::npos)
            {
                line.erase(0, pos + packet_from.size());
                //printf("=== [1] pos=[%d], line = [%s] ===\n", pos, line.c_str() );


                pos = line.find(packet_start_indicate);

                if(pos != std::string::npos)
                {
                    line.erase(0, pos + packet_start_indicate.size());
                    //printf("=== [2] pos=[%d], line = [%s] ===\n", pos, line.c_str() );


                    std::istringstream iss(line);
                    std::vector<std::string> tokens;
                    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                    std::vector<char> bytes;
                    int index = 0;
                    std::ostringstream byte_line;
                    std::for_each(tokens.begin(), tokens.end(), [&bytes, &index, &byte_line](std::string &s){
                        boost::trim_right(s);
                        boost::trim_left(s);
                        if(s != "")
                        {
                            bytes.push_back((char) strtol(s.c_str(), nullptr, 16));
                            index ++;
                            std::string dec;
                            if(index % 40 == 0) dec = "\n";
                            else if(index % 20 == 0) dec = "    ";
                            else if(index % 10 == 0) dec = "  ";
                            else dec = " ";
                            byte_line << s << dec;
                        }
                    });
                    packets.push_back(std::string(bytes.cbegin(), bytes.cend()));
                    LOG_DEBUG("read packet: size=", bytes.size(), "\n", byte_line.str());
                }
            }
            
        }

        return;
    }
    
    void fillHeader( FIX::Header& header, const char* sender, const char* target, int seq )
    {
        header.setField( FIX::SenderCompID( sender ) );
        header.setField( FIX::TargetCompID( target ) );
        header.setField( FIX::SendingTime() );
        header.setField( FIX::MsgSeqNum( seq ) );
    }

    FIX42::ResendRequest createResendRequest( const char* sender, const char* target, int seq, int begin, int end )
    {
        FIX42::ResendRequest resendRequest;
        resendRequest.set( FIX::BeginSeqNo( begin ) );
        resendRequest.set( FIX::EndSeqNo( end ) );
        fillHeader( resendRequest.getHeader(), sender, target, seq );
        return resendRequest;
    }

    FIX42::ExecutionReport createExecutionReport( const char* sender, const char* target, int seq )
    {
        //using namespace FIX;
        FIX42::ExecutionReport executionReport( FIX::OrderID("ID"), FIX::ExecID("ID"), FIX::ExecTransType('0'), FIX::ExecType('0'), FIX::OrdStatus('0'), FIX::Symbol("SYMBOL"), FIX::Side(FIX::Side_BUY), FIX::LeavesQty(100), FIX::CumQty(0), FIX::AvgPx(0) );
        fillHeader( executionReport.getHeader(), sender, target, seq );
        FIX42::ExecutionReport::NoContraBrokers noContraBrokers;
        noContraBrokers.set( FIX::ContraBroker("BROKER") );
        noContraBrokers.set( FIX::ContraTrader("TRADER") );
        noContraBrokers.set( FIX::ContraTradeQty(100) );
        noContraBrokers.set( FIX::ContraTradeTime() );
        executionReport.addGroup( noContraBrokers );
        noContraBrokers.set( FIX::ContraBroker("BROKER2") );
        noContraBrokers.set( FIX::ContraTrader("TRADER2") );
        noContraBrokers.set( FIX::ContraTradeQty(100) );
        noContraBrokers.set( FIX::ContraTradeTime() );
        executionReport.addGroup( noContraBrokers );
        return executionReport;
    }
    
    
    
    FIX42::Logon createLogon( const char* sender, const char* target, int seq )
    {
        FIX42::Logon logon;
        logon.set( FIX::EncryptMethod( 0 ) );
        logon.set( FIX::HeartBtInt( 30 ) );
        fillHeader( logon.getHeader(), sender, target, seq );
        return logon;
    }
    
    FIX42::Logout createLogout( const char* sender, const char* target, int seq )
    {
        FIX42::Logout logout;
        fillHeader( logout.getHeader(), sender, target, seq );
        return logout;
    }
    
    FIX42::Heartbeat createHeartbeat( const char* sender, const char* target, int seq )
    {
        FIX42::Heartbeat heartbeat;
        fillHeader( heartbeat.getHeader(), sender, target, seq );
        return heartbeat;
    }
    
    FIX42::TestRequest createTestRequest( const char* sender, const char* target, int seq, const char* id )
    {
        FIX42::TestRequest testRequest;
        testRequest.set( FIX::TestReqID( id ) );
        fillHeader( testRequest.getHeader(), sender, target, seq );
        return testRequest;
    }
    
    FIX42::SequenceReset createSequenceReset( const char* sender, const char* target, int seq, int newSeq )
    {
        FIX42::SequenceReset sequenceReset;
        sequenceReset.set( FIX::NewSeqNo( newSeq ) );
        fillHeader( sequenceReset.getHeader(), sender, target, seq );
        return sequenceReset;
    }
    
    FIX42::Message createMessage(const char* sender, const char* target, int seq, const char* type )
    {
        FIX::Message message;
        message.getHeader().setField(FIX::BeginString("FIX.4.2"));
        message.getHeader().setField(FIX::SenderCompID(sender)); //"CME"
        message.getHeader().setField(FIX::TargetCompID(target)); // "2E0004N"
        message.getHeader().setField(FIX::MsgType(type)); // FIX::MsgType_OrderMassActionReport
        message.getHeader().setField(FIX::MsgSeqNum( seq ));
        message.setField(FIX::Account("ACCOUNT"));
        message.setField(FIX::ClOrdID("CLORDID"));
        
        return message;
    }
    
    
    FIX42::OrderCancelRequest createOrderCancelRequest42(
            const std::string &cl_order_id,
            const std::string &orig_cl_order_id,
            char side,
            const std::string &symbol,
            const std::string &order_id,
            const std::string &security_desc,            
            const char* sender, 
            const char* target,
            int seq)
    {
        FIX42::OrderCancelRequest orderCancelRequest;
        orderCancelRequest.set(FIX::Account("account"));
        orderCancelRequest.set(FIX::ClOrdID(cl_order_id));
        orderCancelRequest.set(FIX::OrderID(order_id));
        orderCancelRequest.set(FIX::OrigClOrdID(orig_cl_order_id));
        orderCancelRequest.set(FIX::Side(side));  // FIX::Side(FIX::Side_BUY)
        //orderCancelRequest.set(FIX::Symbol(symbol));		// 目前用不到
        orderCancelRequest.set(FIX::TransactTime(true));
        orderCancelRequest.set(FIX::SecurityDesc(security_desc));
        orderCancelRequest.set(FIX::SecurityType("security_type"));

        // 下面的 tag，目前 quickfix 不支持，所以用下面这种方式设置
        orderCancelRequest.setField(fh::cme::exchange::CmeFixField::CorrelationClOrdID, cl_order_id);

        // 下面的 tag，目前 quickfix 的 OrderCancelRequest 不支持，所以用下面这种方式设置
        orderCancelRequest.setField(FIX::FIELD::ManualOrderIndicator, "Y");

        fillHeader( orderCancelRequest.getHeader(), sender, target, seq );
        orderCancelRequest.getHeader().setField( FIX::MsgType( FIX::MsgType_OrderCancelReject ) );
        
        return orderCancelRequest;
    }

    FIX42::NewOrderSingle createNewOrderSingle( const char* sender, const char* target, int seq )
    {
      FIX42::NewOrderSingle newOrderSingle
        ( FIX::ClOrdID("ID"), FIX::HandlInst('1'), FIX::Symbol("SYMBOL"), FIX::Side(FIX::Side_BUY), FIX::TransactTime(), FIX::OrdType(FIX::OrdType_MARKET) );
      fillHeader( newOrderSingle.getHeader(), sender, target, seq );
      return newOrderSingle;
    }
    
    FIX42::Reject createReject( const char* sender, const char* target, int seq, int refSeq )
    {
      FIX42::Reject reject;
      reject.set( FIX::RefSeqNum( refSeq ) );
      fillHeader( reject.getHeader(), sender, target, seq );
      return reject;
    }
    
    FIX42::OrderCancelReject createOrderCancelReject( const char* sender, const char* target, int seq)
    {
      FIX42::OrderCancelReject orderCancelReject;
      orderCancelReject.set( FIX::OrderID( "ORDERID" ) );
      orderCancelReject.set( FIX::ClOrdID( "CLIENTID" ) );
      orderCancelReject.set( FIX::OrigClOrdID( "ORIGINALID" ) );
      orderCancelReject.set( FIX::OrdStatus( '1' ) );
      orderCancelReject.set( FIX::CxlRejResponseTo( '2' ) );
      fillHeader( orderCancelReject.getHeader(), sender, target, seq );
      LOG_DEBUG("orderCancelReject = ", orderCancelReject.toString());
      return orderCancelReject;
    }
    
    
} // namespace utility
} // namespace assist
} // namespace core
} // namespace fh
