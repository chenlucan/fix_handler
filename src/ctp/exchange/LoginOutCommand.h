#ifndef __FH_CTP_EXCHANGE_CTP_LOGINOUTCOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_LOGINOUTCOMMAND_H__

#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
class LoginOutCommand:public fh::ctp::exchange::ApiCommand{
public:
	LoginOutCommand(CThostFtdcTraderApi *api, CThostFtdcUserLogoutField *loginField, int &requestID);
	int execute() override;
private:
	CThostFtdcUserLogoutField *loginField;
};
}
}
}
#endif