#ifndef _IQR_HPP_
#include<system//iqr.hpp>
#endif
uint_32 InterruptServiceRoutine::HandleInterrupt(uint_8 interruptNumber,uint_32 esp){
  return esp;
}

void InterruptServiceRoutine::HandleInterruptRequest0x01(){}
