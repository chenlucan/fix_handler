
#include "channel_settings.h"

namespace rczg
{
    
    const std::unordered_map<std::string, rczg::setting::FeedType> ChannelSettings::FEED_TYPES =
    {
        {"H", rczg::setting::FeedType::H},
        {"I", rczg::setting::FeedType::I},
        {"N", rczg::setting::FeedType::N},    
        {"S", rczg::setting::FeedType::S}
    };
    
    const std::unordered_map<std::string, rczg::setting::Protocol> ChannelSettings::PROTOCOLS =
    {
        {"TCP/IP", rczg::setting::Protocol::TCP},
        {"UDP/IP", rczg::setting::Protocol::UDP}
    };
    
    const std::unordered_map<std::string, rczg::setting::Feed> ChannelSettings::FEEDS =
    {
        {"A", rczg::setting::Feed::A},
        {"B", rczg::setting::Feed::B}
    };

    ChannelSettings::ChannelSettings(const std::string &channel_setting_file)
    {
        this->Read_channels(channel_setting_file);
    }
    
    ChannelSettings::~ChannelSettings()
    {
        // noop
    }
    
    const rczg::setting::Channel ChannelSettings::Get_channel(const std::string &channel_id) const
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
        }   
    }

    std::vector<rczg::setting::Product> ChannelSettings::Read_channel_products(boost::property_tree::ptree::value_type &channel)
    {
        std::vector<rczg::setting::Product> products;
        for(boost::property_tree::ptree::value_type &p : channel.second.get_child("products"))
        {
            auto pcode = p.second.get<std::string>("<xmlattr>.code");  
            auto gcode = p.second.get_child("group").get<std::string>("<xmlattr>.code");  
            
            rczg::setting::Group group = { gcode };
            rczg::setting::Product product = { pcode, group };
            
            products.push_back(product);
        }
        return products;
    }
    
    std::vector<rczg::setting::Connection> ChannelSettings::Read_channel_connections(boost::property_tree::ptree::value_type &channel)
    {
        std::vector<rczg::setting::Connection> connections;
        for(boost::property_tree::ptree::value_type &c : channel.second.get_child("connections"))
        {
            auto cid = c.second.get<std::string>("<xmlattr>.id");  
            auto type = c.second.get_child("type").get<std::string>("<xmlattr>.feed-type");  
            auto type_des = c.second.get<std::string>("type");  
            auto protocol = c.second.get<std::string>("protocol");  
            auto ip = c.second.get<std::string>("ip", "0.0.0.0");  
            auto host_ip = c.second.get<std::string>("host-ip");  
            auto port = c.second.get<std::uint16_t>("port");  
            auto feed = c.second.get<std::string>("feed");  

            rczg::setting::Connection connection = { 
                cid,
				ChannelSettings::Convert(type, ChannelSettings::FEED_TYPES),
                type_des,
				ChannelSettings::Convert(protocol, ChannelSettings::PROTOCOLS),
                boost::asio::ip::address::from_string(ip),
                boost::asio::ip::address::from_string(host_ip),
                port,
				ChannelSettings::Convert(feed, ChannelSettings::FEEDS),
            };

            connections.push_back(connection);
        }
        return connections;
    }
    
    template <typename T> 
    T ChannelSettings::Convert(const std::string &value, const std::unordered_map<std::string, T> &all_values)
    {
        return all_values.at(value);
    }

}

