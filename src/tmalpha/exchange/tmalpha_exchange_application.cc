
#include <thread>
#include "core/assist/logger.h"
#include "tmalpha/exchange/tmalpha_exchange_application.h"

namespace fh
{
namespace tmalpha
{
namespace exchange
{

    TmalphaExchangeApplication::TmalphaExchangeApplication(const std::string &app_setting_file)
    : m_strategy(nullptr), m_exchange_simulater(nullptr), m_strategy_thread(nullptr)
    {
        this->Init(app_setting_file);
    }

    TmalphaExchangeApplication::~TmalphaExchangeApplication()
    {
        delete m_exchange_simulater;
        delete m_strategy;
        delete m_strategy_thread;
    }

    void TmalphaExchangeApplication::Init(const std::string &app_setting_file)
    {
        fh::core::assist::Settings app_settings(app_setting_file);
        std::string send_url = app_settings.Get("alpha-order.send_url");
        std::string receive_url = app_settings.Get("alpha-order.receive_url");
        m_strategy = new fh::core::strategy::StrategyCommunicator(send_url, receive_url);
        m_exchange_simulater = new ExchangeSimulater(m_strategy);
        m_strategy->Set_exchange(m_exchange_simulater);
    }

    bool TmalphaExchangeApplication::Start()
    {
        LOG_DEBUG("start thread to receive order request");
        m_strategy_thread = new std::thread([this]{
            m_strategy->Start_receive();
        });
        m_strategy_thread->detach();

        LOG_DEBUG("start exchange simulater");
        return m_exchange_simulater->Start(this->Get_init_orders());
    }

    void TmalphaExchangeApplication::Join()
    {
        m_exchange_simulater->Join();
    }

    void TmalphaExchangeApplication::Stop()
    {
        m_exchange_simulater->Stop();
    }

    std::vector<::pb::ems::Order> TmalphaExchangeApplication::Get_init_orders()
    {
        // 模拟器启动时，需要传递初期订单一览，这里先传递一个空的。如果需要的话，可能要从 db 检索
        return std::vector<::pb::ems::Order>();
    }

} // namespace exchange
} // namespace tmalpha
} // namespace fh
