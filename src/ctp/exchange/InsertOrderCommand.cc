#include "InsertOrderCommand.h"
#include <iostream>
namespace fh
{
namespace ctp
{
namespace exchange
{
InsertOrderCommand::InsertOrderCommand(CThostFtdcTraderApi *api, CThostFtdcInputOrderField *orderField,
	int &requestID) :fh::ctp::exchange::ApiCommand(requestID, api){
	this->orderField = orderField;
}

int InsertOrderCommand::execute(){
	return api->ReqOrderInsert(orderField, requestID);
}

}
}
}