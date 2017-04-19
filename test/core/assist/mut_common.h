
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
    void getAbsolutePath(std::string &path);
    
    void show_message(const std::string &hex_message);
} // namespace utility
} // namespace assist
} // namespace core
} // namespace fh

#endif //  __FH_CORE_ASSIST_COMMON_H__
