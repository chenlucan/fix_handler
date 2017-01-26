
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
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)

#define BUFFER_MAX_LENGTH 1024 * 2       // max buffer size to receive udp data
#define MESSAGE_HEADER_VERSION 0     // mdp sbe message header version
            
#endif // __GLOBAL_H__    