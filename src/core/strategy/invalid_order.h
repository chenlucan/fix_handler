#ifndef __FH_CORE_STRATEGY_INVALID_ORDER_H__
#define __FH_CORE_STRATEGY_INVALID_ORDER_H__

#include <stdexcept>

namespace fh
{
namespace core
{
namespace strategy
{
    struct InvalidOrder : public std::invalid_argument
    {
        InvalidOrder( const std::string& what = "" ) : invalid_argument(what)
        {
            // noop
        }
    };
} // namespace strategy
} // namespace core
} // namespace fh

#endif     // __FH_CORE_STRATEGY_INVALID_ORDER_H__
