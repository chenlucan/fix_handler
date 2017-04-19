
#include <chrono>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <quickfix/Session.h>

#include "core/assist/logger.h"

#include "cme/market/message/message_utility.h"

#include "mut_common.h"

namespace fh
{
namespace core
{
namespace assist
{
namespace common
{
    void getAbsolutePath(std::string &path)
    {
        char current_absolute_path[256];
        //获取当前程序绝对路径
        int cnt = readlink("/proc/self/exe", current_absolute_path, 256);
        if (cnt < 0 || cnt >= 256)
        {
            printf("***Error***\n");
            return;
        }
        //获取当前目录绝对路径，即去掉程序名
        int i;
        for (i = cnt; i >=0; --i)
        {
            if (current_absolute_path[i] == '/')
            {
                current_absolute_path[i+1] = '\0';
                break;
            }
        }
        path = current_absolute_path;
        printf("--- current absolute path:[%s] ---\n", path.c_str());
    }
    
    void show_message(const std::string &hex_message)
    {
        std::vector<std::string> tokens;
        std::istringstream iss(hex_message);
        std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

        std::vector<char> bytes;
        int index = 0;
        std::ostringstream byte_line;
        std::for_each(tokens.begin(), tokens.end(), [&bytes, &index, &byte_line](std::string &s){
            boost::trim_right(s);
            boost::trim_left(s);
            if(s != "")
            {
                char x = (char) strtol(s.c_str(), nullptr, 16);
                bytes.push_back(x);

                index ++;
                std::string dec;
                if(index % 40 == 0) dec = "\n";
                else if(index % 20 == 0) dec = "    ";
                else if(index % 10 == 0) dec = "  ";
                else dec = " ";
                byte_line << s << dec;
            }
        });

        LOG_INFO("hex message: size=", bytes.size(), "\n", byte_line.str());

        std::vector<fh::cme::market::message::MdpMessage> message;
        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(bytes.data(), bytes.size(), message);

        LOG_INFO("seq=", seq, ", message count=", message.size());

        std::for_each(message.cbegin(), message.cend(), [](const fh::cme::market::message::MdpMessage &m){
            std::string  s = m.Serialize();
            LOG_INFO(s);
        });
    }
    
} // namespace utility
} // namespace assist
} // namespace core
} // namespace fh
