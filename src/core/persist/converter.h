
#ifndef __FH_CORE_PERSIST_CONVERTER_H__
#define __FH_CORE_PERSIST_CONVERTER_H__

#include <map>
#include "core/global.h"

namespace fh
{
namespace core
{
namespace persist
{
    class Converter
    {
        public:
            Converter() {}
            virtual ~Converter() {}

        public:
            // 将原始行情数据转换成解析后播放出的数据（l2, bbo, bid, offer 等）
            virtual std::map<std::string, std::string> Convert(const std::string &message) = 0;

        private:
			DISALLOW_COPY_AND_ASSIGN(Converter);
    };
} // namespace persist
} // namespace core
} // namespace fh

#endif // __FH_CORE_PERSIST_CONVERTER_H__
