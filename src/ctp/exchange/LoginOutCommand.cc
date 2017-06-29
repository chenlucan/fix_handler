#include "LoginOutCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
LoginOutCommand::LoginOutCommand(CThostFtdcTraderApi *api, CThostFtdcUserLogoutField *loginField, int &requestID)
	:fh::ctp::exchange::ApiCommand(requestID,api){
	this->loginField = loginField;
}

int LoginOutCommand::execute(){
	return api->ReqUserLogout(loginField, requestID);
}
}
}
}