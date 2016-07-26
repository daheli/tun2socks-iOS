//
//  byteorder.h
//  tun2socks
//
//  Created by LEI on 7/24/16.
//  Copyright © 2016 TouchingApp. All rights reserved.
//

#ifndef byteorder_h
#define byteorder_h

#include <stdint.h>

// for BYTE_ORDER
#ifdef _WIN32 // note the underscore: without it, it's not msdn official!
// Windows (x64 and x86)
#elif __unix__ // all unices, not all compilers
// Unix
#elif __linux__
// linux
#include <endian.h>
#elif __APPLE__
// Mac OS, not sure if this is covered by __posix__ and/or __unix__ though...
#include <machine/endian.h>
#endif

static uint16_t badvpn_reverse16 (uint16_t x)
{
    uint16_t y;
    *((uint8_t *)&y+0) = *((uint8_t *)&x+1);
    *((uint8_t *)&y+1) = *((uint8_t *)&x+0);
    return y;
}

static uint32_t badvpn_reverse32 (uint32_t x)
{
    uint32_t y;
    *((uint8_t *)&y+0) = *((uint8_t *)&x+3);
    *((uint8_t *)&y+1) = *((uint8_t *)&x+2);
    *((uint8_t *)&y+2) = *((uint8_t *)&x+1);
    *((uint8_t *)&y+3) = *((uint8_t *)&x+0);
    return y;
}

static uint64_t badvpn_reverse64 (uint64_t x)
{
    uint64_t y;
    *((uint8_t *)&y+0) = *((uint8_t *)&x+7);
    *((uint8_t *)&y+1) = *((uint8_t *)&x+6);
    *((uint8_t *)&y+2) = *((uint8_t *)&x+5);
    *((uint8_t *)&y+3) = *((uint8_t *)&x+4);
    *((uint8_t *)&y+4) = *((uint8_t *)&x+3);
    *((uint8_t *)&y+5) = *((uint8_t *)&x+2);
    *((uint8_t *)&y+6) = *((uint8_t *)&x+1);
    *((uint8_t *)&y+7) = *((uint8_t *)&x+0);
    return y;
}

static uint8_t hton8 (uint8_t x)
{
    return x;
}

static uint8_t htol8 (uint8_t x)
{
    return x;
}

#if defined(LITTLE_ENDIAN)

static uint16_t hton16 (uint16_t x)
{
    return badvpn_reverse16(x);
}

static uint32_t hton32 (uint32_t x)
{
    return badvpn_reverse32(x);
}

static uint64_t hton64 (uint64_t x)
{
    return badvpn_reverse64(x);
}

static uint16_t htol16 (uint16_t x)
{
    return x;
}

static uint32_t htol32 (uint32_t x)
{
    return x;
}

static uint64_t htol64 (uint64_t x)
{
    return x;
}

#elif defined(BIG_ENDIAN)

static uint16_t hton16 (uint16_t x)
{
    return x;
}

static uint32_t hton32 (uint32_t x)
{
    return x;
}

static uint64_t hton64 (uint64_t x)
{
    return x;
}

static uint16_t htol16 (uint16_t x)
{
    return badvpn_reverse16(x);
}

static uint32_t htol32 (uint32_t x)
{
    return badvpn_reverse32(x);
}

static uint64_t htol64 (uint64_t x)
{
    return badvpn_reverse64(x);
}

#endif

static uint8_t ntoh8 (uint8_t x)
{
    return hton8(x);
}

static uint16_t ntoh16 (uint16_t x)
{
    return hton16(x);
}

static uint32_t ntoh32 (uint32_t x)
{
    return hton32(x);
}

static uint64_t ntoh64 (uint64_t x)
{
    return hton64(x);
}

static uint8_t ltoh8 (uint8_t x)
{
    return htol8(x);
}

static uint16_t ltoh16 (uint16_t x)
{
    return htol16(x);
}

static uint32_t ltoh32 (uint32_t x)
{
    return htol32(x);
}

static uint64_t ltoh64 (uint64_t x)
{
    return htol64(x);
}


#endif /* byteorder_h */
