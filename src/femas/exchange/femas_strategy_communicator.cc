#include "femas_strategy_communicator.h"


namespace fh
{
namespace femas
{
namespace exchange
{


CFemasStrategyCommunicator::CFemasStrategyCommunicator(const std::string &send_url, const std::string &receive_url)
{

}

CFemasStrategyCommunicator::~CFemasStrategyCommunicator()
{

}



void CFemasStrategyCommunicator::OnOrder(const ::pb::ems::Order &order) 
{

}

void CFemasStrategyCommunicator::OnFill(const ::pb::ems::Fill &fill) 
{

}

void CFemasStrategyCommunicator::OnPosition(const core::exchange::PositionVec& position)
{

}

void CFemasStrategyCommunicator::OnExchangeReady(boost::container::flat_map<std::string, std::string>) 
{

}

void CFemasStrategyCommunicator::OnContractAuctioning(const std::string &contract)
{

}

void CFemasStrategyCommunicator::OnContractNoTrading(const std::string &contract)
{

}

void CFemasStrategyCommunicator::OnContractTrading(const std::string &contract) 
{

}


}
}
}