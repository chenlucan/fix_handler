
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
        std::ostringstream ss;
        auto *facet = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f");    // not required to delete it
        ss.imbue(std::locale(ss.getloc(), facet));
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        ss << now;

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

} // namespace utility
} // namespace assist
} // namespace core
} // namespace fh
