
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <utility>
#include <iostream>
#include <sstream>
#include <random>
#include <iomanip>
#include <set>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <limits>
#include <vector>
#include <functional>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/iterator/counting_iterator.hpp>


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)

#define BUFFER_MAX_LENGTH 1024       // max buffer size to receive udp data
#define MESSAGE_HEADER_VERSION 0     // mdp sbe message header version
            
#endif // __GLOBAL_H__    