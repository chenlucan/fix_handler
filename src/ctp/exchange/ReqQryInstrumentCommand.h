#ifndef __FH_CTP_EXCHANGE_CTP_REQQRYINSTRUMENTCOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_REQQRYINSTRUMENTCOMMAND_H__

#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
class ReqQryInstrumentCommand :public fh::ctp::exchange::ApiCommand{
public:
	ReqQryInstrumentCommand(CThostFtdcTraderApi *api, CThostFtdcQryInstrumentField *orderField, int &requestID);
	int execute() override;
private:
	CThostFtdcQryInstrumentField *orderField;
};


}
}
}
#endif