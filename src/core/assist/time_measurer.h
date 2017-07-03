#ifndef __FH_CORE_ASSIST_TIME_MEASURER_H__
#define __FH_CORE_ASSIST_TIME_MEASURER_H__

#include <chrono>
#include "core/global.h"

namespace fh
{
namespace core
{
namespace assist
{
    class TimeMeasurer
    {
        public:
            TimeMeasurer();
            virtual ~TimeMeasurer();

        public:
            void Restart();
            std::uint64_t Elapsed_nanoseconds();

        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> m_current;

        private:
            DISALLOW_COPY_AND_ASSIGN(TimeMeasurer);
    };
} // namespace assist
} // namespace core
} // namespace fh

#endif // __FH_CORE_ASSIST_TIME_MEASURER_H__
