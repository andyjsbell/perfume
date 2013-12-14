#ifndef _EAU_TYPES_H_
#define _EAU_TYPES_H_

// basic data types
#ifdef WIN32
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;
typedef signed __int64      int64_t;
typedef unsigned __int64    uint64_t;
typedef long                intptr_t;
typedef unsigned long       uintptr_t;
#else
#include <stdint.h>
#endif

typedef void *              voidptr_t;
typedef void *              handle_t;


// uuid, 16bytes
// e.g. F8A287DD-2970-4147-889E-1606464BBCC8
#pragma pack(push, 1)
typedef struct {
    uint32_t u0;
    uint16_t u1;
    uint16_t u2;
    uint16_t u3;
    uint8_t u4[6];
}uuid_t;
#pragma pack(pop)
#define UUID_DEFINE(name,u0,u1,u2,u3,u4) \
    static const uuid_t name __attribute__ ((unused)) = {u0,u1,u2,u3,u4}

#include <vector>
typedef std::vector<uuid_t> uuids_t;


#endif
