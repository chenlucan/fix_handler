
#ifndef __FH_CORE_PERSIST_ORIGINAL_SAVER_H__
#define __FH_CORE_PERSIST_ORIGINAL_SAVER_H__

#include "core/global.h"
#include "core/persist/original_receiver.h"
#include "core/persist/mongo.h"
#include "core/assist/settings.h"

namespace fh
{
namespace core
{
namespace persist
{
    class OriginalSaver
    {
        public:
            explicit OriginalSaver(const std::string &setting_file = "persist_settings.ini");
            virtual ~OriginalSaver();

        public:
            void Start();

        private:
            void Init();
            void Save(char *data, const size_t size);

        private:
            fh::core::assist::Settings m_settings;
            fh::core::persist::Mongo *m_db;
            fh::core::persist::OriginalReceiver *m_receiver;

        private:
            DISALLOW_COPY_AND_ASSIGN(OriginalSaver);
    };
} // namespace persist
} // namespace core
} // namespace fh

#endif // __FH_CORE_PERSIST_ORIGINAL_SAVER_H__
