
#include <thread>
#include "core/assist/logger.h"
#include "core/persist/market_data_provider.h"
#include "tmalpha/market/cme_data_consumer.h"
#include "tmalpha/exchange/tmalpha_exchange_application.h"

namespace fh
{
namespace tmalpha
{
namespace exchange
{

    TmalphaExchangeApplication::TmalphaExchangeApplication(const std::string &app_setting_file, const std::string &persist_setting_file)
    :  m_market(nullptr), m_market_listener(nullptr), m_provider(nullptr), m_consumer(nullptr),
       m_strategy(nullptr), m_exchange_simulater(nullptr), m_strategy_thread(nullptr)
    {
        this->Init(app_setting_file, persist_setting_file);
    }

    TmalphaExchangeApplication::~TmalphaExchangeApplication()
    {
        delete m_market;
        delete m_exchange_simulater;
        delete m_strategy;
        delete m_strategy_thread;
        delete m_consumer;
        delete m_provider;
        delete m_market_listener;
    }

    void TmalphaExchangeApplication::Init(const std::string &app_setting_file, const std::string &persist_setting_file)
    {
        fh::core::assist::Settings app_settings(app_setting_file);
        fh::core::assist::Settings db_settings(persist_setting_file);

        std::string market = app_settings.Get("alpha.market");
        std::string start_include = app_settings.Get("alpha.start_include");
        std::string end_exclude = app_settings.Get("alpha.end_exclude");
        float speed = std::stof(app_settings.Get("alpha.speed"));
        int trade_rate = std::stoi(app_settings.Get("alpha.trade_rate"));   // 利用这个比率（[0,100]）和实际成交数量来决定一个订单是否成交

        // 初期化数据提供者
        m_provider = new fh::core::persist::MarketDataProvider(market, db_settings);
        m_provider->Range(start_include, end_exclude);

        // 初期化数据处理者
        if(market == "CME")
        {
            m_consumer = new fh::tmalpha::market::CmeDataConsumer();
        }
        else
        {
            LOG_WARN("unknow market: ", market);
            throw std::invalid_argument("unknow market");
        }

        // 初期化数据接受者（由于不需要对外发送行情数据以及原始数据，所以参数都不设置了）
        m_market_listener = new fh::tmalpha::exchange::MarketReplayListener("", "");

        // 初期化重放控制模块
        m_market = new fh::tmalpha::market::MarketSimulater(m_market_listener, m_provider, m_consumer);
        m_market->Speed(speed);

        LOG_INFO("==== Replay ", market, " data of [", start_include, ", ", end_exclude, ") on speed ", speed);

        std::string send_url = app_settings.Get("alpha-order.send_url");
        std::string receive_url = app_settings.Get("alpha-order.receive_url");
        m_strategy = new fh::core::strategy::StrategyCommunicator(send_url, receive_url);
        m_exchange_simulater = new ExchangeSimulater(m_market, m_strategy, trade_rate);
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
