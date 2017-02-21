
#include "app_settings.h"

namespace rczg
{
    
	AppSettings::AppSettings(const std::string &app_setting_file)
    {
        this->Read_settings(app_setting_file);
    }
    
    AppSettings::~AppSettings()
    {
        // noop
    }
    
    const std::pair<std::string, std::string> AppSettings::Get_auth() const
    {
        return {m_settings.at("replayer.username"), m_settings.at("replayer.password")};
    }

    const std::pair<std::string, std::string> AppSettings::Get_data_save_url() const
    {
    	return {m_settings.at("zeromq.org_url"), m_settings.at("zeromq.book_url")};
    }

    void AppSettings::Read_settings(const std::string &app_setting_file)
    {
    	boost::property_tree::ptree pt;
    	boost::property_tree::ini_parser::read_ini(app_setting_file, pt);

    	// sections
    	for(boost::property_tree::ptree::value_type &p1 : pt)
    	{
    		auto section = p1.first;
    		// keys
    		for(boost::property_tree::ptree::value_type &p2 : p1.second)
    		{
        		auto key = section + "." + p2.first;
        		auto value = p1.second.get<std::string>(p2.first);

        		m_settings.insert({key, value});
    		}
    	}
    }

}

