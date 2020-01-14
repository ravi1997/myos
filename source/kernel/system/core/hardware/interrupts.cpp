
#include <kernel//system//core//hardware//interrupts.h>


InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

uint32_t InterruptManager::HandleInterrupt(uint8_t interrupt, uint32_t esp)
{
  cout<<"INTERRUPT 0x"<<OutputStream::type::HEX<<interrupt<<endl;
  return esp;
}
