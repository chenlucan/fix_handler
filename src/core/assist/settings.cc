
#include <stdexcept>
#include <boost/property_tree/ini_parser.hpp>
#include "core/assist/settings.h"

namespace fh
{
namespace core
{
namespace assist
{

    Settings::Settings(const std::string &setting_file) : m_sections()
    {
        this->Read_settings(setting_file);
    }

    Settings::~Settings()
    {
        // noop
    }

    const std::string &Settings::Get(const std::string &key) const
    {
        try
        {
            auto pos = key.find(".");
            if(pos == std::string::npos) throw std::invalid_argument("setting key[" + key + "] is invalid");
            return m_sections.at(key.substr(0, pos)).at(key.substr(pos+1));
        }
        catch(const std::out_of_range& oor)
        {
            throw std::invalid_argument("setting key[" + key + "] not found");
        }
    }

    const std::unordered_map<std::string, std::string> &Settings::Get_section(const std::string &key) const
    {
        try
        {
            return m_sections.at(key);
        }
        catch(const std::out_of_range& oor)
        {
            throw std::invalid_argument("setting key[" + key + "] not found");
        }
    }

    void Settings::Read_settings(const std::string &setting_file)
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(setting_file, pt);

        for(boost::property_tree::ptree::value_type &p1 : pt)
        {
            // sections
            std::unordered_map<std::string, std::string> seciton_value;
            for(boost::property_tree::ptree::value_type &p2 : p1.second)
            {
                // keys
                seciton_value.insert({p2.first, p1.second.get<std::string>(p2.first)});
            }
            m_sections.insert({p1.first, seciton_value});
        }
    }

} // namespace assist
} // namespace core
} // namespace fh
