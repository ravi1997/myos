#ifndef _SERVICE_H_
#define _SERVICE_H_ 1

#include <kernel//system//core//hardware//interrupts.hpp>
#include <kernel//system//core//hardware//driver//Driver.hpp>
#include <kernel//system//core//hardware//driver//keyboard.hpp>
#include <kernel//system//core//hardware//driver//mouse.hpp>
#include <kernel//system//core//cpu.hpp>


class Service{
  GlobalDescriptorTable gdtManager;
  InterruptManager isrManager;
  DriverManager driverManager;

  PrintfKeyboardEventHandler  keyHandler;
  KeyboardDriver kDriver;

  MouseToConsole mouseHandler;
  MouseDriver mDriver;

  CPU cpu;

public:
  Service():
    isrManager{gdtManager},
    driverManager{isrManager},
    kDriver{&isrManager,&keyHandler},
    mDriver{&isrManager,&mouseHandler}
    {}

  void Initialize(){
    gdtManager.Initialize();
    gdtManager.Activate();
    isrManager.Initialize();

    driverManager.addDriver(&mDriver);
    driverManager.addDriver(&kDriver);


  }
  void Activate(){
    driverManager.Activate();
    isrManager.Activate();
    cpu.Activate();
    //cpu.cpu_analyze();
  }
  void Loop(){}
  void Stop(){}
};

#endif
