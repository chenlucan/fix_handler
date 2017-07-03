#ifndef __FH_CTP_EXCHANGE_CTP_REQQRYTRADCOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_REQQRYTRADCOMMAND_H__

#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
class ReqQryTradeCommand :public fh::ctp::exchange::ApiCommand{
public:
	ReqQryTradeCommand(CThostFtdcTraderApi *api, CThostFtdcQryTradeField *orderField, int &requestID);
	int execute() override;
private:
	CThostFtdcQryTradeField *orderField;
};
}
}
}
#endif