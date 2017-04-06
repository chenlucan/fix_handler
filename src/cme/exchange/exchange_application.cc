
#include <thread>
#include "cme/exchange/exchange_application.h"
#include "cme/exchange/exchange_settings.h"
#include "core/assist/logger.h"

namespace fh
{
namespace cme
{
namespace exchange
{

    ExchangeApplication::ExchangeApplication(
            const std::string &fix_setting_file,
            const std::string &app_setting_file)
    : m_strategy(nullptr), m_globex(nullptr)
    {
        this->Initial_application(fix_setting_file, app_setting_file);
    }

    ExchangeApplication::~ExchangeApplication()
    {
        delete m_strategy;
        delete m_globex;
    }

    void ExchangeApplication::Initial_application(const std::string &fix_setting_file, const std::string &app_setting_file)
    {
        fh::cme::exchange::ExchangeSettings app_settings(app_setting_file);
        std::pair<std::string, std::string> url = app_settings.Get_strategy_url();
        m_strategy = new StrategyCommunicator(url.first, url.second);
        m_globex = new GlobexCommunicator(m_strategy, fix_setting_file, app_settings);
        m_strategy->Set_exchange(m_globex);
    }

    bool ExchangeApplication::Start()
    {
        // 启动一个线程，用来从策略模块接受交易指令，并将交易结果发送回去
        LOG_DEBUG("start strategy thread");
        std::thread strategy_listener([this]{
            m_strategy->Start_receive();
        });
        strategy_listener.detach();

        // 启动 globex 模块，用来将从策略模块接受到的交易指令发送到 CME，然后将 CME 返回的交易结果返回给策略模块
        LOG_DEBUG("start globex");
        return m_globex->Start(this->Get_init_orders());
    }

    void ExchangeApplication::Stop()
    {
        m_globex->Stop();
    }

    std::vector<::pb::ems::Order> ExchangeApplication::Get_init_orders()
    {
        // 初期启动时，获取需要查询状态的订单一览。将这些订单的状态都查询出来以后，启动才能结束
        return std::vector<::pb::ems::Order>();
    }

} // namespace exchange
} // namespace cme
} // namespace fh
