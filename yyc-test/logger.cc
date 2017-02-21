
#include "logger.h"

namespace rczg
{
    Logger::Level Logger::m_level = Logger::Level::TRACE;
    std::mutex Logger::m_mutex;
}
