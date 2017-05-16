
#ifndef __FH_CORE_ASSIST_COMMON_H__
#define __FH_CORE_ASSIST_COMMON_H__

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <quickfix/Message.h>
#include <quickfix/DataDictionaryProvider.h>
#include <quickfix/fix42/Logon.h>
#include <quickfix/fix42/Logout.h>
#include <quickfix/fix42/Heartbeat.h>
#include <quickfix/fix42/TestRequest.h>
#include <quickfix/fix42/SequenceReset.h>
#include <quickfix/fix42/ResendRequest.h>
#include <quickfix/fix42/Reject.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/BusinessMessageReject.h>
#include <quickfix/fix42/OrderCancelReject.h>
#include <quickfix/fix42/OrderStatusRequest.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>

#include "core/global.h"
#include "pb/ems/ems.pb.h"
#include "pb/dms/dms.pb.h"

#include "cme/exchange/order_manager.h"


namespace fh
{
namespace core
{
namespace assist
{
namespace common
{
    enum CaseIdValue 
    {
        MakePrice_1 = 1,
        MakePrice_2 = 2,
        Sd_1 = 3,
        Sd_2 = 4,
        Fs_1 = 5,
        Fs_2 = 6,
        Order_Qty = 7,
        Sm_1 = 8,
        Sm_2 = 9,
        Sm_3 = 10,
        Sm_4 = 11,
        Sm_5 = 12,
        Sm_6 = 13,
        ChannelReset_MakePrice_1 = 14,
        ChannelReset_MakePrice_2 = 15,
        Market_State_Message_1 = 16,
        Market_State_Message_2 = 17,
        Market_State_Message_3 = 18,
        Market_State_Message_4 = 19,
        Market_State_Message_5 = 20,
        Sdmfo_1 = 21,
        Sdmff_1 = 22,
        qty_pc_1st_instr =23,   // 4Confirm quantities and prices on the book for the first instrument (non implied)
        qty_pc_2nd_instr =24,   // 7Confirm quantities and prices on the book for the second instrument (non implied)
        UNKNOW_VALUE = 255
    };
    
    struct DefineMsg_Compare
    {
        std::string market_segment_id;
        std::string security_group;
        std::string activation_date_ime;
        std::string expiration_date_ime;
        
        std::string strike_price_mantissa;  // strikePrice
        std::string strike_price_exponent; 
        
        std::string order_qty;
        
        std::string mdentry_px_mantissa;  // mdentry_px
        std::string mdentry_px_exponent;
        std::string mdentry_size;
        
        std::string highlimit_price_mantissa;  // highLimitPrice
        std::string highlimit_price_exponent;
        std::string lowlimit_price_mantissa;  // lowLimitPrice
        std::string lowlimit_price_exponent; 
        
        
        std::string security_trading_status; // Market State Message
        std::string security_trading_event;

        
        std::string To_string() const
        {
            std::ostringstream os;
            os << "DefineMsg_Compare={";
            os << "market_segment_id=" << market_segment_id << " ";
            os << "security_group=" << security_group << " ";
            os << "activation_date_ime=" << activation_date_ime << " ";            
            os << "expiration_date_ime=" << expiration_date_ime << "}";

            return os.str();
        }
        
        std::string To_order_qty_string() const
        {
            std::ostringstream os;
            os << "DefineMsg_Compare={";            
            os << "orderQty=" << order_qty << "}";

            return os.str();
        }
        
        std::string To_mdentry_price_size_string() const
        {
            std::ostringstream os;
            os << "mDEntry_Price_Size_Compare={";    
            os << "mdentry_px_mantissa=" << mdentry_px_mantissa << " ";  
            os << "mdentry_px_exponent=" << mdentry_px_exponent << " ";             
            os << "mdentry_size=" << mdentry_size << "}";

            return os.str();
        }
        
        std::string To_limit_price_string() const
        {
            std::ostringstream os;
            os << "limit_Price_Compare={";    
            os << "highlimit_price_mantissa=" << highlimit_price_mantissa << " ";  
            os << "highlimit_price_exponent=" << highlimit_price_exponent << " ";     
            os << "lowlimit_price_mantissa=" << lowlimit_price_mantissa << " ";  
            os << "lowlimit_price_exponent=" << lowlimit_price_exponent << " ";               
            os << "mdentry_size=" << mdentry_size << "}";

            return os.str();
        }
        
        std::string To_security_trading_status_string() const
        {
            std::ostringstream os;
            os << "security_trading_status_Compare={";    
            os << "security_trading_status=" << security_trading_status << " ";   
            os << "security_trading_event=" << security_trading_event << "}";

            return os.str();
        }
        
        std::string To_security_definition_messages_for_options_string() const
        {
            std::ostringstream os;
            os << "security_definition_messages_for_options_Compare={";
            os << "market_segment_id=" << market_segment_id << " ";
            os << "security_group=" << security_group << " ";
            os << "activation_date_ime=" << activation_date_ime << " ";
            os << "expiration_date_ime=" << expiration_date_ime << " ";
            os << "strike_price_mantissa=" << strike_price_mantissa << " ";  
            os << "strike_price_exponent=" << strike_price_exponent << "}";             
            
            return os.str();
        }
        
    };
    
    void getAbsolutePath(std::string &path);
    
    void show_message(const std::string &hex_message);
    
    // read mdp packets from file(one packet per line) for test 
    // std::vector<std::string>
    void Read_packets(std::vector<std::string> &packets, const std::string &filename, const std::string &packet_from, const std::string &packet_start_indicate = "=");

    void fillHeader( FIX::Header& header, const char* sender, const char* target, int seq );
    
    FIX42::ResendRequest createResendRequest( const char* sender, const char* target, int seq, int begin, int end );
    
    FIX42::ExecutionReport createExecutionReport( const char* sender, const char* target, int seq );    
        
    FIX42::Logon createLogon( const char* sender, const char* target, int seq );
    
    FIX42::Logout createLogout( const char* sender, const char* target, int seq );
    
    FIX42::Heartbeat createHeartbeat( const char* sender, const char* target, int seq );
    
    FIX42::TestRequest createTestRequest( const char* sender, const char* target, int seq, const char* id );
    
    FIX42::SequenceReset createSequenceReset( const char* sender, const char* target, int seq, int newSeq );
    
    FIX42::Message createMessage(const char* sender, const char* target, int seq, const char* type );
    
    FIX42::OrderCancelRequest createOrderCancelRequest42(
            const std::string &cl_order_id,
            const std::string &orig_cl_order_id,
            char side,
            const std::string &symbol,
            const std::string &order_id,
            const std::string &security_desc,
            const char* sender, 
            const char* target,
            int seq);
           
    FIX42::NewOrderSingle createNewOrderSingle( const char* sender, const char* target, int seq );
    
    FIX42::Reject createReject( const char* sender, const char* target, int seq, int refSeq );
    
} // namespace utility
} // namespace assist
} // namespace core
} // namespace fh

#endif //  __FH_CORE_ASSIST_COMMON_H__
