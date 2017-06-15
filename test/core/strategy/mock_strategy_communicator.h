
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
            void OnContractDefinition(const pb::dms::Contract &contract)
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
            // implement of ExchangeListenerI
            typedef boost::container::flat_map<std::string, std::string> ValueMap;
            void OnExchangeReady(ValueMap)
            {
            }

        private:
            void OnOrginalMessage(const std::string &message)
            {
            }

        private:
            DISALLOW_COPY_AND_ASSIGN(MockStrategyCommunicator);
    };
} // namespace strategy
} // namespace core
} // namespace fh

#endif     // __FH_CORE_STRATEGY_MOCK_STRATEGY_COMMUNICATOR_H__
