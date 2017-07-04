#ifndef __FH_CTP_EXCHANGE_CTP_QUERYPOSITIONCOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_QUERYPOSITIONCOMMAND_H__

#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
class QueryPositionCommand :public fh::ctp::exchange::ApiCommand
{
	public:
		QueryPositionCommand(CThostFtdcTraderApi *api, CThostFtdcQryInvestorPositionField *accountField, int &requestID);
		int execute() override;
	private:
		CThostFtdcQryInvestorPositionField *accountField;
};
}
}
}
#endif