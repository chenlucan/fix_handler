#include "QueryOrderCommand.h"
namespace fh
{
namespace ctp
{
namespace exchange
{
QueryOrderCommand::QueryOrderCommand(CThostFtdcTraderApi *api, CThostFtdcQryOrderField *orderField,
	int &requestID) :fh::ctp::exchange::ApiCommand(requestID, api){
	this->orderField = orderField;
}

int QueryOrderCommand::execute(){
	return api->ReqQryOrder(orderField, requestID);
}
}
}
}