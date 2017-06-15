
#ifndef __FH_TMALPHA_REPLAY_MOCK_REPLAY_EXCHANGE_LISTENER_H__
#define __FH_TMALPHA_REPLAY_MOCK_REPLAY_EXCHANGE_LISTENER_H__

#include <vector>
#include "core/global.h"
#include "core/exchange/exchangelisteneri.h"

namespace fh
{
namespace tmalpha
{
namespace replay
{

    class MockReplayExchangeListener : public core::exchange::ExchangeListenerI
    {
        public:
            MockReplayExchangeListener() : m_orders(), m_fills() {}
            virtual ~MockReplayExchangeListener() {}

        public:
            // implement of ExchangeListenerI
            void OnOrder(const ::pb::ems::Order &order) override
            {
                m_orders.push_back(order);
            }

            // implement of ExchangeListenerI
            void OnFill(const ::pb::ems::Fill &fill) override
            {
                m_fills.push_back(fill);
            }

            // implement of ExchangeListenerI
            void OnPosition(const core::exchange::PositionVec& position) override
            {
                // noop
            }

            // implement of ExchangeListenerI
            void OnExchangeReady(boost::container::flat_map<std::string, std::string>) override
            {
                // noop
            }

            // implement of ExchangeListenerI
            void OnContractAuctioning(const std::string &contract) override
            {
                // noop
            }

            // implement of ExchangeListenerI
            void OnContractNoTrading(const std::string &contract) override
            {
                // noop
            }

            // implement of ExchangeListenerI
            void OnContractTrading(const std::string &contract) override
            {
                // noop
            }

        public:
            std::vector<::pb::ems::Order> &orders()
            {
                return m_orders;
            }

            std::vector<::pb::ems::Fill> &fills()
            {
                return m_fills;
            }

        private:
            std::vector<::pb::ems::Order> m_orders;
            std::vector<::pb::ems::Fill> m_fills;

        private:
            DISALLOW_COPY_AND_ASSIGN(MockReplayExchangeListener);
    };
}   // namespace replay
}   // namespace tmalpha
}   // namespace fh

#endif  // __FH_TMALPHA_REPLAY_MOCK_REPLAY_EXCHANGE_LISTENER_H__

