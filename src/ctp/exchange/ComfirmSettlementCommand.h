#ifndef __FH_CTP_EXCHANGE_CTP_COMFIRMSETTLEMENTCOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_COMFIRMSETTLEMENTCOMMAND_H__

#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
class ComfirmSettlementCommand :public fh::ctp::exchange::ApiCommand{
public:
	ComfirmSettlementCommand(CThostFtdcTraderApi *api, CThostFtdcSettlementInfoConfirmField *comfirmField, int &requestID);
	int execute() override;
private:
	CThostFtdcSettlementInfoConfirmField *comfirmField;
};
}
}
}
#endif