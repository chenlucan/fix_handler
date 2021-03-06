#include "core/assist/time_measurer.h"

namespace fh
{
namespace core
{
namespace assist
{

    TimeMeasurer::TimeMeasurer() : m_current()
    {
        this->Restart();
    }

    TimeMeasurer::~TimeMeasurer()
    {
        // noop
    }

    void TimeMeasurer::Restart()
    {
        m_current = std::chrono::high_resolution_clock::now();
    }

    std::uint64_t TimeMeasurer::Elapsed_nanoseconds()
    {
        auto prev = m_current;
        m_current = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(m_current - prev).count();
    }

} // namespace assist
} // namespace core
} // namespace fh
