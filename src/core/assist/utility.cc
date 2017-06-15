
#include <chrono>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <quickfix/Session.h>
#include "core/assist/utility.h"
#include "core/assist/logger.h"

namespace fh
{
namespace core
{
namespace assist
{
namespace utility
{

    // get current time in nanoseconds since epoch
    std::uint64_t Current_time_ns()
    {
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    }

    // get current time in "yyyy-MM-dd HH:mm:ss.ssssss" format
    std::string Current_time_str()
    {
        return Current_time_str("%Y-%m-%d %H:%M:%S.%f");
    }

    // get current time in special format
    std::string Current_time_str(const char *format)
    {
        std::ostringstream ss;
        auto *facet = new boost::posix_time::time_facet(format);    // not required to delete it
        ss.imbue(std::locale(ss.getloc(), facet));
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        ss << now;

        return ss.str();
    }

    // convert time to "yyyy-MM-dd HH:mm:ss.ssssss" format
    std::string To_time_str(std::uint64_t nanoseconds)
    {
        boost::posix_time::ptime time_epoch(boost::gregorian::date(1970, 1, 1));
        boost::posix_time::ptime date(time_epoch + boost::posix_time::microseconds(nanoseconds / 1000));

        std::ostringstream ss;
        auto *facet = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f");    // not required to delete it
        ss.imbue(std::locale(ss.getloc(), facet));
        ss << date;

        return ss.str();
    }

    // translate each char in buffer to hex format for display
    // ex: "abc" -> "61 62 63 "
    std::string Hex_str(const char *buffer, std::size_t length)
    {
        std::ostringstream ss;

        for (std::size_t i = 0; i < length; ++i)
        {
            ss << std::hex << std::setfill('0') << std::setw(2) << (int)(unsigned char)buffer[i] << " ";
        }

        return ss.str();
    }

    // generate random number between min and max
    std::uint32_t Random_number(std::uint32_t min, std::uint32_t max)
    {
        static std::default_random_engine g(Current_time_ns());
        std::uniform_int_distribution<std::uint32_t> r(min, max);  
        return r(g);
    }

    // pad string on right with special character or delete character from right if size is longer than special
    std::string Pad_Right(const std::string &str, const std::size_t total_length, const char padding_char)
    {
        std::string nstr(str);
        nstr.resize(total_length, padding_char);
        return nstr;
    }

    // return simple file name from a path with folder
    std::string Get_filename(const char *path)
    {
        std::string str(path);
        auto pos = str.find_last_of('/');
        if(pos == std::string::npos)
        {
            return str;
        }
        else
        {
            return str.substr(pos + 1);
        }
    }

    // calculate fix message checksum
    std::string Calculate_fix_checksum(const std::string &message)
    {
        std::uint32_t sum = 0;
        std::for_each(message.cbegin(), message.cend(), [&sum](const char c){ sum += (std::uint32_t)c; });
        char checksum[8];
        snprintf(checksum, sizeof(checksum), "10=%03d\001", sum % 256);

        return checksum;
    }

    // format fix message to display well
    std::string Format_fix_message(const std::string &message)
    {
        return boost::replace_all_copy(message, "\001", " ");
    }

    // format fix message to display well
    std::string Format_fix_message(const FIX::Message &message)
    {
        return Format_fix_message(message.toString());
    }

    // 判断指定的 session 是否已经准备就绪
    bool Is_fix_session_exist(const FIX::SessionID& sessionID)
    {
        return FIX::Session::lookupSession(sessionID) != nullptr;
    }

    // 取得 quickfix 保存的当前处理掉的最后一个服务器端序号
    std::string Last_processed_seq_num(const FIX::SessionID& sessionID)
    {
        FIX::Session *session = FIX::Session::lookupSession(sessionID);
        return std::to_string(std::max(session->getExpectedTargetNum() - 1, 0));
    }

    // 取得 quickfix 保存的当前处理掉的最后一个服务器端序号
    std::string Last_processed_seq_num(const FIX::Header& header)
    {
        const FIX::BeginString& beginString = FIELD_GET_REF( header, BeginString );
        const FIX::SenderCompID& senderCompID = FIELD_GET_REF( header, SenderCompID );
        const FIX::TargetCompID& targetCompID = FIELD_GET_REF( header, TargetCompID );

        FIX::Session *session = FIX::Session::lookupSession(FIX::SessionID( beginString, senderCompID, targetCompID ) );
        return std::to_string(std::max(session->getExpectedTargetNum() - 1, 0));
    }

    // 取得 quickfix 保存的期望的下一个服务器端序号
    std::string Next_expected_seq_num(const FIX::SessionID& sessionID)
    {
        FIX::Session *session = FIX::Session::lookupSession(sessionID);
        return std::to_string(session->getExpectedTargetNum());
    }

    // 重置quickfix 保存的客户端，服务器端序号
    void Reset_seq_num(const FIX::SessionID& sessionID)
    {
        FIX::Session *session = FIX::Session::lookupSession(sessionID);
        session->setNextSenderMsgSeqNum(1);
        session->setNextTargetMsgSeqNum(1);
    }

    // 判断一个 quickfix 自动做成的 resend request 是不是重复发送的请求
    bool Is_duplicate_resend_request(const FIX::SessionID& sessionID)
    {
        FIX::Session *session = FIX::Session::lookupSession(sessionID);
        auto state = static_cast<const FIX::SessionState *>(session->getStore());
        return state->resendRequested();
    }

    // 获取一个 fix message 的类型
    std::string Fix_message_type(const FIX::Message &message)
    {
        return message.getHeader().getField( FIX::FIELD::MsgType );
    }

    // 把 quickfix 的时间类型转换成标准类型
    boost::posix_time::ptime Fix_time_to_posix(const FIX::UtcTimeStamp & value)
    {
        boost::gregorian::date d(value.getYear(), value.getMonth(), value.getDay());
        boost::posix_time::time_duration t(value.getHour(), value.getMinute(), value.getSecond());

        return boost::posix_time::ptime(d, t + boost::posix_time::millisec(value.getMillisecond()));
    }

    // 把字符串类型的时间（yyyyMMdd-HH:mi:ss.fff）转换成 protobuf 类型
    void To_pb_time(pb::ems::Timestamp *ts, const std::string &datetime)
    {
        if(datetime.length() != 21)
        {
            LOG_WARN("invalid datetime: ", datetime, ", length=", datetime.length());
            return;
        }

        pb::ems::Date *d = ts->mutable_date();
        d->set_year(boost::lexical_cast<std::uint32_t>(datetime.data(), 4));
        d->set_month(boost::lexical_cast<std::uint32_t>(datetime.data() + 4, 2));
        d->set_day(boost::lexical_cast<std::uint32_t>(datetime.data() + 6, 2));

        pb::ems::Time *t = ts->mutable_time();
        t->set_hour(boost::lexical_cast<std::uint32_t>(datetime.data() + 9, 2));
        t->set_minute(boost::lexical_cast<std::uint32_t>(datetime.data() + 12, 2));
        t->set_second(boost::lexical_cast<std::uint32_t>(datetime.data() + 15, 2));
        t->set_micros(boost::lexical_cast<std::uint32_t>(datetime.data() + 18, 3));

        ts->set_timezone(0);  // 时间都是 UTC
    }

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::ems::Order &order)
    {
        std::ostringstream oss;

        oss << "client_order_id=" << order.client_order_id() << ", ";

        oss << "account=" << order.account() << ", ";
        oss << "contract=" << order.contract() << ", ";
        oss << "buy_sell=" << order.buy_sell() << ", ";
        oss << "price=" << order.price() << ", ";
        oss << "quantity=" << order.quantity() << ", ";
        oss << "tif=" << order.tif() << ", ";
        oss << "order_type=" << order.order_type() << ", ";
        oss << "exchange_order_id=" << order.exchange_order_id() << ", ";
        oss << "status=" << order.status() << ", ";
        oss << "working_price=" << order.working_price() << ", ";
        oss << "working_quantity=" << order.working_quantity() << ", ";
        oss << "filled_quantity=" << order.filled_quantity() << ", ";
        oss << "message=" << order.message() << ", ";
        oss << "submit_time=" << Format_pb_timestamp(order.submit_time());

        return oss.str();
    }

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::ems::Fill &fill)
    {
        std::ostringstream oss;

        oss << "fill_id=" << fill.fill_id() << ", ";
        oss << "fill_quantity=" << fill.fill_quantity() << ", ";
        oss << "fill_price=" << fill.fill_price() << ", ";
        oss << "account=" << fill.account() << ", ";
        oss << "client_order_id=" << fill.client_order_id() << ", ";
        oss << "exchange_order_id=" << fill.exchange_order_id() << ", ";
        oss << "contract=" << fill.contract() << ", ";
        oss << "buy_sell=" << fill.buy_sell() << ", ";
        oss << "fill_time=" << Format_pb_timestamp(fill.fill_time());

        return oss.str();
    }

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::dms::Trade &trade)
    {
        std::ostringstream oss;

        oss << "contract=" << trade.contract() << ", ";
        oss << "last=" << Format_pb_message(trade.last());

        return oss.str();
    }

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::dms::BBO &bbo)
    {
        std::ostringstream oss;

        oss << "contract=" << bbo.contract() << ", ";
        oss << "bid=" << Format_pb_message(bbo.bid());
        oss << "offer=" << Format_pb_message(bbo.offer());

        return oss.str();
    }

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::dms::Bid &bid)
    {
        std::ostringstream oss;

        oss << "contract=" << bid.contract() << ", ";
        oss << "bid=" << Format_pb_message(bid.bid());

        return oss.str();
    }

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::dms::Offer &offer)
    {
        std::ostringstream oss;

        oss << "contract=" << offer.contract() << ", ";
        oss << "offer=" << Format_pb_message(offer.offer());

        return oss.str();
    }

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::dms::L2 &l2)
    {
        std::ostringstream oss;

        oss << "contract=" << l2.contract() << ", ";
        oss << "bid=";
        std::for_each(l2.bid().begin(), l2.bid().end(), [&oss](const pb::dms::DataPoint &dp){ oss << Format_pb_message(dp); });
        oss << ", offer=";
        std::for_each(l2.offer().begin(), l2.offer().end(), [&oss](const pb::dms::DataPoint &dp){ oss << Format_pb_message(dp); });

        return oss.str();
    }

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::dms::Contract &contract)
    {
        std::ostringstream oss;

        oss << "name=" << contract.name() << ", ";
        oss << "tick_size=" << contract.tick_size() << ", ";
        oss << "tick_value=" << contract.tick_value() << ", ";
        oss << "yesterday_close_price=" << contract.yesterday_close_price() << ", ";
        oss << "upper_limit=" << contract.upper_limit() << ", ";
        oss << "lower_limit=" << contract.lower_limit() << ", ";
        oss << "contract_type=" << contract.contract_type() << ", ";
        oss << "legA=" << contract.lega() << ", ";
        oss << "legB=" << contract.legb();

        return oss.str();
    }

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::dms::DataPoint &dp)
    {
        std::ostringstream oss;

        oss << "[price=" << std::to_string(dp.price()) << ", ";
        oss << "size=" << dp.size() << "]";

        return oss.str();
    }

    // 把 protobuf 的时间格式整理成可读字符串
    std::string Format_pb_timestamp(const pb::ems::Timestamp &dt)
    {
        std::ostringstream oss;

        oss << dt.date().year() << "/";
        oss << dt.date().month() << "/";
        oss << dt.date().day() << " ";
        oss << dt.time().hour() << ":";
        oss << dt.time().minute() << ":";
        oss << dt.time().second() << ".";
        oss << dt.time().micros();

        return oss.str();
    }

    // 把一个字符串中的第一个 null(\0) 以及后面的字符都删除掉
    std::string Trim_null(const std::string &str)
    {
        auto pos = str.find('\0');
        if(pos == std::string::npos) return str;
        return str.substr(0, pos);
    }

    bool Is_price_valid(const std::string &str)
    {
        int len = str.length();
        if(!len)
        {
            return false;
        }

        if( (0 == isdigit(str[0])) || (0 == isdigit(str[len-1])) )
        {
            return false;
        }

        int count=0;
        for(int i = 1; i<len-1; i++)
        { 
            if( (0 == isdigit(str[i])) && (str[i] != '.') )
            {
                return false;
            }
            else if(str[i] == '.')
            {
                count++;
            }            
        }

        if(count>1)
        {
            return false;
        }
        
        return true;
    }

} // namespace utility
} // namespace assist
} // namespace core
} // namespace fh
