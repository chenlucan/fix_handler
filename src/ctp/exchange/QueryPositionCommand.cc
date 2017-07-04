#include "QueryPositionCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
QueryPositionCommand::QueryPositionCommand(CThostFtdcTraderApi *api, CThostFtdcQryInvestorPositionField *accountField,
	int &requestID) :fh::ctp::exchange::ApiCommand(requestID, api){
	this->accountField = accountField;
}

int QueryPositionCommand::execute(){
	return api->ReqQryInvestorPosition(accountField, requestID);
}
}
}
}