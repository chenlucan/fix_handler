#ifndef __FH_CTP_EXCHANGE_CTP_APICOMMAND_H__
#define __FH_CTP_EXCHANGE_CTP_APICOMMAND_H__

#include "ThostFtdcTraderApi.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
//封装api命令的接口
class ApiCommand
{
	public:
		virtual ~ApiCommand();
		virtual int execute() = 0;
	protected:
		ApiCommand(int &requestID, CThostFtdcTraderApi *api);
		int &requestID;
		CThostFtdcTraderApi *api;
};

}
}
}

#endif