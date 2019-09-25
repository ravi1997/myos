#include<system//iqr.hpp>

uint_32 InterruptServiceRoutine::HandleInterrupt(uint_8 interruptNumber,uint_32 esp){
  return esp;
}

void InterruptServiceRoutine::handleInterruptRequest0x01(){}
