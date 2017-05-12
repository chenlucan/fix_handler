
#ifndef __FH_TMALPHA_TRADE_ORDER_EXPIRED_LISTENER_H__
#define __FH_TMALPHA_TRADE_ORDER_EXPIRED_LISTENER_H__

#include <map>
#include "core/global.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{
    class OrderExpiredListener
    {
        public:
            OrderExpiredListener() {};
            virtual ~OrderExpiredListener() {};

        public:
            // 订单已过期
            virtual void On_order_expired(const pb::ems::Order *order) = 0;

        private:
            DISALLOW_COPY_AND_ASSIGN(OrderExpiredListener);
    };
} // namespace trade
} // namespace tmalpha
} // namespace fh

#endif     // __FH_TMALPHA_TRADE_ORDER_EXPIRED_LISTENER_H__
