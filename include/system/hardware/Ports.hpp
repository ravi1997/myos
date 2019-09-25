#ifndef _PORTS_HPP_
#define _PORTS_HPP_ 1

#ifndef _TYPES_HPP_
#include<lang//types.hpp>
#endif

template<typename t,bool slow=false>
class Port{
private:
  uint_16 portNumber;
public:
  Port(uint_16 Number):portNumber{Number}{}
  ~Port(){}

  void write(t value){
    if constexpr (sizeof(t) == 1)
      if constexpr (slow==false)
        asm volatile ("outb %0, %1"::"a"(value),"Nd"(portNumber));
      else
        asm volatile ("outb %0, %1"::"a"(value),"Nd"(portNumber));
    else if constexpr (sizeof(t)==2)
      asm volatile ("outw %0, %1"::"a"(value),"Nd"(portNumber));
    else
      asm volatile ("outl %0, %1"::"a"(value),"Nd"(portNumber));
  }

  t read(){
    t value;
    if constexpr (sizeof(t)==1)
      if constexpr (slow==false)
        asm volatile ("inb %1, %0"::"=a"(value),"Nd"(portNumber));
      else
        asm volatile ("inb %1, %0"::"=a"(value),"Nd"(portNumber));
    else if constexpr (sizeof(t)==2)
      asm volatile ("inw %1, %0"::"=a"(value),"Nd"(portNumber));
    else
      asm volatile ("inl %1, %0"::"=a"(value),"Nd"(portNumber));
    return value;
  }

};

typedef Port<uint_8> Port_8Bit;
typedef Port<uint_8,true> Port_8Bit_Slow;
typedef Port<uint_16> Port_16Bit;
typedef Port<uint_32> Port_32Bit;

#endif