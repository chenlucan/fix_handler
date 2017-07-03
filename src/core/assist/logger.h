
#ifndef __FH_CORE_ASSIST_LOGGER_H__
#define __FH_CORE_ASSIST_LOGGER_H__

#include <iostream>
#include <mutex>
#include <thread>
#include "core/global.h"
#include "core/assist/utility.h"

#define __FL__ fh::core::assist::utility::Pad_Right(fh::core::assist::utility::Get_filename(__FILE__)  + "(" + std::to_string(__LINE__) + ")", 30)
#define LOG_TRACE(...) fh::core::assist::Logger::Trace("[", __FL__, "] ", __VA_ARGS__)
#define LOG_DEBUG(...) fh::core::assist::Logger::Debug("[", __FL__, "] ", __VA_ARGS__)
#define LOG_INFO(...) fh::core::assist::Logger::Info("[", __FL__, "] ", __VA_ARGS__)
#define LOG_WARN(...) fh::core::assist::Logger::Warn("[", __FL__, "] ", __VA_ARGS__)
#define LOG_ERROR(...) fh::core::assist::Logger::Error("[", __FL__, "] ", __VA_ARGS__)
#define LOG_FATAL(...) fh::core::assist::Logger::Fatal("[", __FL__, "] ", __VA_ARGS__)

namespace fh
{
namespace core
{
namespace assist
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
                    std::cout << "[" << fh::core::assist::utility::Current_time_str() << "] ";
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
} // namespace assist
} // namespace core 
} // namespace fh

#endif // __FH_CORE_ASSIST_LOGGER_H__
