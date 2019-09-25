#ifndef _SERVICE_HPP_
#define _SERVICE_HPP_ 1

#ifndef _GDT_HPP_
#include<system//gdt.hpp>
#endif

class Service{
public:
  static void initialize(){
    GlobalDescriptorTableManager::Initialize();
  }
  static void run(){
    GlobalDescriptorTableManager::Activate();
  }
  static void loop(){}
  static void stop(){}
};

#endif
