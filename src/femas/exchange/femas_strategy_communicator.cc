#include "femas_strategy_communicator.h"
#include "core/assist/logger.h"

namespace fh
{
namespace femas
{
namespace exchange
{


CFemasStrategyCommunicator::CFemasStrategyCommunicator(const std::string &send_url, const std::string &receive_url)
{
    LOG_INFO("CFemasStrategyCommunicator::CFemasStrategyCommunicator ");
}

CFemasStrategyCommunicator::~CFemasStrategyCommunicator()
{
    LOG_INFO("CFemasStrategyCommunicator::~CFemasStrategyCommunicator ");
}



void Start_receive()
{
    LOG_INFO("CFemasStrategyCommunicator::Start_receive ");
}

void Set_exchange(core::exchange::ExchangeI *exchange)
{
    LOG_INFO("CFemasStrategyCommunicator::Set_exchange ");
}



void CFemasStrategyCommunicator::OnOrder(const ::pb::ems::Order &order) 
{
    LOG_INFO("CFemasStrategyCommunicator::OnOrder ");
}

void CFemasStrategyCommunicator::OnFill(const ::pb::ems::Fill &fill) 
{
    LOG_INFO("CFemasStrategyCommunicator::OnFill ");
}

void CFemasStrategyCommunicator::OnPosition(const core::exchange::PositionVec& position)
{
    LOG_INFO("CFemasStrategyCommunicator::OnPosition ");
}

void CFemasStrategyCommunicator::OnExchangeReady(boost::container::flat_map<std::string, std::string>) 
{
    LOG_INFO("CFemasStrategyCommunicator::OnExchangeReady ");
}

void CFemasStrategyCommunicator::OnContractAuctioning(const std::string &contract)
{
    LOG_INFO("CFemasStrategyCommunicator::OnContractAuctioning ");
}

void CFemasStrategyCommunicator::OnContractNoTrading(const std::string &contract)
{
    LOG_INFO("CFemasStrategyCommunicator::OnContractNoTrading ");
}

void CFemasStrategyCommunicator::OnContractTrading(const std::string &contract) 
{
    LOG_INFO("CFemasStrategyCommunicator::OnContractTrading ");
}


}
}
}