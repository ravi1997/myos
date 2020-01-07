#ifndef _IQR_HPP_
#define _IQR_HPP_ 1

#include<lang//types.hpp>
#include<system//hardware//Ports.hpp>

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
    uint_16 size; uint_32 base;
  }__attribute__((packed));

public:
  class InterruptHandler;
protected:
  static InterruptHandler* handlers[256];
  static Port8BitSlow programmableInterruptControllerMasterCommandPort;
  static Port8BitSlow programmableInterruptControllerMasterDataPort;
  static Port8BitSlow programmableInterruptControllerSlaveCommandPort;
  static Port8BitSlow programmableInterruptControllerSlaveDataPort;

public:
  class InterruptHandler{
  protected:
    uint_8 InterruptNumber;
    InterruptHandler(uint_8 InterruptNumber){
      this->InterruptNumber = InterruptNumber;
      InterruptServiceRoutine::handlers[InterruptNumber] = this;
    }
    ~InterruptHandler(){ handlers[InterruptNumber] = nullptr; }
  public:
    virtual uint_32 HandleInterrupt(uint_32 esp){      return esp;}
  };

  friend class InterruptHandler;

  InterruptServiceRoutine()=delete;
  InterruptServiceRoutine(InterruptServiceRoutine&)=delete;
  InterruptServiceRoutine(InterruptServiceRoutine&&)=delete;
  InterruptServiceRoutine& operator=(InterruptServiceRoutine&)=delete;
  InterruptServiceRoutine& operator=(InterruptServiceRoutine&&)=delete;
  ~InterruptServiceRoutine()=delete;

  static bool interruptEnable;
  static GateDescriptor interruptDescriptorTable[256];
  const static uint_16 hardwareInterruptOffset=0x20;

  static void registerInterrupt(uint_8 interruptNumber,
    uint_16 codeSegmentSelectorOffset,void (*handler)(),
    uint_8 DPL,uint_8 DescriptorType
  )
  {
    auto &t=interruptDescriptorTable[interruptNumber];
    t.handlerAddressLowBits = (uint_16)(((uint_32) handler) & 0x0000FFFF);
    t.handlerAddressHighBits = (uint_16)((((uint_32) handler) >> 16) & 0x0000FFFF);
    t.codeSegmentSelector = codeSegmentSelectorOffset;

    const uint_8 IDT_DESC_PRESENT = 0x80;
    t.access = IDT_DESC_PRESENT | DescriptorType;
    t.reserved = 0;
  }

  static uint_32 HandleInterrupt(uint_8 interruptNumber,uint_32 esp);

  static uint_32 DoHandleInterrupt(uint_8 interruptNumber,uint_32 esp){
    cout<<"UNHANDLED INTERRUPT 0x"<<OutputStream::type::HEX<<interruptNumber<<OutputStream::type::DEC;
    return esp;
  }

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

  static void HandleException0x00();   static void HandleException0x01();
  static void HandleException0x02();   static void HandleException0x03();
  static void HandleException0x04();  static void HandleException0x05();
  static void HandleException0x06();  static void HandleException0x07();
  static void HandleException0x08();  static void HandleException0x09();
  static void HandleException0x0A();  static void HandleException0x0B();
  static void HandleException0x0C();  static void HandleException0x0D();
  static void HandleException0x0E();  static void HandleException0x0F();
  static void HandleException0x10();  static void HandleException0x11();
  static void HandleException0x12();  static void HandleException0x13();

  static void Activate(){
    interruptEnable=true;
    asm("sti");
  }
  static void Deactivate(){
    interruptEnable=false;
    asm("cli");
  }

  static void Initialize(){
    auto CodeSegment = GlobalDescriptorTableManager::CodeSegmentSelector();

    const uint_8 IDT_INTERRUPT_GATE = 0x0E;
    for(uint_8 i = 255; i > 0; --i)
    {
      registerInterrupt(i, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
      InterruptServiceRoutine::handlers[i] = nullptr;
    }
//    registerInterrupt(0, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
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

    registerInterrupt(0x80, CodeSegment, &HandleInterruptRequest0x80, 0, IDT_INTERRUPT_GATE);

    InterruptServiceRoutine::programmableInterruptControllerMasterCommandPort.write(0x11);
    InterruptServiceRoutine::programmableInterruptControllerSlaveCommandPort.write(0x11);

    InterruptServiceRoutine::programmableInterruptControllerMasterDataPort.write(hardwareInterruptOffset);
    InterruptServiceRoutine::programmableInterruptControllerSlaveDataPort.write(0x70);

    InterruptServiceRoutine::programmableInterruptControllerMasterDataPort.write(0x04);
    InterruptServiceRoutine::programmableInterruptControllerSlaveDataPort.write(0x02);

    InterruptServiceRoutine::programmableInterruptControllerMasterDataPort.write(0x01);
    InterruptServiceRoutine::programmableInterruptControllerSlaveDataPort.write(0x01);

    InterruptServiceRoutine::programmableInterruptControllerMasterDataPort.write(0x00);
    InterruptServiceRoutine::programmableInterruptControllerSlaveDataPort.write(0x00);

    InterruptDescriptorTablePointer idt_pointer;
    static_assert(sizeof(GateDescriptor)==8,"Gate Descriptor error");
    idt_pointer.size  = 256*sizeof(GateDescriptor) - 1;
    idt_pointer.base  = (uint_32)interruptDescriptorTable;
    asm volatile("lidtl %0" : : "m" (idt_pointer));
  }

  static uint_16 getHardwareInterruptOffset(){
    return InterruptServiceRoutine::hardwareInterruptOffset;
  }

}__attribute__((packed));

#endif
