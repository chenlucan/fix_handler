
#ifndef __FH_CORE_ASSIST_COMMON_H__
#define __FH_CORE_ASSIST_COMMON_H__

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <quickfix/Message.h>
#include "core/global.h"
#include "pb/ems/ems.pb.h"
#include "pb/dms/dms.pb.h"

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
        UNKNOW_VALUE = 255
    };
    
    struct DefineMsg_Compare
    {
        std::string market_segment_id;
        std::string security_group;
        std::string activation_date_ime;
        std::string expiration_date_ime;
        
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
    };
    
    void getAbsolutePath(std::string &path);
    
    void show_message(const std::string &hex_message);
    
    // read mdp packets from file(one packet per line) for test 
    // std::vector<std::string>
    void Read_packets(std::vector<std::string> &packets, const std::string &filename, const std::string &packet_from, const std::string &packet_start_indicate = "=");

} // namespace utility
} // namespace assist
} // namespace core
} // namespace fh

#endif //  __FH_CORE_ASSIST_COMMON_H__
