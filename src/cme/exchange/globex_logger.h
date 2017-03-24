#ifndef __FH_CME_EXCHANGE_GLOBEX_LOGGER_H__
#define __FH_CME_EXCHANGE_GLOBEX_LOGGER_H__

#include <string>
#include <quickfix/Session.h>
#include <quickfix/Log.h>
#include "core/global.h"

namespace fh
{
namespace cme
{
namespace exchange
{
    class GlobexLog : public FIX::Log
    {
        public:
            GlobexLog();
            virtual ~GlobexLog();

        public:
            void clear();
            void backup();
            void onIncoming( const std::string& value );
            void onOutgoing( const std::string& value );
            void onEvent( const std::string& value );

        private:
            DISALLOW_COPY_AND_ASSIGN(GlobexLog);
    };

    class GlobexLogFactory : public FIX::LogFactory
    {
        public:
            GlobexLogFactory( const FIX::SessionSettings& settings );
            virtual ~GlobexLogFactory();

        public:
            FIX::Log* create();
            FIX::Log* create( const FIX::SessionID& );
            void destroy( FIX::Log* log );

        private:
            DISALLOW_COPY_AND_ASSIGN(GlobexLogFactory);
    };
} // namespace exchange
} // namespace cme
} // namespace fh

#endif // __FH_CME_EXCHANGE_GLOBEX_LOGGER_H__
