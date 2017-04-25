#ifndef __FH_FEMA_STRATEGY_GLOBX_H__
#define __FH_FEMA_STRATEGY_GLOBX_H__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "core/global.h"
#include "core/zmq/zmq_receiver.h"
#include "core/zmq/zmq_sender.h"
#include "core/exchange/exchangelisteneri.h"
#include "core/exchange/exchangei.h"
#include "pb/ems/ems.pb.h"

namespace fh
{
namespace femas
{
namespace exchange
{


    class CFemasStrategyCommunicator : public core::exchange::ExchangeListenerI
    {
        public:
            CFemasStrategyCommunicator(const std::string &send_url, const std::string &receive_url);
            virtual ~CFemasStrategyCommunicator();

        public:
            void Start_receive();
            void Set_exchange(core::exchange::ExchangeI *exchange);

	 public:		
	     // implement of ExchangeListenerI
            virtual void OnOrder(const ::pb::ems::Order &order) override;
            // implement of ExchangeListenerI
            virtual void OnFill(const ::pb::ems::Fill &fill) override;
            // implement of ExchangeListenerI
            virtual void OnPosition(const core::exchange::PositionVec& position) override;
            // implement of ExchangeListenerI
            virtual void OnExchangeReady(boost::container::flat_map<std::string, std::string>) override;
            // implement of ExchangeListenerI
            virtual void OnContractAuctioning(const std::string &contract) override;
            // implement of ExchangeListenerI
            virtual void OnContractNoTrading(const std::string &contract) override;
            // implement of ExchangeListenerI
            virtual void OnContractTrading(const std::string &contract) override;	
    };





}
}
}









#endif