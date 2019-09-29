#ifndef _IQR_HPP_
#include<system//iqr.hpp>
#endif

uint_32 InterruptServiceRoutine::HandleInterrupt(uint_8 interruptNumber,uint_32 esp){
    printf("UNHANDLED INTERRUPT 0x");
    printfHex(interruptNumber);
    return (interruptEnable)?DoHandleInterrupt(interruptNumber,esp):esp;
}


InterruptServiceRoutine::InterruptHandler* InterruptServiceRoutine::handlers[256];
bool InterruptServiceRoutine::interruptEnable=false;
InterruptServiceRoutine::GateDescriptor InterruptServiceRoutine::interruptDescriptorTable[256];
Port8BitSlow InterruptServiceRoutine::programmableInterruptControllerMasterCommandPort{0x20};
Port8BitSlow InterruptServiceRoutine::programmableInterruptControllerMasterDataPort{0x21};
Port8BitSlow InterruptServiceRoutine::programmableInterruptControllerSlaveCommandPort{0xA0};
Port8BitSlow InterruptServiceRoutine::programmableInterruptControllerSlaveDataPort{0xA1};
