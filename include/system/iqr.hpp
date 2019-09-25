#ifndef _IQR_HPP_
#define _IQR_HPP_ 1

#include<lang//types.hpp>

#ifndef _GDT_HPP_
#include<system//gdt.hpp>
#endif

class InterruptServiceRoutine{
protected:
  class GateDescriptor{
  public:
    uint_16 handlerAddressLowBits;
    uint_16 codeSegmentSelector;
    uint_8 reserved;
    uint_8 access;
    uint_16 handlerAddressHighBits;
  }__attribute__((packed));

  struct InterruptDescriptorTablePointer{
    uint_16 size;
    uint_32 base;
  }__attribute__((packed));

  static InterruptServiceRoutine* ActiveInterruptServiceRoutine;
public:
  class InterruptHandler;
protected:
  static InterruptHandler* handlers[256];
public:
  class InterruptHandler{
  protected:
      uint_8 InterruptNumber;
      InterruptServiceRoutine* interruptManager;
      InterruptHandler(InterruptServiceRoutine* interruptManager, uint_8 InterruptNumber){
          this->InterruptNumber = InterruptNumber;
          this->interruptManager = interruptManager;
          interruptManager->handlers[InterruptNumber] = this;
      }

      ~InterruptHandler(){
          if(interruptManager->handlers[InterruptNumber] == this)
              interruptManager->handlers[InterruptNumber] = 0;
      }

  public:
      virtual uint_32 HandleInterrupt(uint_32 esp){
          return esp;
      }
  };

  friend class InterruptHandler;

  InterruptServiceRoutine()=delete;
  InterruptServiceRoutine(InterruptServiceRoutine&)=delete;
  InterruptServiceRoutine(InterruptServiceRoutine&&)=delete;
  InterruptServiceRoutine& operator=(InterruptServiceRoutine&)=delete;
  InterruptServiceRoutine& operator=(InterruptServiceRoutine&&)=delete;
  ~InterruptServiceRoutine()=delete;

  static GateDescriptor interruptDescriptorTable[256];
  const static uint_16 hardwareInterruptOffset=0x20;

  static void registerInterrupt(
                                  uint_8 interruptNumber,
                                  uint_16 codeSegmentSelectorOffset,
                                  void (*handler)(),
                                  uint_8 DescriptorPrivilegeLevel,
                                  uint_8 DescriptorType
                               )
                               {
                                   // address of pointer to code segment (relative to global descriptor table)
                                   // and address of the handler (relative to segment)
                                   interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint_32) handler) & 0xFFFF;
                                   interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint_32) handler) >> 16) & 0xFFFF;
                                   interruptDescriptorTable[interruptNumber].codeSegmentSelector = codeSegmentSelectorOffset;

                                   const uint_8 IDT_DESC_PRESENT = 0x80;
                                   interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | ((DescriptorPrivilegeLevel & 3) << 5) | DescriptorType;
                                   interruptDescriptorTable[interruptNumber].reserved = 0;
                               }

  uint_32 DoHandleInterrupt(uint_8 interruptNumber,uint_32 esp);
  static uint_32 HandleInterrupt(uint_8 interruptNumber,uint_32 esp);

  static void InterruptIgnore();

  static void HandleInterruptRequest0x00();
  static void HandleInterruptRequest0x01();
  static void HandleInterruptRequest0x02();
  static void HandleInterruptRequest0x03();
  static void HandleInterruptRequest0x04();
  static void HandleInterruptRequest0x05();
  static void HandleInterruptRequest0x06();
  static void HandleInterruptRequest0x07();
  static void HandleInterruptRequest0x08();
  static void HandleInterruptRequest0x09();
  static void HandleInterruptRequest0x0A();
  static void HandleInterruptRequest0x0B();
  static void HandleInterruptRequest0x0C();
  static void HandleInterruptRequest0x0D();
  static void HandleInterruptRequest0x0E();
  static void HandleInterruptRequest0x0F();
  static void HandleInterruptRequest0x31();

  static void HandleInterruptRequest0x80();

  static void HandleException0x00();
  static void HandleException0x01();
  static void HandleException0x02();
  static void HandleException0x03();
  static void HandleException0x04();
  static void HandleException0x05();
  static void HandleException0x06();
  static void HandleException0x07();
  static void HandleException0x08();
  static void HandleException0x09();
  static void HandleException0x0A();
  static void HandleException0x0B();
  static void HandleException0x0C();
  static void HandleException0x0D();
  static void HandleException0x0E();
  static void HandleException0x0F();
  static void HandleException0x10();
  static void HandleException0x11();
  static void HandleException0x12();
  static void HandleException0x13();


  static void Activate(){

  }
  static void Deactivate();

  static void registerInterrupt(){
    uint_32 CodeSegment = GlobalDescriptorTableManager::CodeSegmentSelector();

    const uint_8 IDT_INTERRUPT_GATE = 0xE;
    for(uint_8 i = 255; i > 0; --i)
    {
        registerInterrupt(i, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
        InterruptServiceRoutine::handlers[i] = nullptr;
    }
    registerInterrupt(0, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
    InterruptServiceRoutine::handlers[0] = nullptr;

    registerInterrupt(0x00, CodeSegment, &HandleException0x00, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x01, CodeSegment, &HandleException0x01, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x02, CodeSegment, &HandleException0x02, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x03, CodeSegment, &HandleException0x03, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x04, CodeSegment, &HandleException0x04, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x05, CodeSegment, &HandleException0x05, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x06, CodeSegment, &HandleException0x06, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x07, CodeSegment, &HandleException0x07, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x08, CodeSegment, &HandleException0x08, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x09, CodeSegment, &HandleException0x09, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x0A, CodeSegment, &HandleException0x0A, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x0B, CodeSegment, &HandleException0x0B, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x0C, CodeSegment, &HandleException0x0C, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x0D, CodeSegment, &HandleException0x0D, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x0E, CodeSegment, &HandleException0x0E, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x0F, CodeSegment, &HandleException0x0F, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x10, CodeSegment, &HandleException0x10, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x11, CodeSegment, &HandleException0x11, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x12, CodeSegment, &HandleException0x12, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(0x13, CodeSegment, &HandleException0x13, 0, IDT_INTERRUPT_GATE);

    registerInterrupt(hardwareInterruptOffset + 0x00, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x01, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x02, CodeSegment, &HandleInterruptRequest0x02, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x03, CodeSegment, &HandleInterruptRequest0x03, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x04, CodeSegment, &HandleInterruptRequest0x04, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x05, CodeSegment, &HandleInterruptRequest0x05, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x06, CodeSegment, &HandleInterruptRequest0x06, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x07, CodeSegment, &HandleInterruptRequest0x07, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x08, CodeSegment, &HandleInterruptRequest0x08, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x09, CodeSegment, &HandleInterruptRequest0x09, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x0A, CodeSegment, &HandleInterruptRequest0x0A, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x0B, CodeSegment, &HandleInterruptRequest0x0B, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x0C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x0D, CodeSegment, &HandleInterruptRequest0x0D, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x0E, CodeSegment, &HandleInterruptRequest0x0E, 0, IDT_INTERRUPT_GATE);
    registerInterrupt(hardwareInterruptOffset + 0x0F, CodeSegment, &HandleInterruptRequest0x0F, 0, IDT_INTERRUPT_GATE);

    registerInterrupt(                          0x80, CodeSegment, &HandleInterruptRequest0x80, 0, IDT_INTERRUPT_GATE);

  }

  static void initialize(){
    InterruptDescriptorTablePointer idt_pointer;
    idt_pointer.size  = 256*sizeof(GateDescriptor) - 1;
    idt_pointer.base  = (uint_32)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt_pointer));
  }

  static uint_16 getHardwareInterruptOffset();

};



#endif
