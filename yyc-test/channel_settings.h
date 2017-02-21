
#ifndef __CHANNEL_SETTINGS_H__
#define __CHANNEL_SETTINGS_H__

#include "global.h"
#include "channel.h"

namespace rczg
{
    class ChannelSettings
    {
        public:
            explicit ChannelSettings(const std::string &channel_setting_file = "config.xml");
            virtual ~ChannelSettings();
            
        public:
            const rczg::setting::Channel Get_channel(const std::string &channel_id) const;
            
        private:
            static const std::unordered_map<std::string, rczg::setting::FeedType> FEED_TYPES;
            static const std::unordered_map<std::string, rczg::setting::Protocol> PROTOCOLS;
            static const std::unordered_map<std::string, rczg::setting::Feed> FEEDS;
            
        private:
            void Read_channels(const std::string &channel_setting_file);
            std::vector<rczg::setting::Product> Read_channel_products(boost::property_tree::ptree::value_type &channel);
            std::vector<rczg::setting::Connection> Read_channel_connections(boost::property_tree::ptree::value_type &channel);
            template <typename T> 
            static T Convert(const std::string &value, const std::unordered_map<std::string, T> &all_values);

        private:
            std::unordered_map<std::string, rczg::setting::Channel> m_channels;
            
        private:
            DISALLOW_COPY_AND_ASSIGN(ChannelSettings);
    };
}

#endif // __CHANNEL_SETTINGS_H__
