#include "ReqQryInstrumentCommand.h"
namespace fh
{
namespace ctp
{
namespace exchange
{
	
ReqQryInstrumentCommand::ReqQryInstrumentCommand(CThostFtdcTraderApi *api, CThostFtdcQryInstrumentField *orderField,
	int &requestID) :fh::ctp::exchange::ApiCommand(requestID, api){
	this->orderField = orderField;
}

int ReqQryInstrumentCommand::execute(){
	return api->ReqQryInstrument(orderField, requestID);
}

}
}
}