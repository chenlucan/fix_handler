
#include "zmq_receiver.h"
#include "logger.h"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 1)
        {
            rczg::Logger::Error("Usage: zmq_receiver_test");
            return 1;
        }
        
        rczg::ZmqReceiver r("tcp://localhost:5557");
        r.Start_receive();
    }
    catch (std::exception& e)
    {
        rczg::Logger::Error("Exception: ", e.what());
    }

    return 0;
}

// ./zmq_receiver_test 