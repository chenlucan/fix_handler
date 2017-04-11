
#ifndef __FH_CME_MARKET_DAT_PROCESSOR_H__
#define __FH_CME_MARKET_DAT_PROCESSOR_H__

#include "core/global.h"
#include "cme/market/message/mdp_message.h"
#include "cme/market/dat_arbitrator.h"
#include "cme/market/dat_saver.h"
#include "cme/market/dat_replayer.h"

namespace fh
{
namespace cme
{
namespace market
{
    class DatProcessor
    {
        public:
            DatProcessor(fh::cme::market::DatSaver *, fh::cme::market::DatReplayer *);
            virtual ~DatProcessor();

        public:
            // process udp feed data to mdp messages and save it
            virtual void Process_feed_data(char *buffer, const size_t data_length);
            // process tcp replay data to mdp messages and save it
            virtual void Process_replay_data(char *buffer, const size_t data_length);

        private:
            void Process_data(char *buffer, const size_t data_length);
            void Start_tcp_replay(std::uint32_t begin, std::uint32_t end);
            void Save_message(std::uint32_t packet_seq_num, std::vector<fh::cme::market::message::MdpMessage> &mdp_messages);

        private:
            fh::cme::market::DatArbitrator m_arbitrator;
            fh::cme::market::DatSaver *m_saver;
            fh::cme::market::DatReplayer *m_replayer;

        private:
            DISALLOW_COPY_AND_ASSIGN(DatProcessor);
    };   
} // namespace market
} // namespace cme
} // namespace fh

#endif // __FH_CME_MARKET_DAT_PROCESSOR_H__
