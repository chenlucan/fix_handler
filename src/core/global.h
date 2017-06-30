
#ifndef __FH_CORE_GLOBAL_H__
#define __FH_CORE_GLOBAL_H__

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)

#define BUFFER_MAX_LENGTH 1024 * 2       // max buffer size to receive udp data

// define cme config filename
const char CME_EXCHANGE_CLIENT_CFG[] = "cme_exchange_client.cfg";
const char CME_EXCHANGE_SETTINGS_INI[] = "cme_exchange_settings.ini";
const char CME_EXCHANGE_SERVER_CFG[] = "cme_exchange_server.cfg";

const char CME_MARKET_CONFIG_XML[] = "cme_market_config.xml";
const char CME_MARKET_SETTINGS_INI[] = "cme_market_settings.ini";

const char PERSIST_SETTINGS_INI[] = "persist_settings.ini";

const char TRADE_MATCHING_SETTINGS_INI[] = "trade_matching_settings.ini";

// define femas config filename
const char FEMAS_CONFIG_INI[] = "femas_config.ini";
// define rem config filename
const char REM_CONFIG_INI[] = "rem_config.ini";


#endif // __FH_CORE_GLOBAL_H__
