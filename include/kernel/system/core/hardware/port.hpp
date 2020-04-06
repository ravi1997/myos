
#ifndef __PORT_H
#define __PORT_H

#include <common//types.hpp>


    template<typename t,bool slow>
    class Port;


    template<typename t,bool slow=false>
    class Port{
    private:
      uint16_t portNumber;
    public:
      Port(uint16_t Number):portNumber{Number}{}
      ~Port(){}

      void Write(t value){
        if constexpr (sizeof(t) == 1)
          if constexpr (slow==false)
            asm volatile ("outb %0, %1"::"a"(value),"Nd"(portNumber));
          else
            asm volatile ("outb %0, %1\njmp 1f\n1: jmp 1f\n1:"::"a"(value),"Nd"(portNumber));
        else if constexpr (sizeof(t)==2)
          asm volatile ("outw %0, %1"::"a"(value),"Nd"(portNumber));
        else
          asm volatile ("outl %0, %1"::"a"(value),"Nd"(portNumber));
      }

      t read(){
        t value;
        if constexpr (sizeof(t)==1)
          if constexpr (slow==false)
            asm volatile ("inb %1, %0":"=a"(value):"Nd"(portNumber));
          else
            asm volatile ("inb %1, %0":"=a"(value):"Nd"(portNumber));
        else if constexpr (sizeof(t)==2)
          asm volatile ("inw %1, %0":"=a"(value):"Nd"(portNumber));
        else
          asm volatile ("inl %1, %0":"=a"(value):"Nd"(portNumber));
        return value;
      }

    };


    typedef Port<uint8_t> Port_8Bit;
    typedef Port<uint8_t,true> Port8BitSlow;
    typedef Port<uint16_t> Port_16Bit;
    typedef Port<uint32_t> Port_32Bit;


#endif
