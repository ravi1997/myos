#ifndef __GDT_H
#define __GDT_H

#include <common//types.hpp>
#include <common//Manager.hpp>
#include <kernel//system//core//outputstream.hpp>


class GlobalDescriptorTable:public Manager
{
public:

  class SegmentDescriptor
  {
  private:
    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t base_hi;
    uint8_t type;
    uint8_t limit_hi;
    uint8_t base_vhi;

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

    friend OutputStream& operator<<(OutputStream& o,SystemType s){
      switch (s) {
        case SystemType::RESERVED1:o<<"reserved";break;
        case SystemType::_16_BIT_TSS_AVAILABLE:o<<"16 Bit Tss available";break;
        case SystemType::LDT:o<<"LDT";break;
        case SystemType::_16_BIT_TSS_BUSY:o<<"16 Bit Tss Busy";break;
        case SystemType::_16_BIT_CALL_GATE:o<<"16 Bit Call Gate";break;
        case SystemType::TASK_GATE:o<<"Task Gate";break;
        case SystemType::_16_BIT_INTERUPT_GATE:o<<"16 Bit Interupt Gate";break;
        case SystemType::_16_BIT_TRAP_GATE:o<<"16 Bit Trap Gate";break;
        case SystemType::RESERVED2:o<<"reserved";break;
        case SystemType::_32_BIT_TSS_BUSY:o<<"32 Bit Tss Busy";break;
        case SystemType::_32_BIT_CALL_GATE:o<<"32 Bit Call Gate";break;
        case SystemType::RESERVED4:o<<"reserved";break;
        case SystemType::_32_BIT_INTERUPT_GATE:o<<"32 Bit Interupt gate";break;
        case SystemType::_32_BIT_TRAP_GATE:o<<"32 Bit Trap Gate";break;
      }
      return o;
    }
    SystemType getSystemType(){
      uint8_t flag=type&0x0f;
      switch(flag){
        case 0: return SystemType::RESERVED1;
        case 1: return SystemType::_16_BIT_TSS_AVAILABLE;
        case 2: return SystemType::LDT;
        case 3: return SystemType::_16_BIT_TSS_BUSY;
        case 4: return SystemType::_16_BIT_CALL_GATE;
        case 5: return SystemType::TASK_GATE;
        case 6: return SystemType::_16_BIT_INTERUPT_GATE;
        case 7: return SystemType::_16_BIT_TRAP_GATE;
        case 8: return SystemType::RESERVED2;
        case 9: return SystemType::_32_BIT_TSS_AVAILABLE;
        case 10: return SystemType::RESERVED3;
        case 11: return SystemType::_32_BIT_TSS_BUSY;
        case 12: return SystemType::_32_BIT_CALL_GATE;
        case 13: return SystemType::RESERVED4;
        case 14: return SystemType::_32_BIT_INTERUPT_GATE;
        case 15: return SystemType::_32_BIT_TRAP_GATE;
      }
    }

    friend OutputStream& operator<<(OutputStream& o,DescriptorLevel d){
      switch(d){
        case DescriptorLevel::KERNELLEVEL:o<<"Kernel Level";break;
        case DescriptorLevel::DRIVERLEVEL:o<<"Driver Level";break;
        case DescriptorLevel::SHELLLEVEL:o<<"Shell Level";break;
        case DescriptorLevel::USERLEVEL:o<<"User Level";break;
      }
      return o;
    }
    DescriptorLevel getDescriptorLevel(){
      uint8_t flag=type&0x60;
      switch (flag>>5) {
        case 0:return DescriptorLevel::KERNELLEVEL;
        case 1:return DescriptorLevel::DRIVERLEVEL;
        case 2:return DescriptorLevel::SHELLLEVEL;
        case 3:return DescriptorLevel::USERLEVEL;
      }
    }
    SegmentDescriptor(uint32_t base=0, uint32_t limit=0, uint8_t type=0){
      uint8_t* target = (uint8_t*)this;

      if (limit <= 65536)
      {
        // 16-bit address space
        target[6] = 0x40;
      }
      else
      {
        // 32-bit address space
        // Now we have to squeeze the (32-bit) limit into 2.5 regiters (20-bit).
        // This is done by discarding the 12 least significant bits, but this
        // is only legal, if they are all ==1, so they are implicitly still there

        // so if the last bits aren't all 1, we have to set them to 1, but this
        // would increase the limit (cannot do that, because we might go beyond
          // the physical limit or get overlap with other segments) so we have to
          // compensate this by decreasing a higher bit (and might have up to
            // 4095 wasted bytes behind the used memory)

            if((limit & 0xFFF) != 0xFFF)
            limit = (limit >> 12)-1;
            else
            limit = limit >> 12;

            target[6] = 0xC0;
          }

          // Encode the limit
          target[0] = limit & 0xFF;
          target[1] = (limit >> 8) & 0xFF;
          target[6] |= (limit >> 16) & 0xF;

          // Encode the base
          target[2] = base & 0xFF;
          target[3] = (base >> 8) & 0xFF;
          target[4] = (base >> 16) & 0xFF;
          target[7] = (base >> 24) & 0xFF;

          // Type
          target[5] = type;
        }
        friend OutputStream& operator<<(OutputStream& o,SegmentDescriptor g){
          o<<"Print Discriptor as follows : ";
          o<<"Base address : "<<OutputStream::type::HEX<<g.getBase()<<" limit : "<<g.getLimit()<<endl;
          //o<<((g.type&0x60)>>5);
          o<<"DPL : "<<g.getDescriptorLevel()<<" Discriptor Type : "<<(((g.type&0x10)==0)?"system":"code//data");
          if((g.type&0x10)==0){
            o<<" System Type : "<<g.getSystemType();
          }else{
            o<<" Type : "<<(((g.type&0x08)==0)?"data":"code")<<endl;
            if((g.type&0x08)==0){
              o<<"Expand Direction : "<<((g.type&0x04)?"true":"false")<<" Write Enabled : "<<((g.type&0x02)?"true":"false")<<" Access : "<<((g.type&0x01)?"true":"false");
            }else{

            }
          }
          return o;
        }
        uint32_t getBase(){
          uint8_t* target = (uint8_t*)this;

          uint32_t result = target[7];
          result = (result << 8) + target[4];
          result = (result << 8) + target[3];
          result = (result << 8) + target[2];

          return result;
        }
        uint32_t getLimit(){
          uint8_t* target = (uint8_t*)this;

          uint32_t result = target[6] & 0xF;
          result = (result << 8) + target[1];
          result = (result << 8) + target[0];

          if((target[6] & 0xC0) == 0xC0)
          result = (result << 12) | 0xFFF;

          return result;
        }
        SegmentDescriptor& operator=(const SegmentDescriptor& s){
          limit_lo=s.limit_lo;
          base_lo=s.base_lo;
          base_hi=s.base_hi;
          type=s.type;
          limit_hi=s.limit_hi;
          base_vhi=s.base_vhi;
          return *this;
        }
        SegmentDescriptor& operator=(const SegmentDescriptor&& s){
          limit_lo=s.limit_lo;
          base_lo=s.base_lo;
          base_hi=s.base_hi;
          type=s.type;
          limit_hi=s.limit_hi;
          base_vhi=s.base_vhi;
          return *this;
        }
      } __attribute__((packed));

    private:
      SegmentDescriptor gdt[4];
      uint32_t entries=0;

      virtual void Loop()final{}
      virtual void Stop()final{}
      virtual void Deactivate()final{};
    public:

      GlobalDescriptorTable()
      {
    //    Initialize();
    //    Activate();
      }

      void Activate(){
        uint32_t i[2];
        i[1] = (uint32_t)gdt;
        i[0] = (sizeof(SegmentDescriptor)*entries) << 16;
        asm volatile("lgdt (%0)": :"p" (((uint8_t *) i)+2));
      }
      ~GlobalDescriptorTable(){}

      uint16_t CodeSegmentSelector(){
        return (uint8_t*)&gdt[2] - (uint8_t*)gdt;
      }
      uint16_t DataSegmentSelector(){
        return (uint8_t*)&gdt[3] - (uint8_t*)gdt;
      }

      void registerDescriptor(uint32_t base, uint32_t limit, uint8_t type){
        gdt[entries++]=SegmentDescriptor{base,limit,type};
      }

      static SegmentDescriptor makeSystemSegment(
        uint32_t base,
        uint32_t limit,
        SegmentDescriptor::DescriptorLevel DPL,
        bool Present,
        SegmentDescriptor::SystemType type
      )
      {
        uint8_t accessed=(Present)?0x80:0x00;
        switch(DPL){
          case SegmentDescriptor::DescriptorLevel::KERNELLEVEL: accessed|=0x00;break;
          case SegmentDescriptor::DescriptorLevel::DRIVERLEVEL: accessed|=0x20;break;
          case SegmentDescriptor::DescriptorLevel::SHELLLEVEL: accessed|=0x40;break;
          case SegmentDescriptor::DescriptorLevel::USERLEVEL: accessed|=0x60;break;
        }
        switch (type) {
          case SegmentDescriptor::SystemType::RESERVED1:accessed|=0x00;break;
          case SegmentDescriptor::SystemType::_16_BIT_TSS_AVAILABLE:accessed|=0x01;break;
          case SegmentDescriptor::SystemType::LDT:accessed|=0x02;break;
          case SegmentDescriptor::SystemType::_16_BIT_TSS_BUSY:accessed|=0x03;break;
          case SegmentDescriptor::SystemType::_16_BIT_CALL_GATE:accessed|=0x04;break;
          case SegmentDescriptor::SystemType::TASK_GATE:accessed|=0x05;break;
          case SegmentDescriptor::SystemType::_16_BIT_INTERUPT_GATE:accessed|=0x06;break;
          case SegmentDescriptor::SystemType::_16_BIT_TRAP_GATE:accessed|=0x07;break;
          case SegmentDescriptor::SystemType::RESERVED2:accessed|=0x08;break;
          case SegmentDescriptor::SystemType::_32_BIT_TSS_AVAILABLE:accessed|=0x09;break;
          case SegmentDescriptor::SystemType::RESERVED3:accessed|=0x0a;break;
          case SegmentDescriptor::SystemType::_32_BIT_TSS_BUSY:accessed|=0x0b;break;
          case SegmentDescriptor::SystemType::_32_BIT_CALL_GATE:accessed|=0x0c;break;
          case SegmentDescriptor::SystemType::RESERVED4:accessed|=0x0d;break;
          case SegmentDescriptor::SystemType::_32_BIT_INTERUPT_GATE:accessed|=0x0e;break;
          case SegmentDescriptor::SystemType::_32_BIT_TRAP_GATE:accessed|=0x0f;break;
        }
        return SegmentDescriptor{base,limit,accessed};
      }
      static SegmentDescriptor makeCodeSegment(
        uint32_t base,
        uint32_t limit,
        SegmentDescriptor::DescriptorLevel DPL,
        bool Present,
        bool Conforming,
        bool ReadEnabled,
        bool Access
      )
      {
        uint8_t accessed=(Present)?0x80:0x00;
        switch(DPL){
          case SegmentDescriptor::DescriptorLevel::KERNELLEVEL: accessed|=0x00;break;
          case SegmentDescriptor::DescriptorLevel::DRIVERLEVEL: accessed|=0x20;break;
          case SegmentDescriptor::DescriptorLevel::SHELLLEVEL: accessed|=0x40;break;
          case SegmentDescriptor::DescriptorLevel::USERLEVEL: accessed|=0x60;break;
        }
        accessed|=0x18;
        accessed|=((ReadEnabled)?0x02:0x00);
        accessed|=((Access)?0x01:0x00);


        return SegmentDescriptor{base,limit,accessed};
      }

      static SegmentDescriptor makeDataSegment(
        uint32_t base,
        uint32_t limit,
        SegmentDescriptor::DescriptorLevel DPL,
        bool Present,
        bool ExpandDirection,
        bool WriteEnabled,
        bool Access
      )
      {
        uint8_t accessed=(Present)?0x80:0x00;
        switch(DPL){
          case SegmentDescriptor::DescriptorLevel::KERNELLEVEL: accessed|=0x00;break;
          case SegmentDescriptor::DescriptorLevel::DRIVERLEVEL: accessed|=0x20;break;
          case SegmentDescriptor::DescriptorLevel::SHELLLEVEL: accessed|=0x40;break;
          case SegmentDescriptor::DescriptorLevel::USERLEVEL: accessed|=0x60;break;
        }
        accessed|=0x10;
        accessed|=((ExpandDirection)?0x04:0x00);
        accessed|=((WriteEnabled)?0x02:0x00);
        accessed|=((Access)?0x01:0x00);


        return SegmentDescriptor{base,limit,accessed};
      }

      virtual void Initialize() final {
        registerDescriptor(makeSystemSegment(0,0,SegmentDescriptor::DescriptorLevel::KERNELLEVEL,0,SegmentDescriptor::SystemType::RESERVED1));
        registerDescriptor(makeSystemSegment(0,0,SegmentDescriptor::DescriptorLevel::KERNELLEVEL,0,SegmentDescriptor::SystemType::RESERVED1));
        registerDescriptor(makeCodeSegment(0,64*1024*1024,SegmentDescriptor::DescriptorLevel::KERNELLEVEL,true,false,true,false));
        registerDescriptor(makeDataSegment(0,64*1024*1024,SegmentDescriptor::DescriptorLevel::KERNELLEVEL,true,false,true,false));
      }

      void registerDescriptor(SegmentDescriptor s){
        gdt[entries++]=s;
      }

    };

    #endif
