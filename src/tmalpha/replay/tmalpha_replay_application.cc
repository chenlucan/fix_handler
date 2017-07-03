
#include "core/assist/logger.h"
#include "core/book/book_sender.h"
#include "tmalpha/replay/tmalpha_replay_application.h"

namespace fh
{
namespace tmalpha
{
namespace replay
{

    TmalphaReplayApplication::TmalphaReplayApplication(
            const std::string &app_setting_file, const std::string &persist_setting_file)
    : m_listener(nullptr),  m_provider(nullptr), m_simulater(nullptr), m_strategy(nullptr), m_strategy_thread(nullptr),
      m_market(nullptr), m_exchange(nullptr)
    {
        fh::core::assist::Settings app_settings(app_setting_file);
        fh::core::assist::Settings db_settings(persist_setting_file);
        this->Init(app_settings, db_settings);
    }

    TmalphaReplayApplication::~TmalphaReplayApplication()
    {
        delete m_exchange;
        delete m_market;
        delete m_strategy_thread;
        delete m_strategy;
        delete m_simulater;
        delete m_provider;
        delete m_listener;
    }

    bool TmalphaReplayApplication::Start()
    {
        LOG_DEBUG("start replay simulater");

        // 启动行情接受模块
        m_market->Start();
        // 启动交易模块
        m_exchange->Start(this->Get_init_orders());
        // 启动交易指令接受模块
        m_strategy_thread = new std::thread([this]{
            m_strategy->Start_receive();
        });
        m_strategy_thread->detach();
        // 启动模拟回放交易所
        return m_simulater->Start();
    }

    void TmalphaReplayApplication::Join()
    {
        m_simulater->Join();
    }

    void TmalphaReplayApplication::Stop()
    {
        m_market->Stop();
        m_exchange->Stop();
        m_simulater->Stop();
    }

    void TmalphaReplayApplication::Init(
            const fh::core::assist::Settings &app_settings,
            const fh::core::assist::Settings &persist_setting_file)
    {
        std::string market = app_settings.Get("alpha.market");  // 回放这个交易所的数据
        std::string contracts = app_settings.Get("alpha.contracts");  // 回放这些合约的数据
        std::string start_include = app_settings.Get("alpha.start_include");
        std::string end_exclude = app_settings.Get("alpha.end_exclude");
        float speed = std::stof(app_settings.Get("alpha.speed"));
        int trade_rate = std::stoi(app_settings.Get("alpha.trade_rate"));   // 利用这个比率（[0,100]）和实际成交数量来决定一个订单是否成交

        std::string book_receive_url = app_settings.Get("alpha-market.book_url");
        std::string send_url = app_settings.Get("alpha-order.send_url");
        std::string receive_url = app_settings.Get("alpha-order.receive_url");

        // 初期化模拟回放交易所
        m_provider = new ReplayDataProvider(market, contracts, persist_setting_file);
        m_provider->Range(start_include, end_exclude);
        m_simulater = new fh::tmalpha::replay::ReplaySimulater(m_provider, trade_rate);
        m_simulater->Speed(speed);

        // 接受行情数据（由于不需要接受原始数据，所以第一个参数不设置了）
        m_listener = new fh::core::book::BookSender("", book_receive_url);
        m_market = new fh::tmalpha::replay::market::ReplayMarket(m_listener);
        m_market->Set_simulater(m_simulater);

        // 交易模块（从外部接受订单，发送订单应答）
        m_strategy = new fh::core::strategy::StrategyCommunicator(send_url, receive_url);
        m_exchange = new fh::tmalpha::replay::exchange::ReplayExchange(m_strategy);
        m_strategy->Set_exchange(m_exchange);
        m_exchange->Set_simulater(m_simulater);

        LOG_INFO("==== Replay ", market, " data of [", start_include, ", ", end_exclude, ") on speed ", speed);
    }

    std::vector<::pb::ems::Order> TmalphaReplayApplication::Get_init_orders()
    {
        // 模拟器启动时，需要传递初期订单一览，这里先传递一个空的。如果需要的话，可能要从 db 检索
        return std::vector<::pb::ems::Order>();
    }

}   // namespace replay
}   // namespace tmalpha
}   // namespace fh
