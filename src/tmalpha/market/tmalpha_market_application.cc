
#include "core/assist/logger.h"
#include "tmalpha/market/market_data_provider.h"
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
    : m_provider(nullptr), m_consume(nullptr),  m_default_listener(nullptr), m_simulater(nullptr), m_thread(nullptr)
    {
        fh::core::assist::Settings app_settings(app_setting_file);
        fh::core::assist::Settings db_settings(persist_setting_file);
        this->Init(app_settings, db_settings);
    }

    TmalphaMarketApplication::~TmalphaMarketApplication()
    {
        delete m_thread;
        delete m_simulater;
        delete m_default_listener;
        delete m_consume;
        delete m_provider;
    }

    void TmalphaMarketApplication::Add_replay_listener(fh::tmalpha::market::MarketReplayListener *replay_listener)
    {
        m_simulater->Add_replay_listener(replay_listener);
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

        m_thread = new std::thread([this](){m_simulater->Start();});
        LOG_INFO("simulater started");

        return true;
    }

    void TmalphaMarketApplication::Join()
    {
        if(m_simulater == nullptr)
        {
            LOG_WARN("simulater is invalid");
            return;
        }

        if(!m_simulater->Is_runing())
        {
            LOG_INFO("simulater is stopped");
            return;
        }

        m_thread->join();
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
        float speed = std::stof(app_settings.Get("alpha.speed"));

        // 初期化数据提供者
        m_provider = new fh::tmalpha::market::MarketDataProvider(market, persist_setting_file);
        m_provider->Range(start_include, end_exclude);

        // 初期化数据处理者
        if(market == "CME")
        {
            m_consume = new fh::tmalpha::market::CmeDataConsumer();
        }
        else
        {
            LOG_WARN("unknow market: ", market);
            return;
        }

        // 初期化默认的数据接受者
        m_default_listener = new fh::tmalpha::market::DefaultMarketReplayListener();

        // 初期化重放控制模块
        m_simulater = new fh::tmalpha::market::MarketSimulater(m_provider, m_consume);
        m_simulater->Add_replay_listener(m_default_listener);
        m_simulater->Speed(speed);

        LOG_INFO("==== Replay ", market, " data of [", start_include, ", ", end_exclude, ") on speed ", speed);
    }

}   // namespace market
}   // namespace tmalpha
}   // namespace fh
