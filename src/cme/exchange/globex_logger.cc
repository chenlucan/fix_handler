
#include "cme/exchange/globex_logger.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"

namespace fh
{
namespace cme
{
namespace exchange
{

    GlobexLog::GlobexLog()
    {
        // noop
    }

    GlobexLog::~GlobexLog()
    {
        // noop
    }

    void GlobexLog::clear()
    {
        // noop
    }

    void GlobexLog::backup()
    {
        // noop
    }

    void GlobexLog::onIncoming( const std::string& value )
    {
        LOG_INFO("RECEIVED FROM CME: ", fh::core::assist::utility::Format_fix_message(value));
    }

    void GlobexLog::onOutgoing( const std::string& value )
    {
        LOG_INFO("SEND TO CME: ", fh::core::assist::utility::Format_fix_message(value));
    }

    void GlobexLog::onEvent( const std::string& value )
    {
        LOG_INFO("GLOBEX EVENT: ", fh::core::assist::utility::Format_fix_message(value));
    }

    GlobexLogFactory::GlobexLogFactory( const FIX::SessionSettings& settings )
    {
        // noop
    }

    GlobexLogFactory::~GlobexLogFactory()
    {
        // noop
    }

    FIX::Log* GlobexLogFactory::create()
    {
        return new GlobexLog();
    }

    FIX::Log* GlobexLogFactory::create( const FIX::SessionID& sessionID )
    {
      return new GlobexLog();
    }

    void GlobexLogFactory::destroy( FIX::Log* pLog )
    {
      delete pLog;
    }

} // namespace exchange
} // namespace cme
} // namespace fh
