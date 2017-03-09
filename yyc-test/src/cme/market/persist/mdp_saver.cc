
#include "cme/market/persist/mdp_saver.h"
#include "cme/market/setting/market_settings.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"

namespace fh
{
namespace cme
{
namespace market
{
namespace persist
{

    MdpSaver::MdpSaver(const std::string &app_setting_file) : m_receiver(nullptr)
    {
        this->Init(app_setting_file);
    }

    MdpSaver::~MdpSaver()
    {
        delete m_receiver;
    }

    void MdpSaver::Init(const std::string &app_setting_file)
    {
        fh::cme::market::setting::MarketSettings app_settings(app_setting_file);
        std::string receive_url = app_settings.Get_persist_receive_url();
        std::string db_url = app_settings.Get_db_url();
        std::pair<std::string, std::string> coll = app_settings.Get_save_collection();

        m_receiver = new fh::cme::market::persist::MdpReceiver(receive_url, db_url, coll.first, coll.second);
    }

    void MdpSaver::Start_save()
    {
        m_receiver->Start_receive();
    }

} // namespace persist
} // namespace market
} // namespace cme
} // namespace fh
