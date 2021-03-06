#ifndef __FH_CTP_EXCHANGE_CTP_COMMANDQUEUE_H__
#define __FH_CTP_EXCHANGE_CTP_COMMANDQUEUE_H__

#include <thread>
#include <chrono>
#include <queue>
#include <memory>
#include <mutex>
#include "ApiCommand.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
//指令队列
class CommandQueue
{
	public:
		CommandQueue():working(true)
		{
			
		}
		~CommandQueue();
		void addCommand(std::shared_ptr<fh::ctp::exchange::ApiCommand> newCommand);
	private:
		std::queue<std::shared_ptr<fh::ctp::exchange::ApiCommand>> commandQueue;
		std::mutex queueMutex;
		bool working;				//工作状态
	public:
		void run();
};
}
}
}
#endif