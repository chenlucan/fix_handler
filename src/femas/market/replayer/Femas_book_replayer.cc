
#include "Femas_book_replayer.h"
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>
#include "core/assist/logger.h"
#include "core/market/marketlisteneri.h"

#define  GET_SUB_FROM_JSON(view, key) view[key]
#define  GET_STR_FROM_JSON(view, key) view[key].get_utf8().value.to_string()
#define  GET_ARR_FROM_JSON(view, key) view[key].get_array().value
#define  GET_CHAR_FROM_JSON(view, key) GET_STR_FROM_JSON(view, key).at(0)
#define  GET_INT_FROM_JSON(view, key) std::stol(GET_STR_FROM_JSON(view, key))
#define  GET_INT_OR_DEFAULT_FROM_JSON(view, key, def) GET_SUB_FROM_JSON(view, key) ? GET_INT_FROM_JSON(view, key) : def
#define  GET_OPTIONAL_PRICE_FROM_JSON(target, view, key) { auto v = GET_SUB_FROM_JSON(view, #key); \
                                                                                                                    if(v) target.key = {GET_INT_FROM_JSON (v, "mantissa"), GET_INT_FROM_JSON (v, "exponent")}; }

namespace fh
{
namespace femas
{
namespace market
{
namespace replayer
{

FemasBookReplayer::FemasBookReplayer()
: m_listener(nullptr)	
{

}

FemasBookReplayer::~FemasBookReplayer()
{

}

void FemasBookReplayer::Add_listener(fh::core::market::MarketListenerI *listener)
{
    LOG_INFO("FemasBookReplayer::Add_listener");
    m_listener = listener;
    return;
}

void FemasBookReplayer::Apply_message(const std::string &message)
{
    LOG_INFO("FemasBookReplayer::Apply_message");
    return;
}

void FemasBookReplayer::Parse_definitions(const JSON_ELEMENT &message)
{
    return;
}



} // namespace replayer
} // namespace market
} // namespace femas
} // namespace fh

