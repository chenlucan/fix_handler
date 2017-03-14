
#include "cme/market/persist/mdp_saver.h"
#include "core/assist/logger.h"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 1)
        {
            LOG_ERROR("Usage: zmq_receiver_test");
            LOG_ERROR("Ex:       zmq_receiver_test");

            return 1;
        }

        fh::cme::market::persist::MdpSaver r;
        r.Start_save();
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./zmq_receiver_test
