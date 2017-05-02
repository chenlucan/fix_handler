#ifndef __FH_CME_EXCHANGE_ORDER_H__
#define __FH_CME_EXCHANGE_ORDER_H__

#include "core/global.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    // 从策略接收来的订单信息；对应消息类型：
    // D：New Order，F：Order Cancel Request，G：Order Cancel-Replace Request，H：Order Status Request
    struct Order
    {
        std::string symbol;                  // （D/F/G/H）
        std::string security_desc;        // （D/F/G/H）
        std::string cl_order_id;           // （D/F/G/H）
        char side;                                // （D/F/G/H）  1: Buy  2: Sell
        char order_type;                     // （D/G）          1: Market  2: Limit  3: Stop  4: Stop Limit  K: Market-Limit order
        std::uint64_t  order_qty;         // （D/G）
        double price;                         //  （D/G）
        double stop_px;                     // （D/G）
        char time_in_force;               // （D/G）            0: Day  1: Good Till Cancel  3: Fill and Kill  6: Good Till Date
        std::string expire_date;         // （D/G）
        std::string order_id;               // （F/G/H）
        std::string orig_cl_order_id;  // （F/G）
    };

    // 从策略接收来的多订单操作信息；对应消息类型：
    // AF：Order Mass Status Request，CA：Order Mass Action Request
    struct MassOrder
    {
        std::string cl_order_id;                              // （CA）
        std::uint8_t mass_action_type;                 // （CA）      3: Cancel Orders
        std::uint8_t mass_action_scope;               // （CA）     1: Instrument  9: Market Segment ID   10: Instrument Group
        std::string market_segment_id;                // （AF/CA）
        std::string symbol;                                    // （AF/CA）
        std::string security_desc;                          // （AF/CA）
        std::uint8_t mass_cancel_req_type;         // （CA）
        std::string account;                                  // （AF/CA）
        char side;                                                 // （CA）       1: Buy  2: Sell
        char order_type;                                      // （CA）       2: Limit  4: Stop Limit
        char time_in_force;                                 // （AF/CA）  0: Day  1: Good Till Cancel  6: Good Till Date
        std::string mass_status_req_id;                // （AF）
        std::uint8_t mass_status_req_type;         // （AF）         1: Instrument  3: Instrument Group  7: All Orders  100: Market Segment
        std::uint8_t ord_status_req_type;            // （AF）         100: SenderSubID  101: Account
    };

    // 从 CME 接受的订单处理结果；对应消息类型：
    // 9：Order Cancel Reject
    // 8：Execution Report
    //    (1：39-OrdStatus=0, 4 or 5)：Order Creation Cancel or Modify
    //    (2：150-ExecType=I)：Order Status Request Acknowledgment
    //    (3：39-OrdStatus=1 or 2)：Fill Notice
    //    (4：39-OrdStatus=C, 150-ExecType=C)：Order Elimination
    //    (5：39-OrdStatus=8)：Reject
    //    (6：39-OrdStatus=H)：Trade Cancel
    struct SingleOrderReport
    {
        char exec_type;                         // （8） 0: New Order Ack  1: Partial fill  2: Complete fill  4: Cancel Ack   5: Modify Ack  8: Reject Ack  H: Trade Cancel Ack  I: Order Status
        std::string cl_order_id;            // （8，9）
        std::string account;                // （8，9）
        std::string order_id;               // （8，9）
        std::string mass_status_req_id;     // （8-2）
        std::string exec_id;                //（8，9）
        std::string orig_cl_order_id;   // （8，9）
        char order_status;              // （8，9）     0: New Order Ack  1: Partially Filled  2: Complete fill   4: Cancel Ack  5: Modify Ack  8: Rejected  C: Expired   H: Trade Cancel  U: Undefined
        char order_type;                        // （8-1,2,3,4,5）        1: Market  2: Limit  3: Stop  4: Stop Limit  K: Market-Limit
        char side;                                  // （8）      1: Buy  2: Sell
        std::string symbol;                     // （8）
        std::string security_id;      // （8，9）
        std::string security_desc;      //  （8，9）
        char cancel_rej_response_to;       // （9）       1: Order Cancel Request   2: Order Cancel/Replace Request
        char time_in_force;                    // （8-1,2,3,4,5）  0: Day  1: Good Till Cancel  3: Fill and Kill  6: Good Till Date
        std::uint64_t  cum_qty;               // （8）
        std::uint64_t  leaves_qty;               // （8-1,2,3,4,5）
        std::uint64_t  order_qty;               // （8-1,2,3,4,5）
        double price;                            // （8-1,2,3,4,5）
        double last_px;                         // （8-3,6）
        std::uint64_t last_qty;                 // （8-3,6）
        std::uint32_t rej_reason;       // （8-2,5，9）
        std::string text;           // （8-2,5，9）
        std::string transact_time;     // （8，9）
        std::string trade_date;         // （8-2,3,6）
    };

    // 从 CME 接受的多订单处理结果；对应消息类型：
    // BZ：Order Mass Action Report
    struct MassOrderReport
    {
        std::string cl_order_id;
        std::uint8_t mass_action_response;      // 1: Accepted
        std::uint32_t total_affected_orders;
        std::uint32_t no_affected_orders;
        std::string orig_cl_order_ids;       // 将所有的 OrigCIOrdID 用 "^" 拼接起来
        std::string transact_time;
        std::string text;
    };

    // 统一用这个来回调处理函数
    struct OrderReport
    {
        std::string message_type;
        //union
        //{
            SingleOrderReport single_report;
            MassOrderReport mass_report;
        //};
    };

} // namespace exchange
} // namespace cme
} // namespace fh

#endif     // __FH_CME_EXCHANGE_ORDER_H__
