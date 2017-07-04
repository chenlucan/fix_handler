#include "LoginCommand.h"
namespace fh
{
namespace ctp
{
namespace exchange
{
LoginCommand::LoginCommand(CThostFtdcTraderApi *api, CThostFtdcReqUserLoginField *loginField, int &requestID)
	:fh::ctp::exchange::ApiCommand(requestID,api){
	this->loginField = loginField;
}

int LoginCommand::execute(){
	return api->ReqUserLogin(loginField, requestID);
}
}
}
}