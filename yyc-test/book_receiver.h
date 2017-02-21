
#ifndef __BOOK_RECEIVER_H__
#define __BOOK_RECEIVER_H__

#include "global.h"
#include "zmq_receiver.h"

namespace rczg
{
    class BookReceiver : public ZmqReceiver
    {
        public:
            explicit BookReceiver(const char *url);
            virtual ~BookReceiver();
            
        public:
            virtual void Save(char *data, size_t size);
            
        private:
        	DISALLOW_COPY_AND_ASSIGN(BookReceiver);
    };
}

#endif // __BOOK_RECEIVER_H__
