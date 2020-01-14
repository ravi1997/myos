#ifndef _SERVICE_H_
#define _SERVICE_H_ 1

#include <kernel//system//core//hardware//interrupts.h>

class Service{
  GlobalDescriptorTable gdtManager;
  InterruptManager isrManager;
public:
  Service():
    isrManager{gdtManager}
    {}
  void Initialize(){
    gdtManager.Initialize();
    gdtManager.Activate();
    isrManager.Initialize();
  }
  void Activate(){
    isrManager.Activate();
  }
  void Loop(){}
  void Stop(){}
};

#endif
