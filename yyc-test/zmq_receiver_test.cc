
#include "zmq_receiver.h"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 1)
        {
            std::cerr << "Usage: zmq_receiver_test \n";
            std::cerr << "Ex:    zmq_receiver_test \n";
            
            return 1;
        }
        
        rczg::ZmqReceiver r("tcp://localhost:5557");
        r.Start_receive();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

// ./zmq_receiver_test 