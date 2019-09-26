#ifndef _SERVICE_HPP_
#define _SERVICE_HPP_ 1

#ifndef _GDT_HPP_
#include<system//gdt.hpp>
#endif
#include<system//iqr.hpp>


class Service{
public:
  static void initialize(){
    GlobalDescriptorTableManager::Initialize();
    InterruptServiceRoutine::Initialize();
  }
  static void run(){
    GlobalDescriptorTableManager::Activate();
    InterruptServiceRoutine::Activate();
  }
  static void loop(){}
  static void stop(){}
};

#endif
