
#ifndef __FH_CORE_ASSIST_SETTINGS_H__
#define __FH_CORE_ASSIST_SETTINGS_H__

#include <string>
#include <unordered_map>
#include "core/global.h"

namespace fh
{
namespace core
{
namespace assist
{
    class Settings
    {
        public:
            explicit Settings(const std::string &setting_file);
            virtual ~Settings();

        public:
            std::string Get(const std::string &key) const;

        private:
            void Read_settings(const std::string &app_setting_file);

        private:
            std::unordered_map<std::string, std::string> m_settings;

        private:
            DISALLOW_COPY_AND_ASSIGN(Settings);
    };
} // namespace assist
} // namespace core
} // namespace fh

#endif // __FH_CORE_ASSIST_SETTINGS_H__
