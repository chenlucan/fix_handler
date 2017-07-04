#ifndef __FH_CTP_EXCHANGE_CTP_LOGINCOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_LOGINCOMMAND_H__

#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
class LoginCommand:public fh::ctp::exchange::ApiCommand{
public:
	LoginCommand(CThostFtdcTraderApi *api, CThostFtdcReqUserLoginField *loginField, int &requestID);
	int execute() override;
private:
	CThostFtdcReqUserLoginField *loginField;
};
}
}
}
#endif