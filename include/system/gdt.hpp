#ifndef _GDT_HPP_
#define _GDT_HPP_ 1

#ifndef _TYPES_HPP_
#include<lang//types.hpp>
#endif

#define MAXGDT 20

class GlobalDescriptorTableManager{
public:
  class GlobalDescriptor{
  private:
    uint_16 limit_lo;
    uint_32 base_lo:24;
    uint_8 flag_low;
    uint_8 limit_hi:4;
    uint_8 flag_hi:4;
    uint_8 base_hi;
  public:
    enum class DescriptorLevel{
      KERNELLEVEL=0x0,
      DRIVERLEVEL=0x1,
      SHELLLEVEL=0x2,
      USERLEVEL=0x3
    };

    enum class Granuality{
      _1_kib=0,
      _4_kib=1
    };

    enum  class SystemType {
      RESERVED1,
      _16_BIT_TSS_AVAILABLE,
      LDT,
      _16_BIT_TSS_BUSY,
      _16_BIT_CALL_GATE,
      TASK_GATE,
      _16_BIT_INTERUPT_GATE,
      _16_BIT_TRAP_GATE,
      RESERVED2,
      _32_BIT_TSS_AVAILABLE,
      RESERVED3,
      _32_BIT_TSS_BUSY,
      _32_BIT_CALL_GATE,
      RESERVED4,
      _32_BIT_INTERUPT_GATE,
      _32_BIT_TRAP_GATE
    };

    GlobalDescriptor(uint_32 base=0,uint_32 limit=0,uint_8 access=0,uint_8 granuality=0){
      limit_lo=limit&0xffff;

      base_lo=base&0xffffff;
      flag_low=access;
      limit_hi=((limit>>16)&0xf);
      flag_hi=(granuality>>4)&0xf;
      base_hi=(base>>24)&0xff;
    }

    GlobalDescriptor(const GlobalDescriptor& g){
      limit_lo=g.limit_lo;
      base_lo=g.base_lo;
      flag_low=g.flag_low;
      limit_hi=g.limit_hi;
      flag_hi=g.flag_hi;
      base_hi=g.base_hi;
    }

    GlobalDescriptor(const GlobalDescriptor&& g){
      limit_lo=g.limit_lo;
      base_lo=g.base_lo;
      flag_low=g.flag_low;
      limit_hi=g.limit_hi;
      flag_hi=g.flag_hi;
      base_hi=g.base_hi;
    }

    GlobalDescriptor& operator=(const GlobalDescriptor& g){
      limit_lo=g.limit_lo;
      base_lo=g.base_lo;
      flag_low=g.flag_low;
      limit_hi=g.limit_hi;
      flag_hi=g.flag_hi;
      base_hi=g.base_hi;
      return *this;
    }

    GlobalDescriptor& operator=(const GlobalDescriptor&& g){
      limit_lo=g.limit_lo;
      base_lo=g.base_lo;
      flag_low=g.flag_low;
      limit_hi=g.limit_hi;
      flag_hi=g.flag_hi;
      base_hi=g.base_hi;
      return *this;
    }


    uint_32 getBase(){
      uint_32 base=base_hi;
      base<<=24;
      base|=(base_lo&0xffffff);
      return base;
    }


    uint_32 getLimit(){
      uint_32 limit=limit_hi;
      limit<<=16;
      limit|=(limit_lo&0xffff);
      return limit;
    }
  };


//  static GlobalDescriptor makeLDT();
  static GlobalDescriptor makeSystemSegment(
    uint_32 base,
    uint_32 limit,
    GlobalDescriptor::DescriptorLevel DPL,
    bool G,
    bool Present,
    GlobalDescriptor::SystemType type
  )
  {
    uint_8 accessed=(Present)?0x80:0x00;
    switch(DPL){
      case GlobalDescriptor::DescriptorLevel::KERNELLEVEL: accessed|=0x00;break;
      case GlobalDescriptor::DescriptorLevel::DRIVERLEVEL: accessed|=0x20;break;
      case GlobalDescriptor::DescriptorLevel::SHELLLEVEL: accessed|=0x40;break;
      case GlobalDescriptor::DescriptorLevel::USERLEVEL: accessed|=0x60;break;
    }
    switch (type) {
      case GlobalDescriptor::SystemType::RESERVED1:accessed|=0x00;break;
      case GlobalDescriptor::SystemType::_16_BIT_TSS_AVAILABLE:accessed|=0x01;break;
      case GlobalDescriptor::SystemType::LDT:accessed|=0x02;break;
      case GlobalDescriptor::SystemType::_16_BIT_TSS_BUSY:accessed|=0x03;break;
      case GlobalDescriptor::SystemType::_16_BIT_CALL_GATE:accessed|=0x04;break;
      case GlobalDescriptor::SystemType::TASK_GATE:accessed|=0x05;break;
      case GlobalDescriptor::SystemType::_16_BIT_INTERUPT_GATE:accessed|=0x06;break;
      case GlobalDescriptor::SystemType::_16_BIT_TRAP_GATE:accessed|=0x07;break;
      case GlobalDescriptor::SystemType::RESERVED2:accessed|=0x08;break;
      case GlobalDescriptor::SystemType::_32_BIT_TSS_AVAILABLE:accessed|=0x09;break;
      case GlobalDescriptor::SystemType::RESERVED3:accessed|=0x0a;break;
      case GlobalDescriptor::SystemType::_32_BIT_TSS_BUSY:accessed|=0x0b;break;
      case GlobalDescriptor::SystemType::_32_BIT_CALL_GATE:accessed|=0x0c;break;
      case GlobalDescriptor::SystemType::RESERVED4:accessed|=0x0d;break;
      case GlobalDescriptor::SystemType::_32_BIT_INTERUPT_GATE:accessed|=0x0e;break;
      case GlobalDescriptor::SystemType::_32_BIT_TRAP_GATE:accessed|=0x0f;break;
    }
    uint_8 granuality=(G)?0x80:0x00;
    return GlobalDescriptor(base,limit,accessed,granuality);
  }
  static GlobalDescriptor makeCodeSegment(
          uint_32 base,
          uint_32 limit,
          GlobalDescriptor::DescriptorLevel DPL,
          bool G,
          bool Default,
          bool Availability,
          bool Present,
          bool Conforming,
          bool ReadEnabled,
          bool Access
        )
        {
          uint_8 accessed=(Present)?0x80:0x00;
          switch(DPL){
            case GlobalDescriptor::DescriptorLevel::KERNELLEVEL: accessed|=0x00;break;
            case GlobalDescriptor::DescriptorLevel::DRIVERLEVEL: accessed|=0x20;break;
            case GlobalDescriptor::DescriptorLevel::SHELLLEVEL: accessed|=0x40;break;
            case GlobalDescriptor::DescriptorLevel::USERLEVEL: accessed|=0x60;break;
          }
          accessed|=0x18;
          accessed|=((Conforming)?0x04:0x00);
          accessed|=((ReadEnabled)?0x02:0x00);
          accessed|=((Access)?0x01:0x00);

          uint_8 granuality=(G)?0x80:0x00;
          granuality|=((Default)?0x40:0x00);
          granuality|=((Availability)?0x10:0x00);
          return GlobalDescriptor(base,limit,accessed,granuality);
        }

  static GlobalDescriptor makeDataSegment(
          uint_32 base,
          uint_32 limit,
          GlobalDescriptor::DescriptorLevel DPL,
          bool G,
          bool Big,
          bool Availability,
          bool Present,
          bool ExpandDirection,
          bool WriteEnabled,
          bool Access
        )
        {
          uint_8 accessed=(Present)?0x80:0x00;
          switch(DPL){
            case GlobalDescriptor::DescriptorLevel::KERNELLEVEL: accessed|=0x00;break;
            case GlobalDescriptor::DescriptorLevel::DRIVERLEVEL: accessed|=0x20;break;
            case GlobalDescriptor::DescriptorLevel::SHELLLEVEL: accessed|=0x40;break;
            case GlobalDescriptor::DescriptorLevel::USERLEVEL: accessed|=0x60;break;
          }
          accessed|=0x10;
          accessed|=((ExpandDirection)?0x04:0x00);
          accessed|=((WriteEnabled)?0x02:0x00);
          accessed|=((Access)?0x01:0x00);

          uint_8 granuality=(G)?0x80:0x00;
          granuality|=((Big)?0x40:0x00);
          granuality|=((Availability)?0x10:0x00);
          return GlobalDescriptor(base,limit,accessed,granuality);
        }

//  static TaskSegmentSelector makeTSS();


  static void Initialize(){
    GlobalDescriptorTableManager::registerDescriptor(
      GlobalDescriptorTableManager::makeSystemSegment(
        0,
        0,
        GlobalDescriptor::DescriptorLevel::KERNELLEVEL,
        false,
        false,
        GlobalDescriptor::SystemType::RESERVED1
      )
    ); //NULL Descriptor
    GlobalDescriptorTableManager::registerDescriptor(
      GlobalDescriptorTableManager::makeCodeSegment(
        0,
        0xFFFFF,
        GlobalDescriptor::DescriptorLevel::KERNELLEVEL,
        true,
        true,
        false,
        true,
        false,
        true,
        false
      )
    ); //kernel code Descriptor
    GlobalDescriptorTableManager::registerDescriptor(
      GlobalDescriptorTableManager::makeDataSegment(
        0,
        0xFFFFF,
        GlobalDescriptor::DescriptorLevel::KERNELLEVEL,
        true,
        true,
        false,
        true,
        false,
        true,
        false
      )
    ); //kernel data Descriptor
    GlobalDescriptorTableManager::registerDescriptor(
      GlobalDescriptorTableManager::makeCodeSegment(
        0,
        0xFFFFF,
        GlobalDescriptor::DescriptorLevel::USERLEVEL,
        true,
        true,
        false,
        true,
        false,
        true,
        false
      )
    ); //kernel code Descriptor
    GlobalDescriptorTableManager::registerDescriptor(
      GlobalDescriptorTableManager::makeDataSegment(
        0,
        0xFFFFF,
        GlobalDescriptor::DescriptorLevel::USERLEVEL,
        true,
        true,
        false,
        true,
        false,
        true,
        false
      )
    ); //kernel data Descriptor

  }
  static void Activate(){
    uint_32 i[2];
    i[1] = (uint_32)GlobalDescriptorTableManager::gdt;
    i[0] =  GlobalDescriptorTableManager::GDT_Entries<< 16;
    asm volatile("lgdt (%0)"::"a" (((uint_8 *) i)+2));
  }
  static void registerDescriptor(const GlobalDescriptor g){
    gdt[GDT_Entries++]=g;
  }

  static GlobalDescriptor gdt[MAXGDT];
  static uint_32 GDT_Entries;
  const static uint_32 NULLSEGMENTOFFSET=0;
  const static uint_32 KERNELCODESEGMENTOFFSET=1;
  const static uint_32 KERNELDATASEGMENTOFFSET=2;

  static uint16_t GlobalDescriptorTable::DataSegmentSelector()
  {
      return (uint8_t*)&GlobalDescriptorTable::gdt[GlobalDescriptorTable::KERNELDATASEGMENTOFFSET] - (uint8_t*)GlobalDescriptorTable::gdt;
  }

  static uint16_t GlobalDescriptorTable::CodeSegmentSelector()
  {
    return (uint8_t*)&GlobalDescriptorTable::gdt[GlobalDescriptorTable::KERNELCODESEGMENTOFFSET] - (uint8_t*)GlobalDescriptorTable::gdt;
  }

};

GlobalDescriptorTableManager::GlobalDescriptor GlobalDescriptorTableManager::gdt[MAXGDT];
uint_32 GlobalDescriptorTableManager::GDT_Entries=0;


#endif
