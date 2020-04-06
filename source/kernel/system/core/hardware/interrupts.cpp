
#include <kernel//system//core//hardware//interrupts.hpp>
#include <kernel//system//core//outputstream.hpp>

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager* InterruptManager::ActiveInterruptManager = nullptr;


InterruptHandler::InterruptHandler(InterruptManager* interruptManager, uint8_t InterruptNumber)
{
    this->InterruptNumber = InterruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[InterruptNumber] = this;
}


InterruptHandler::~InterruptHandler()
{
    if(interruptManager->handlers[InterruptNumber] == this)
        interruptManager->handlers[InterruptNumber] = nullptr;
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
    return esp;
}


uint32_t InterruptManager::HandleInterrupt(uint8_t interrupt, uint32_t esp)
{
  if(ActiveInterruptManager != 0)
      return ActiveInterruptManager->DoHandleInterrupt(interrupt, esp);
  return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interrupt, uint32_t esp){
  if(handlers[interrupt] != nullptr)
  {
      esp = handlers[interrupt]->HandleInterrupt(esp);
  }
  else if(interrupt != hardwareInterruptOffset)
  {
      cout<<"UNHANDLED INTERRUPT 0x"<<interrupt;
      cout<<(hardwareInterruptOffset <= interrupt && interrupt < hardwareInterruptOffset+16)<<endl;
  }

  if(interrupt == hardwareInterruptOffset)
  {
  //    esp = (uint32_t)taskManager->Schedule((CPUState*)esp);
  }

  // hardware interrupts must be acknowledged
  if(hardwareInterruptOffset <= interrupt && interrupt < hardwareInterruptOffset+16)
  {
      programmableInterruptControllerMasterCommandPort.Write(0x20);
      if(hardwareInterruptOffset + 8 <= interrupt)
          programmableInterruptControllerSlaveCommandPort.Write(0x20);
  }

  return esp;
}
