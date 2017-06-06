
#ifndef __FH_CORE_STRATEGY_STRATEGY_COMMUNICATOR_H__
#define __FH_CORE_STRATEGY_STRATEGY_COMMUNICATOR_H__

#include "core/global.h"
#include "core/zmq/zmq_receiver.h"
#include "core/zmq/zmq_sender.h"
#include "core/exchange/exchangelisteneri.h"
#include "core/exchange/exchangei.h"
#include "pb/ems/ems.pb.h"
#include "pb/dms/dms.pb.h"

namespace fh
{
namespace core
{
namespace strategy
{
    class StrategyReceiver : public fh::core::zmq::ZmqReceiver
    {
        public:
            explicit StrategyReceiver(const std::string &url);
            virtual ~StrategyReceiver();

        public:
            void Set_processor(std::function<void(char *, const size_t)> processor);
            virtual void Save(char *data, size_t size);

        private:
            std::function<void(char *, const size_t)> m_processor;

        private:
            DISALLOW_COPY_AND_ASSIGN(StrategyReceiver);
    };

    class StrategyCommunicator : public core::exchange::ExchangeListenerI
    {
        public:
            StrategyCommunicator(const std::string &send_url,const std::string &org_url, const std::string &receive_url);
	     StrategyCommunicator(const std::string &send_url,const std::string &receive_url);		
            virtual ~StrategyCommunicator();

        public:
            void Start_receive();
            void Set_exchange(core::exchange::ExchangeI *exchange);

        public:
            // implement of ExchangeListenerI
            void OnOrder(const ::pb::ems::Order &order) override;
            // implement of ExchangeListenerI
            void OnFill(const ::pb::ems::Fill &fill) override;
            // implement of ExchangeListenerI
            void OnPosition(const core::exchange::PositionVec& position) override;
	     // implement of MarketListenerI
            void OnContractDefinition(const pb::dms::Contract &contract) override;		
            // implement of ExchangeListenerI
            void OnExchangeReady(boost::container::flat_map<std::string, std::string>) override;
            // implement of ExchangeListenerI
            void OnContractAuctioning(const std::string &contract) override;
            // implement of ExchangeListenerI
            void OnContractNoTrading(const std::string &contract) override;
            // implement of ExchangeListenerI
            void OnContractTrading(const std::string &contract) override;
	     // implement of ExchangeListenerI
            void OnOrginalMessage(const std::string &message) override;		

        private:
            void On_from_strategy(char *data, size_t size);
            void Order_request(const char *data, size_t size);
            static ::pb::ems::Order Create_order(const char *data, size_t size);

        private:
	     fh::core::zmq::ZmqSender m_org_sender;		 
            fh::core::zmq::ZmqSender m_sender;
            StrategyReceiver m_receiver;
            core::exchange::ExchangeI *m_exchange;

        private:
            DISALLOW_COPY_AND_ASSIGN(StrategyCommunicator);
    };
} // namespace strategy
} // namespace core
} // namespace fh

#endif     // __FH_CORE_STRATEGY_STRATEGY_COMMUNICATOR_H__
