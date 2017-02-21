
#ifndef __APP_SETTINGS_H__
#define __APP_SETTINGS_H__

#include "global.h"

namespace rczg
{
    class AppSettings
    {
        public:
            explicit AppSettings(const std::string &app_setting_file = "settings.ini");
            virtual ~AppSettings();
            
        public:
            const std::pair<std::string, std::string> Get_auth() const;
            const std::pair<std::string, std::string> Get_data_save_url() const;
            
        private:
            void Read_settings(const std::string &app_setting_file);

        private:
            std::unordered_map<std::string, std::string> m_settings;
            
        private:
            DISALLOW_COPY_AND_ASSIGN(AppSettings);
    };
}

#endif // __APP_SETTINGS_H__
