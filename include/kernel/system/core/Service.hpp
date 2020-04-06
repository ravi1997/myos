#ifndef _SERVICE_H_
#define _SERVICE_H_ 1

#include <kernel//system//core//hardware//interrupts.hpp>
#include <kernel//system//core//hardware//driver//Driver.hpp>
#include <kernel//system//core//hardware//driver//keyboard.hpp>

class Service{
  GlobalDescriptorTable gdtManager;
  InterruptManager isrManager;
  DriverManager driverManager;

  PrintfKeyboardEventHandler  keyHandler;
  KeyboardDriver kDriver;


public:
  Service():
    isrManager{gdtManager},
    driverManager{isrManager},
    kDriver{&isrManager,&keyHandler}
    {}

  void Initialize(){
    gdtManager.Initialize();
    gdtManager.Activate();
    isrManager.Initialize();

    driverManager.addDriver(&kDriver);

  }
  void Activate(){
    driverManager.Activate();
    isrManager.Activate();

  }
  void Loop(){}
  void Stop(){}
};

#endif
