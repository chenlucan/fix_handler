#ifndef __FH_CTP_EXCHANGE_CTP_QUERYORDERCOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_QUERYORDERCOMMAND_H__

#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
class QueryOrderCommand :public fh::ctp::exchange::ApiCommand{
public:
	QueryOrderCommand(CThostFtdcTraderApi *api, CThostFtdcQryOrderField *orderField, int &requestID);
	int execute() override;
private:
	CThostFtdcQryOrderField *orderField;
};
}
}
}
#endif