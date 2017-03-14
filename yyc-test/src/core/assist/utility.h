
#ifndef __FH_CORE_ASSIST_UTILITY_H__
#define __FH_CORE_ASSIST_UTILITY_H__

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <quickfix/Message.h>
#include "core/global.h"
#include "pb/ems/ems.pb.h"

namespace fh
{
namespace core
{
namespace assist
{
namespace utility
{
    // get current time in nanoseconds since epoch
    std::uint64_t Current_time_ns();

    // get current time in "yyyy-MM-dd HH:mm:ss.ssssss" format
    std::string Current_time_str();

    // translate each char in buffer to hex format for display
    // ex: "abc" -> "61 62 63 "
    std::string Hex_str(const char *buffer, std::size_t length);

    // generate random number between min and max
    std::uint32_t Random_number(std::uint32_t min, std::uint32_t max);

    // pad string on right with special character or delete character from right if size is longer than special
    std::string Pad_Right(const std::string &str, const std::size_t total_length, const char padding_char = ' ');

    // return simple file name from a path with folder
    std::string Get_filename(const char *path);

    // calculate fix message checksum
    std::string Calculate_fix_checksum(const std::string &message);

    // format fix message to display well
    std::string Format_fix_message(const std::string &message);

    // format fix message to display well
    std::string Format_fix_message(const FIX::Message &message);

    // 取得 quickfix 保存的当前处理掉的最后一个服务器端序号
    std::string Last_processed_seq_num(const FIX::SessionID& sessionID);

    // 取得 quickfix 保存的当前处理掉的最后一个服务器端序号
    std::string Last_processed_seq_num(const FIX::Header& header);

    // 取得 quickfix 保存的期望的下一个服务器端序号
    std::string Next_expected_seq_num(const FIX::SessionID& sessionID);

    // 重置quickfix 保存的客户端，服务器端序号
    void Reset_seq_num(const FIX::SessionID& sessionID);

    // 判断一个 quickfix 自动做成的 resend request 是不是重复发送的请求
    bool Is_duplicate_resend_request(const FIX::SessionID& sessionID);

    // 把 quickfix 的时间类型转换成标准类型
    boost::posix_time::ptime Fix_time_to_posix(const FIX::UtcTimeStamp &value);

    // 把字符串类型的时间（yyyyMMdd-HH:mi:ss.fff）转换成 protobuf 类型
    void To_pb_time(pb::ems::Timestamp *ts, const std::string &datetime);

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::ems::Order &order);

    // 把 protobuf 的消息整理成可读字符串
    std::string Format_pb_message(const pb::ems::Fill &fill);

    // 把 protobuf 的时间格式整理成可读字符串
    std::string Format_pb_timestamp(const pb::ems::Timestamp &time);

} // namespace utility
} // namespace assist
} // namespace core
} // namespace fh

#endif //  __FH_CORE_ASSIST_UTILITY_H__
