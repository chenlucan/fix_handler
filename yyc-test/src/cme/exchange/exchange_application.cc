
#include <thread>
#include "core/assist/time_measurer.h"
#include "cme/exchange/exchange_application.h"
#include "cme/exchange/exchange_settings.h"
#include "core/assist/logger.h"
#include "cme/exchange/strategy_communicator.h"
#include "cme/exchange/globex_communicator.h"

namespace fh
{
namespace cme
{
namespace exchange
{

    ExchangeApplication::ExchangeApplication(
            bool is_week_begin,
            const std::string &fix_setting_file,
            const std::string &app_setting_file)
    : m_strategy(nullptr), m_globex(nullptr)
    {
        this->Initial_application(fix_setting_file, app_setting_file, is_week_begin);
    }

    ExchangeApplication::~ExchangeApplication()
    {
        delete m_strategy;
        delete m_globex;
    }

    void ExchangeApplication::Initial_application(const std::string &fix_setting_file, const std::string &app_setting_file, bool is_week_begin)
    {
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        m_strategy = new StrategyCommunicator(url.first, url.second);
        m_globex = new GlobexCommunicator(fix_setting_file, app_settings, is_week_begin);
    }

    void ExchangeApplication::Start()
    {
        // 启动一个线程，用来从策略模块接受交易指令，并将交易结果发送回去
        LOG_DEBUG("start strategy thread");
        std::thread strategy_listener([this]{
            m_strategy->Start_receive(std::bind(&ExchangeApplication::On_from_strategy, this, std::placeholders::_1, std::placeholders::_2));
        });
        strategy_listener.detach();

        // 启动一个线程，用来将从策略模块接受到的交易指令发送到 CME，然后将 CME 返回的交易结果返回给策略模块
        LOG_DEBUG("start globex thread");
        std::thread globex_listener([this]{
            m_globex->Start(std::bind(&ExchangeApplication::On_from_globex, this, std::placeholders::_1, std::placeholders::_2));
        });
        globex_listener.detach();
    }

    void ExchangeApplication::Stop()
    {
        m_globex->Stop();
    }

    void ExchangeApplication::On_from_strategy(char *data, size_t size)
    {
        // 收到策略模块的信息后，将对应的指令发送到交易所
        LOG_INFO("received from stategy: ", std::string(data, size));

        fh::core::assist::TimeMeasurer t;
        bool result = m_globex->Order_request(data, size);
        LOG_INFO("order processed:", result, ", used: ", t.Elapsed_nanoseconds(), "ns");
    }

    void ExchangeApplication::On_from_globex(char *data, size_t size)
    {
        // 接受到交易所返回的信息后，转发给策略模块
        LOG_INFO("send to stategy: ", std::string(data, size));
        m_strategy->Send(data, size);
    }
} // namespace exchange
} // namespace cme
} // namespace fh
