#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include<common//types.hpp>

#define BIT(n) (1U<<n)
void* memset(void* dest, uint8_t val, size_t bytes)
{
    void* retval = dest;
    size_t dwords = bytes/4; // Number of dwords (4 Byte blocks) to be written
    bytes %= 4;              // Remaining bytes
    uint32_t dval = (val<<24)|(val<<16)|(val<<8)|val; // Create dword from byte value
    __asm__ volatile("cld\n"
                     "rep stosl\n"
                     "mov %%edx, %%ecx\n"
                     "rep stosb" : "+D"(dest), "+c"(dwords) : "a"(dval), "d"(bytes) : "memory");
    return retval;
}

#endif
