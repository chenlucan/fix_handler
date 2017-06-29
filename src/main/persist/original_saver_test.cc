
#include "core/persist/original_saver.h"
#include "core/assist/logger.h"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 1)
        {
            LOG_ERROR("Usage: original_saver_test");
            LOG_ERROR("Ex:       original_saver_test");

            return 1;
        }

        fh::core::persist::OriginalSaver r;
        r.Start();
    }
    catch (std::exception& e)
    {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}

// ./original_saver_test
