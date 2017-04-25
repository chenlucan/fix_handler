
#include "core/assist/logger.h"
#include "core/assist/utility.h"
#include "core/persist/original_saver.h"

namespace fh
{
namespace core
{
namespace persist
{

    OriginalSaver::OriginalSaver(const std::string &setting_file)
    : m_settings(setting_file), m_db(nullptr), m_receiver(nullptr)
    {
        this->Init();
    }

    OriginalSaver::~OriginalSaver()
    {
        delete m_receiver;
        delete m_db;
    }

    void OriginalSaver::Init()
    {
        std::string receive_url = m_settings.Get("persist.receive_url");
        std::string db_url = m_settings.Get("persist.db_url");
        std::string db_name = m_settings.Get("persist.db_name");
        std::uint32_t page_size = std::stoi(m_settings.Get("persist.page_size"));

        m_db = new fh::core::persist::Mongo(db_url, db_name, page_size);
        m_receiver = new fh::core::persist::OriginalReceiver(receive_url,
                std::bind(&OriginalSaver::Save, this, std::placeholders::_1, std::placeholders::_2));
    }

    void OriginalSaver::Start()
    {
        m_receiver->Start_receive();
    }

    void OriginalSaver::Save(char *data, const size_t size)
    {
        m_db->Insert(std::string(data, size));
    }

} // namespace persist
} // namespace core
} // namespace fh

