
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "global.h"
#include "utility.h"


#define __FL__ rczg::utility::Pad_Right(std::string(__FILE__)  + "(" + std::to_string(__LINE__) + ")", 30)
#define LOG_TRACE(...) rczg::Logger::Trace("[", __FL__, "] ", __VA_ARGS__)
#define LOG_DEBUG(...) rczg::Logger::Debug("[", __FL__, "] ", __VA_ARGS__)
#define LOG_INFO(...) rczg::Logger::Info("[", __FL__, "] ", __VA_ARGS__)
#define LOG_WARN(...) rczg::Logger::Warn("[", __FL__, "] ", __VA_ARGS__)
#define LOG_ERROR(...) rczg::Logger::Error("[", __FL__, "] ", __VA_ARGS__)
#define LOG_FATAL(...) rczg::Logger::Fatal("[", __FL__, "] ", __VA_ARGS__)

namespace rczg
{
    class Logger
    {
        public:
            enum class Level 
            {
                TRACE, 
                DEBUG, 
                INFO, 
                WARNING, 
                ERR,
                FATAL
            };

        public:
            static void Set_level(const Level &level)
            {
                Logger::m_level = level;
            }

            template<typename T, typename... Params>
            static void Trace(T first, Params... parameters)
            {
                Logger::Write("TRACE", Logger::Level::TRACE, first, parameters...);
            }
            
            template<typename T, typename... Params>
            static void Debug(T first, Params... parameters)
            {
                Logger::Write("DEBUG", Logger::Level::DEBUG, first, parameters...);
            }

            template<typename T, typename... Params>
            static void Info(T first, Params... parameters)
            {
                Logger::Write("INFO ", Logger::Level::INFO, first, parameters...);
            }
            
            template<typename T, typename... Params>
            static void Warn(T first, Params... parameters)
            {
                Logger::Write("WARN ", Logger::Level::WARNING, first, parameters...);
            }

            template<typename T, typename... Params>
            static void Error(T first, Params... parameters)
            {
                Logger::Write("ERROR", Logger::Level::ERR, first, parameters...);
            }

            template<typename T, typename... Params>
            static void Fatal(T first, Params... parameters)
            {
                Logger::Write("FATAL", Logger::Level::FATAL, first, parameters...);
            }
            
        private:
            template<typename T, typename... Params>
            static void Write(const char* level_str, const Logger::Level &level, T first, Params... parameters)
            {
                if(level >= Logger::m_level)
                {
                	std::lock_guard<std::mutex> lock(m_mutex);
                    std::cout << "[" << rczg::utility::Current_time_str() << "] ";
                    std::cout << "[" << std::hex << std::setfill('0') << std::setw(2) << std::this_thread::get_id() << "] " << std::dec;
                    std::cout << "[" << level_str << "] ";
                    Logger::Write_(first, parameters...);
                }
            }
        
            template<typename T, typename... Params>
            static void Write_(T first, Params... parameters)
            {
                std::cout << first;
                Logger::Write_(parameters...);
            }

            static void Write_()
            {
                std::cout << std::endl;
            }
            
        private:
            static Level m_level;
            static std::mutex m_mutex;
            
        private:
            Logger()
            {
                // noop
            }

            ~Logger()
            {
                // noop
            }
            
        private:
            DISALLOW_COPY_AND_ASSIGN(Logger);            
    };
}

#endif // __LOGGER_H__
