#ifndef __FH_FEMAS_MARKET_REPLAYER_FEMAS_BOOK_REPLAYER_H__
#define __FH_FEMAS_MARKET_REPLAYER_FEMAS_BOOK_REPLAYER_H__

#include <vector>
#include <unordered_map>
#include <bsoncxx/document/view.hpp>
#include "core/global.h"
#include "core/market/marketlisteneri.h"


#define JSON_ELEMENT bsoncxx::document::element


namespace fh
{
namespace femas
{
namespace market
{
namespace replayer
{

    class FemasBookReplayer
    {
        public:
            FemasBookReplayer();
            virtual ~FemasBookReplayer();

	 public:
	 	//重放整理好的数据  
	 	void Add_listener(fh::core::market::MarketListenerI *listener);
		//重放一条消息
		void Apply_message(const std::string &message);

        private:
            void Parse_definitions(const JSON_ELEMENT &message);

        private:
	     fh::core::market::MarketListenerI *m_listener;		
		
	 private:
            DISALLOW_COPY_AND_ASSIGN(FemasBookReplayer);		
    };


} // namespace replayer
} // namespace market
} // namespace femas
} // namespace fh

#endif // __FH_FEMAS_MARKET_REPLAYER_FEMAS_BOOK_REPLAYER_H__