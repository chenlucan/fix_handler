
#ifndef __FH_CME_MARKET_SETTING_CHANNEL_SETTINGS_H__
#define __FH_CME_MARKET_SETTING_CHANNEL_SETTINGS_H__

#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "core/global.h"
#include "cme/market/setting/channel.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace setting
{
    class ChannelSettings
    {
        public:
            explicit ChannelSettings(const std::string &channel_setting_file);
            virtual ~ChannelSettings();

        public:
            boost::optional<const fh::cme::market::setting::Channel> Get_channel(const std::string &channel_id) const;
            std::vector<fh::cme::market::setting::Channel> All_channels() const;

        private:
            static const std::unordered_map<std::string, fh::cme::market::setting::FeedType> FEED_TYPES;
            static const std::unordered_map<std::string, fh::cme::market::setting::Protocol> PROTOCOLS;
            static const std::unordered_map<std::string, fh::cme::market::setting::Feed> FEEDS;

        private:
            void Read_channels(const std::string &channel_setting_file);
            std::vector<fh::cme::market::setting::Product> Read_channel_products(boost::property_tree::ptree::value_type &channel);
            std::vector<fh::cme::market::setting::Connection> Read_channel_connections(boost::property_tree::ptree::value_type &channel);
            template <typename T> 
            static T Convert(const std::string &value, const std::unordered_map<std::string, T> &all_values);

        private:
            std::unordered_map<std::string, fh::cme::market::setting::Channel> m_channels;

        private:
            DISALLOW_COPY_AND_ASSIGN(ChannelSettings);
    };
} // namespace setting
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_SETTING_CHANNEL_SETTINGS_H__
