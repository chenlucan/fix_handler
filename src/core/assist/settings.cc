
#include <boost/property_tree/ini_parser.hpp>
#include "core/assist/settings.h"

namespace fh
{
namespace core
{
namespace assist
{

    Settings::Settings(const std::string &setting_file)
    {
        this->Read_settings(setting_file);
    }

    Settings::~Settings()
    {
        // noop
    }

    std::string Settings::Get(const std::string &key) const
    {
        return m_settings.at(key);
    }

    void Settings::Read_settings(const std::string &setting_file)
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(setting_file, pt);

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

} // namespace assist
} // namespace core
} // namespace fh
