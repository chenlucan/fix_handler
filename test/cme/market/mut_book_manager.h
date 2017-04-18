#ifndef __FH_CME_MARKET_MUT_BOOK_MANAGER_H__
#define __FH_CME_MARKET_MUT_BOOK_MANAGER_H__

#include "gtest/gtest.h"

#include "core/global.h"
#include "cme/market/message/book.h"
#include "cme/market/book_state_controller.h"
#include "cme/market/message/mdp_message.h"
#include "core/zmq/zmq_sender.h"
#include "cme/market/message/message_parser_r.h"
#include "cme/market/message/message_parser_x.h"
#include "cme/market/message/message_parser_w.h"
#include "cme/market/definition_manager.h"
#include "cme/market/status_manager.h"
#include "core/market/marketlisteneri.h"

namespace fh
{
namespace cme
{
namespace market
{
    class MutBookManager : public testing::Test
    {
        public:
            explicit MutBookManager();
            virtual ~MutBookManager();
            
            virtual void SetUp();
            virtual void TearDown();

            // ”√”⁄multiset≈≈–Ú
            struct Message_Compare
            {
                bool operator() (const fh::cme::market::message::MdpMessage &a, const fh::cme::market::message::MdpMessage &b)
                {
                    return a.packet_seq_num() < b.packet_seq_num();
                }
            };
            
        
            std::multiset<fh::cme::market::message::MdpMessage, Message_Compare> m_datas;

        private:
            DISALLOW_COPY_AND_ASSIGN(MutBookManager);
    };
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_MUT_BOOK_MANAGER_H__
