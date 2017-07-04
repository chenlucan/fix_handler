#include "ReqQryTradeCommand.h"
namespace fh
{
namespace ctp
{
namespace exchange
{
ReqQryTradeCommand::ReqQryTradeCommand(CThostFtdcTraderApi *api, CThostFtdcQryTradeField *orderField,
	int &requestID) :fh::ctp::exchange::ApiCommand(requestID, api){
	this->orderField = orderField;
}

int ReqQryTradeCommand::execute(){
	return api->ReqQryTrade(orderField, requestID);
}
}
}
}