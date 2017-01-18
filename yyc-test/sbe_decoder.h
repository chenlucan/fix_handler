
#ifndef __SBE_DECODER_H__
#define __SBE_DECODER_H__

#include "global.h"
#include "mktdata.h"

namespace rczg
{
    class SBEDecoder
    {
        public:
            explicit SBEDecoder(char *buffer, std::size_t buffer_length);
            virtual ~SBEDecoder();
            
        public:
            // decode buffer to mdp message
            // return shared pointer of message header and message body
            // message header and body's data are same as input buffer
            virtual std::pair<std::shared_ptr<void>, std::shared_ptr<void>> Start_decode();
            
        private:
            template <typename HeaderType> 
            std::shared_ptr<HeaderType> Decode_header();
            template <typename HeaderType, typename MessageType> 
            std::shared_ptr<MessageType> Decode_message(const HeaderType &header);
            
        private:
            char* m_buffer;
            std::size_t m_buffer_length;
          
        private:
            DISALLOW_COPY_AND_ASSIGN(SBEDecoder);
    };
}
    
#endif // __SBE_DECODER_H__