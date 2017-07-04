#ifndef __FH_CTP_EXCHANGE_CTP_WITHDRAWORDERCOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_WITHDRAWORDERCOMMAND_H__

#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
class WithdrawOrderCommand :public fh::ctp::exchange::ApiCommand{
public:
	WithdrawOrderCommand(CThostFtdcTraderApi *api, CThostFtdcInputOrderActionField *orderField, int &requestID);
	int execute() override;
private:
	CThostFtdcInputOrderActionField *orderField;
};
}
}
}
#endif