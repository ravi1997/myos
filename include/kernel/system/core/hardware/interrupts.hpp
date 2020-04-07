
#ifndef __INTERRUPTMANAGER_H
#define __INTERRUPTMANAGER_H

    #include <kernel//system//core//gdt.hpp>
    #include <kernel//system//core//hardware//port.hpp>


    class InterruptManager;

    class InterruptHandler
    {
    protected:
        uint8_t InterruptNumber;
        InterruptManager* interruptManager;
        InterruptHandler(InterruptManager* interruptManager, uint8_t InterruptNumber);
        ~InterruptHandler();
    public:
        virtual uint32_t HandleInterrupt(uint32_t esp);
        friend class InterruptManager;
    };


    class InterruptManager:public Manager
    {
        //friend class InterruptHandler;
        protected:
          Port8BitSlow programmableInterruptControllerMasterCommandPort;
          Port8BitSlow programmableInterruptControllerMasterDataPort;
          Port8BitSlow programmableInterruptControllerSlaveCommandPort;
          Port8BitSlow programmableInterruptControllerSlaveDataPort;

          virtual void Loop()final{}
          virtual void Stop()final{}

          InterruptHandler* handlers[256];
            static InterruptManager* ActiveInterruptManager ;

        public:
          friend class InterruptHandler;
            struct GateDescriptor
            {
                uint16_t handlerAddressLowBits;
                uint16_t gdt_codeSegmentSelector;
                uint8_t reserved;
                uint8_t access;
                uint16_t handlerAddressHighBits;



            } __attribute__((packed));

            static GateDescriptor interruptDescriptorTable[256];

            struct InterruptDescriptorTablePointer
            {
                uint16_t size;
                uint32_t base;
            } __attribute__((packed));

            const static uint16_t hardwareInterruptOffset=0x0020;
            uint16_t CodeSegment;
            //static InterruptManager* ActiveInterruptManager;
            static void SetInterruptDescriptorTableEntry(uint8_t interrupt,
                uint16_t codeSegmentSelectorOffset, void (*handlers)(),
                uint8_t DescriptorPrivilegeLevel, uint8_t DescriptorType){
                    // address of pointer to code segment (relative to global descriptor table)
                    // and address of the handler (relative to segment)
                    interruptDescriptorTable[interrupt].handlerAddressLowBits = ((uint32_t) handlers) & 0xFFFF;
                    interruptDescriptorTable[interrupt].handlerAddressHighBits = (((uint32_t) handlers) >> 16) & 0xFFFF;
                    interruptDescriptorTable[interrupt].gdt_codeSegmentSelector = codeSegmentSelectorOffset;

                    const uint8_t IDT_DESC_PRESENT = 0x80;
                    interruptDescriptorTable[interrupt].access = IDT_DESC_PRESENT | ((DescriptorPrivilegeLevel & 3) << 5) | DescriptorType;
                    interruptDescriptorTable[interrupt].reserved = 0;
                }

            uint32_t DoHandleInterrupt(uint8_t interrupt, uint32_t esp);

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

            static uint32_t HandleInterrupt(uint8_t interrupt, uint32_t esp);

            InterruptManager(GlobalDescriptorTable&g):
              programmableInterruptControllerMasterCommandPort(0x20),
              programmableInterruptControllerMasterDataPort(0x21),
              programmableInterruptControllerSlaveCommandPort(0xA0),
              programmableInterruptControllerSlaveDataPort(0xA1),
              CodeSegment {g.CodeSegmentSelector()}
        {
      //    Initialize();
        }

            InterruptManager(const InterruptManager& im):programmableInterruptControllerMasterCommandPort(0x20),
            programmableInterruptControllerMasterDataPort(0x21),
            programmableInterruptControllerSlaveCommandPort(0xA0),
            programmableInterruptControllerSlaveDataPort(0xA1),
            CodeSegment {im.CodeSegment}
            {
              const uint8_t IDT_INTERRUPT_GATE = 0xE;

              for(uint8_t i = 255; i > 0; --i)
              {
                SetInterruptDescriptorTableEntry(i, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
                handlers[i] = nullptr;
              }
              SetInterruptDescriptorTableEntry(0, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
              handlers[0] = nullptr;

            }

            ~InterruptManager(){
                Deactivate();
            }
            virtual void Initialize()final{

              const uint8_t IDT_INTERRUPT_GATE = 0xE;

              SetInterruptDescriptorTableEntry(0x00, CodeSegment, &HandleException0x00, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x01, CodeSegment, &HandleException0x01, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x02, CodeSegment, &HandleException0x02, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x03, CodeSegment, &HandleException0x03, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x04, CodeSegment, &HandleException0x04, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x05, CodeSegment, &HandleException0x05, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x06, CodeSegment, &HandleException0x06, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x07, CodeSegment, &HandleException0x07, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x08, CodeSegment, &HandleException0x08, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x09, CodeSegment, &HandleException0x09, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x0A, CodeSegment, &HandleException0x0A, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x0B, CodeSegment, &HandleException0x0B, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x0C, CodeSegment, &HandleException0x0C, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x0D, CodeSegment, &HandleException0x0D, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x0E, CodeSegment, &HandleException0x0E, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x0F, CodeSegment, &HandleException0x0F, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x10, CodeSegment, &HandleException0x10, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x11, CodeSegment, &HandleException0x11, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x12, CodeSegment, &HandleException0x12, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(0x13, CodeSegment, &HandleException0x13, 0, IDT_INTERRUPT_GATE);

              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x00, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x01, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x02, CodeSegment, &HandleInterruptRequest0x02, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x03, CodeSegment, &HandleInterruptRequest0x03, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x04, CodeSegment, &HandleInterruptRequest0x04, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x05, CodeSegment, &HandleInterruptRequest0x05, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x06, CodeSegment, &HandleInterruptRequest0x06, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x07, CodeSegment, &HandleInterruptRequest0x07, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x08, CodeSegment, &HandleInterruptRequest0x08, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x09, CodeSegment, &HandleInterruptRequest0x09, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0A, CodeSegment, &HandleInterruptRequest0x0A, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0B, CodeSegment, &HandleInterruptRequest0x0B, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0D, CodeSegment, &HandleInterruptRequest0x0D, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0E, CodeSegment, &HandleInterruptRequest0x0E, 0, IDT_INTERRUPT_GATE);
              SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0F, CodeSegment, &HandleInterruptRequest0x0F, 0, IDT_INTERRUPT_GATE);

              programmableInterruptControllerMasterCommandPort.Write(0x11);
              programmableInterruptControllerSlaveCommandPort.Write(0x11);

              // remap
              programmableInterruptControllerMasterDataPort.Write(hardwareInterruptOffset);
              programmableInterruptControllerSlaveDataPort.Write(hardwareInterruptOffset+8);

              programmableInterruptControllerMasterDataPort.Write(0x04);
              programmableInterruptControllerSlaveDataPort.Write(0x02);

              programmableInterruptControllerMasterDataPort.Write(0x01);
              programmableInterruptControllerSlaveDataPort.Write(0x01);

              programmableInterruptControllerMasterDataPort.Write(0x00);
              programmableInterruptControllerSlaveDataPort.Write(0x00);

              InterruptDescriptorTablePointer idt_pointer;
              idt_pointer.size  = 256*sizeof(GateDescriptor) - 1;
              idt_pointer.base  = (uint32_t)interruptDescriptorTable;
              asm volatile("lidt %0" : : "m" (idt_pointer));

            }

            static uint16_t HardwareInterruptOffset(){
                return hardwareInterruptOffset;
            }

            virtual void Activate() final
            {
              if(ActiveInterruptManager == 0)
              {
                ActiveInterruptManager = this;
                asm("sti");
              }
            }

            virtual void Deactivate() final
            {
              if(ActiveInterruptManager == this)
              {
                ActiveInterruptManager = 0;

                asm("cli");

              }
            }

    };
#endif
