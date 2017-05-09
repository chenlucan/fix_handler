
#ifndef __FH_CORE_STRATEGY_MOCK_STRATEGY_COMMUNICATOR_H__
#define __FH_CORE_STRATEGY_MOCK_STRATEGY_COMMUNICATOR_H__

#include "core/global.h"
#include "core/exchange/exchangelisteneri.h"
#include "core/exchange/exchangei.h"
#include "pb/ems/ems.pb.h"

namespace fh
{
namespace core
{
namespace strategy
{
    class MockStrategyCommunicator : public core::exchange::ExchangeListenerI
    {
        public:
            MockStrategyCommunicator()
            {
            }
            
            virtual ~MockStrategyCommunicator()
            {
            }

        public:
            void Start_receive()
            {
            }
            void Set_exchange(core::exchange::ExchangeI *exchange)
            {
            }

        public:
            // implement of ExchangeListenerI
            void OnOrder(const ::pb::ems::Order &order)
            {
            }
            // implement of ExchangeListenerI
            void OnFill(const ::pb::ems::Fill &fill)
            {
            }
            // implement of ExchangeListenerI
            void OnPosition(const core::exchange::PositionVec& position)
            {
            }
            // implement of ExchangeListenerI
            void OnExchangeReady(boost::container::flat_map<std::string, std::string>)
            {
            }
            // implement of ExchangeListenerI
            void OnContractAuctioning(const std::string &contract)
            {
            }
            // implement of ExchangeListenerI
            void OnContractNoTrading(const std::string &contract)
            {
            }
            // implement of ExchangeListenerI
            void OnContractTrading(const std::string &contract)
            {
            }

        private:
            void On_from_strategy(char *data, size_t size)
            {
            }
            void Order_request(const char *data, size_t size)
            {
            }

        private:
            DISALLOW_COPY_AND_ASSIGN(MockStrategyCommunicator);
    };
} // namespace strategy
} // namespace core
} // namespace fh

#endif     // __FH_CORE_STRATEGY_MOCK_STRATEGY_COMMUNICATOR_H__
