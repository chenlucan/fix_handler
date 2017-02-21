#ifndef __TIME_MEASURER_H__
#define __TIME_MEASURER_H__

#include "global.h"

namespace rczg
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
}

#endif // __TIME_MEASURER_H__
