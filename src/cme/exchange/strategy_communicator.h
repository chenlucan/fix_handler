
#ifndef __FH_CME_EXCHANGE_STRATEGY_COMMUNICATOR_H__
#define __FH_CME_EXCHANGE_STRATEGY_COMMUNICATOR_H__

#include "core/global.h"
#include "core/zmq/zmq_receiver.h"
#include "core/zmq/zmq_sender.h"
#include "core/exchange/exchangelisteneri.h"

namespace fh
{
namespace cme
{
namespace exchange
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
            StrategyCommunicator(const std::string &send_url, const std::string &receive_url);
            virtual ~StrategyCommunicator();

        public:
            void Start_receive(std::function<void(char *, const size_t)> processor);

        public:
            // implement of ExchangeListenerI
            void OnOrder(const ::pb::ems::Order &order) override;
            void OnFill(const ::pb::ems::Fill &fill) override;
            void OnPosition(const core::exchange::PositionVec& position) override;
            void OnExchangeReady(boost::container::flat_map<std::string, std::string>) override;
            void OnContractAuctioning(std::string contract) override;
            void OnContractNoTrading(std::string contract) override;
            void OnContractTrading(std::string contract) override;

        private:
            fh::core::zmq::ZmqSender m_sender;
            StrategyReceiver m_receiver;

        private:
            DISALLOW_COPY_AND_ASSIGN(StrategyCommunicator);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif     // __FH_CME_EXCHANGE_STRATEGY_COMMUNICATOR_H__
