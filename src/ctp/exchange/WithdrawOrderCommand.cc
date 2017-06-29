#include "WithdrawOrderCommand.h"
namespace fh
{
namespace ctp
{
namespace exchange
{
WithdrawOrderCommand::WithdrawOrderCommand(CThostFtdcTraderApi *api, CThostFtdcInputOrderActionField *orderField,
	int &requestID) :fh::ctp::exchange::ApiCommand(requestID, api){
	this->orderField = orderField;
}

int WithdrawOrderCommand::execute(){
	return api->ReqOrderAction(orderField, requestID);
}
}
}
}