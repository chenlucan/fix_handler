
#include "cme/market/setting/channel_settings.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace setting
{

    const std::unordered_map<std::string, fh::cme::market::setting::FeedType> ChannelSettings::FEED_TYPES =
    {
        {"H", fh::cme::market::setting::FeedType::H},
        {"I", fh::cme::market::setting::FeedType::I},
        {"N", fh::cme::market::setting::FeedType::N},    
        {"S", fh::cme::market::setting::FeedType::S},
        {"", fh::cme::market::setting::FeedType::UNKNOW}
    };

    const std::unordered_map<std::string, fh::cme::market::setting::Protocol> ChannelSettings::PROTOCOLS =
    {
        {"TCP/IP", fh::cme::market::setting::Protocol::TCP},
        {"UDP/IP", fh::cme::market::setting::Protocol::UDP},
        {"", fh::cme::market::setting::Protocol::UNKNOW}
    };

    const std::unordered_map<std::string, fh::cme::market::setting::Feed> ChannelSettings::FEEDS =
    {
        {"A", fh::cme::market::setting::Feed::A},
        {"B", fh::cme::market::setting::Feed::B},
        {"", fh::cme::market::setting::Feed::UNKNOW}
    };

    ChannelSettings::ChannelSettings(const std::string &channel_setting_file)
    {
        this->Read_channels(channel_setting_file);
    }

    ChannelSettings::~ChannelSettings()
    {
        // noop
    }

    const fh::cme::market::setting::Channel ChannelSettings::Get_channel(const std::string &channel_id) const
    {
        return m_channels.at(channel_id);
    }

    void ChannelSettings::Read_channels(const std::string &channel_setting_file)
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_xml(channel_setting_file, pt, boost::property_tree::xml_parser::trim_whitespace);

        for(boost::property_tree::ptree::value_type &channel : pt.get_child("configuration"))
        {
            if(channel.first == "<xmlattr>")
            {
                // it's configuration's attribute
                continue;
            }

            auto id = channel.second.get<std::string>("<xmlattr>.id");  
            auto label = channel.second.get<std::string>("<xmlattr>.label");  
            auto products = this->Read_channel_products(channel);
            auto connections = this->Read_channel_connections(channel);

            m_channels.insert({id, {id, label, products, connections}});

            //LOG_DEBUG("read channel end, id=", id);
        }   

        LOG_INFO("channels read ok, size=", m_channels.size());
    }

    std::vector<fh::cme::market::setting::Product> ChannelSettings::Read_channel_products(boost::property_tree::ptree::value_type &channel)
    {
        std::vector<fh::cme::market::setting::Product> products;
        for(boost::property_tree::ptree::value_type &p : channel.second.get_child("products"))
        {
            auto pcode = p.second.get<std::string>("<xmlattr>.code");
            auto gcode = p.second.get_child("group").get<std::string>("<xmlattr>.code");

            fh::cme::market::setting::Group group = { gcode };
            fh::cme::market::setting::Product product = { pcode, group };

            products.push_back(product);
        }
        return products;
    }

    std::vector<fh::cme::market::setting::Connection> ChannelSettings::Read_channel_connections(boost::property_tree::ptree::value_type &channel)
    {
        std::vector<fh::cme::market::setting::Connection> connections;
        for(boost::property_tree::ptree::value_type &c : channel.second.get_child("connections"))
        {
            auto cid = c.second.get<std::string>("<xmlattr>.id");  
            auto type = c.second.get_child("type").get<std::string>("<xmlattr>.feed-type");  
            auto type_des = c.second.get<std::string>("type");  
            auto protocol = c.second.get<std::string>("protocol");  
            auto ip = c.second.get<std::string>("ip", "127.0.0.1");
            auto host_ip = c.second.get<std::string>("host-ip");  
            auto port = c.second.get<std::uint16_t>("port");  
            auto feed = c.second.get<std::string>("feed");

            fh::cme::market::setting::Connection connection = { 
                cid,
                ChannelSettings::Convert(type, ChannelSettings::FEED_TYPES),
                type_des,
                ChannelSettings::Convert(protocol, ChannelSettings::PROTOCOLS),
                boost::asio::ip::address::from_string(ip == "" ? "127.0.0.1" : ip),
                boost::asio::ip::address::from_string(host_ip),
                port,
                ChannelSettings::Convert(feed, ChannelSettings::FEEDS),
            };

            connections.push_back(connection);
            //LOG_INFO("channel connections read end, id= ", cid);
        }
        return connections;
    }

    template <typename T> 
    T ChannelSettings::Convert(const std::string &value, const std::unordered_map<std::string, T> &all_values)
    {
        auto index = all_values.find(value);
        if(index == all_values.end()) return all_values.at("");
        return all_values.at(value);
    }

} // namespace setting
} // namespace market
} // namespace cme
} // namespace fh
