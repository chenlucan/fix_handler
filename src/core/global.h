
#ifndef __FH_CORE_GLOBAL_H__
#define __FH_CORE_GLOBAL_H__

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)

#define BUFFER_MAX_LENGTH 1024 * 2       // max buffer size to receive udp data



#endif // __FH_CORE_GLOBAL_H__
