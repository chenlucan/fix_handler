
#include "core/assist/logger.h"
#include "core/book/book_sender.h"
#include "core/persist/market_data_provider.h"
#include "tmalpha/market/cme_data_consumer.h"
#include "tmalpha/market/market_simulater.h"
#include "tmalpha/market/tmalpha_market_application.h"

namespace fh
{
namespace tmalpha
{
namespace market
{

    TmalphaMarketApplication::TmalphaMarketApplication(
            const std::string &app_setting_file, const std::string &persist_setting_file)
    : m_provider(nullptr), m_consumer(nullptr), m_listener(nullptr),  m_simulater(nullptr)
    {
        fh::core::assist::Settings app_settings(app_setting_file);
        fh::core::assist::Settings db_settings(persist_setting_file);
        this->Init(app_settings, db_settings);
    }

    TmalphaMarketApplication::~TmalphaMarketApplication()
    {
        delete m_simulater;
        delete m_listener;
        delete m_consumer;
        delete m_provider;
    }

    bool TmalphaMarketApplication::Start()
    {
        if(m_simulater == nullptr)
        {
            LOG_WARN("simulater is invalid");
            return false;
        }

        if(m_simulater->Is_runing())
        {
            LOG_INFO("simulater is already runing");
            return false;
        }

        return m_simulater->Start();
    }

    void TmalphaMarketApplication::Join()
    {
        m_simulater->Join();
    }

    void TmalphaMarketApplication::Stop()
    {
        if(m_simulater == nullptr)
        {
            LOG_WARN("simulater is invalid");
            return;
        }

        if(!m_simulater->Is_runing())
        {
            LOG_INFO("simulater is already stopped");
            return;
        }

        LOG_INFO("stop simulater");
        m_simulater->Stop();
    }

    void TmalphaMarketApplication::Init(
            const fh::core::assist::Settings &app_settings,
            const fh::core::assist::Settings &persist_setting_file)
    {
        std::string market = app_settings.Get("alpha.market");
        std::string start_include = app_settings.Get("alpha.start_include");
        std::string end_exclude = app_settings.Get("alpha.end_exclude");
        std::string book_receive_url = app_settings.Get("alpha-market.book_url");
        float speed = std::stof(app_settings.Get("alpha.speed"));

        // 初期化数据提供者
        m_provider = new fh::core::persist::MarketDataProvider(market, persist_setting_file);
        m_provider->Range(start_include, end_exclude);

        // 初期化数据处理者
        if(market == "CME")
        {
            m_consumer = new fh::tmalpha::market::CmeDataConsumer();
        }
        else
        {
            LOG_WARN("unknow market: ", market);
            return;
        }

        // 初期化数据接受者（由于不需要发送原始数据，所以第一个参数不设置了）
        m_listener = new fh::core::book::BookSender("", book_receive_url);

        // 初期化重放控制模块
        m_simulater = new fh::tmalpha::market::MarketSimulater(m_listener, m_provider, m_consumer);
        m_simulater->Speed(speed);

        LOG_INFO("==== Replay ", market, " data of [", start_include, ", ", end_exclude, ") on speed ", speed);
    }

}   // namespace market
}   // namespace tmalpha
}   // namespace fh
