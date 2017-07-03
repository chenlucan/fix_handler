#include "ApiCommand.h"
namespace fh
{
namespace ctp
{
namespace exchange
{
	ApiCommand::ApiCommand(int &requestID, CThostFtdcTraderApi *api):requestID(requestID){
		this->api = api;
		requestID++;
	}

	ApiCommand::~ApiCommand(){
		api = nullptr;
	}

}
}
}