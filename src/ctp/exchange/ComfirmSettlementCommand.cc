#include "ComfirmSettlementCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
ComfirmSettlementCommand::ComfirmSettlementCommand(CThostFtdcTraderApi *api,
	CThostFtdcSettlementInfoConfirmField *comfirmField, int &requestID) :fh::ctp::exchange::ApiCommand(requestID, api){
	this->comfirmField = comfirmField;
}

int ComfirmSettlementCommand::execute(){
	return api->ReqSettlementInfoConfirm(comfirmField, requestID);
}
}
}
}