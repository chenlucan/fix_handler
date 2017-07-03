#include "CommandQueue.h"
using std::lock_guard;
using std::mutex;

namespace fh
{
namespace ctp
{
namespace exchange
{
CommandQueue::~CommandQueue(){
	working = false;		
}

void CommandQueue::addCommand(std::shared_ptr<ApiCommand> newCommand){
	lock_guard<mutex> lock(queueMutex);
	commandQueue.push(newCommand);
}

//一秒之内不能连续发送指令
void CommandQueue::run(){
	std::thread t([this]{
	while (working){
		if (!commandQueue.empty()){
			int NOT_ZERO = 100;
			int result = NOT_ZERO;
			{
				lock_guard<mutex> lock(queueMutex);
				auto command = commandQueue.front();
				result = command->execute();
				if (result == 0){
					//result为0意味着发送指令成功
					commandQueue.pop();
				}
			}
			//如果发送了指令休息一秒，把这段代码放出来是为了释放锁，使得沉睡的一秒期间其它代码可以访问临界资源
			if (result == 0){
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
	}});
	t.detach();
}


}
}
}