
#include "core/assist/logger.h"

namespace fh
{
namespace core
{
namespace assist
{
    Logger::Level Logger::m_level = Logger::Level::TRACE;
    std::mutex Logger::m_mutex;
} // namespace assist
} // namespace core
} // namespace fh
