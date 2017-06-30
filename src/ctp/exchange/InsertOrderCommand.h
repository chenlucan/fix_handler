#ifndef __FH_CTP_EXCHANGE_CTP_INSERTORDERCOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_INSERTORDERCOMMAND_H__

#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
class InsertOrderCommand :public fh::ctp::exchange::ApiCommand
{
	public:
		InsertOrderCommand(CThostFtdcTraderApi *api, CThostFtdcInputOrderField *orderField, int &requestID);
		int execute() override;
	private:
		CThostFtdcInputOrderField *orderField;
};
}
}
}
#endif