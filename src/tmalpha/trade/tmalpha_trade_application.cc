
#include <thread>
#include "core/assist/logger.h"
#include "core/book/book_sender.h"
#include "tmalpha/trade/trade_algorithm_simple.h"
#include "tmalpha/trade/tmalpha_trade_application.h"

namespace fh
{
namespace tmalpha
{
namespace trade
{

    TmalphaTradeApplication::TmalphaTradeApplication(const std::string &app_setting_file)
    :  m_market_listener(nullptr), m_strategy(nullptr), m_strategy_thread(nullptr),
       m_algorithm(nullptr), m_trade_simulater(nullptr),
       m_market(nullptr), m_exchange(nullptr)
    {
        this->Init(app_setting_file);
    }

    TmalphaTradeApplication::~TmalphaTradeApplication()
    {
        delete m_exchange;
        delete m_market;
        delete m_trade_simulater;
        delete m_algorithm;
        delete m_strategy_thread;
        delete m_strategy;
        delete m_market_listener;
    }

    void TmalphaTradeApplication::Init(const std::string &app_setting_file)
    {
        fh::core::assist::Settings app_settings(app_setting_file);
        std::string book_receive_url = app_settings.Get("alpha-market.book_url");
        std::string send_url = app_settings.Get("alpha-order.send_url");
        std::string receive_url = app_settings.Get("alpha-order.receive_url");
        const std::unordered_map<std::string, std::string> &contracts = app_settings.Get_section("alpha-contract");

        // 初期化模拟交易所
        m_trade_simulater = new fh::tmalpha::trade::TradeSimulater();
        // 交易算法
        m_algorithm = new TradeAlgorithmSimple();
        // 加载合约
        m_trade_simulater->Load_contracts(contracts);
        // 加载交易匹配算法
        m_trade_simulater->Load_match_algorithm(m_algorithm);

        // 接受行情数据（由于不需要接受原始数据，所以第一个参数不设置了）
        m_market_listener = new fh::core::book::BookSender("", book_receive_url);
        m_market = new fh::tmalpha::trade::market::SimulaterMarket(m_market_listener);
        m_market->Set_simulater(m_trade_simulater);

        // 交易模块（从外部接受订单，发送订单应答）
        m_strategy = new fh::core::strategy::StrategyCommunicator(send_url, receive_url);
        m_exchange = new fh::tmalpha::trade::exchange::SimulaterExchange(m_strategy);
        m_strategy->Set_exchange(m_exchange);
        m_exchange->Set_simulater(m_trade_simulater);
    }

    bool TmalphaTradeApplication::Start()
    {
        LOG_DEBUG("start trade simulater");

        // 启动行情接受模块
        m_market->Start();
        // 启动交易模块
        m_exchange->Start(this->Get_init_orders());
        // 启动交易指令接受模块
        m_strategy_thread = new std::thread([this]{
            m_strategy->Start_receive();
        });
        m_strategy_thread->detach();
        // 启动模拟交易所
        return m_trade_simulater->Start();
    }

    void TmalphaTradeApplication::Stop()
    {
        m_market->Stop();
        m_exchange->Stop();
        m_trade_simulater->Stop();
    }

    std::vector<::pb::ems::Order> TmalphaTradeApplication::Get_init_orders()
    {
        // 模拟器启动时，需要传递初期订单一览，这里先传递一个空的。如果需要的话，可能要从 db 检索
        return std::vector<::pb::ems::Order>();
    }

} // namespace trade
} // namespace tmalpha
} // namespace fh
