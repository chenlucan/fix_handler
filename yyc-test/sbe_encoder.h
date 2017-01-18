
#ifndef __SBE_ENCODER_H__
#define __SBE_ENCODER_H__

#include "global.h"
#include "mktdata.h"

namespace rczg
{
    class SBEEncoder
    {
        public:
            SBEEncoder();
            virtual ~SBEEncoder();
            
        public:
            void Start_encode(std::uint16_t templateId);
            std::pair<char*, std::size_t> Encoded_buffer();
            std::string Encoded_hex_str() const;
            
        private:
            template <typename MessageType> 
            std::size_t Encode_header(mktdata::MessageHeader &header);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::MDIncrementalRefreshVolume37 &message);
            std::size_t Encode_message(mktdata::MessageHeader &header, mktdata::SecurityStatus30 &message);
            
        private:
            std::size_t m_encoded_length;
            char m_buffer[BUFFER_MAX_LENGTH];
          
        private:
            DISALLOW_COPY_AND_ASSIGN(SBEEncoder);
    };
}
    
#endif // __SBE_ENCODER_H__