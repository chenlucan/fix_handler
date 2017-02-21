
#include "book_receiver.h"
#include "logger.h"
#include "utility.h"

namespace rczg
{
    
    BookReceiver::BookReceiver(const char *url) : ZmqReceiver(url)
    {
    	// noop
    }
    
    BookReceiver::~BookReceiver()
    {
        // noop
    }
    
    // TODO save it
    void BookReceiver::Save(char *data, size_t size)
    {
        LOG_TRACE("books: ", std::string(data, size));
    }
    
}







