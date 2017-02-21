
#include "mdp_receiver.h"
#include "book_receiver.h"
#include "logger.h"

int main(int argc, char* argv[])
{
    try
    {
    	if (argc != 3 || (strcmp(argv[1], "-m") != 0 && strcmp(argv[1], "-b") != 0))
		{
			LOG_ERROR("Usage: zmq_receiver_test -m|-b <receive_port>");
			LOG_ERROR("Ex:       zmq_receiver_test -m 5557");

			return 1;
		}

		std::string host = "tcp://localhost:";

		rczg::ZmqReceiver *r;
		if(strcmp(argv[1], "-m") == 0)
		{
			r = new rczg::MdpReceiver(host.append(argv[2]).data());
		}
		else
		{
			r = new rczg::BookReceiver(host.append(argv[2]).data());
		}

		r->Start_receive();
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./zmq_receiver_test -m 5557
// ./zmq_receiver_test -b 5558
