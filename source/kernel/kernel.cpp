#ifndef _SERVICE_HPP_
#include<system//service.hpp>
#endif

#include<system//outputstream.hpp>


typedef void (*constructor)();

extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){
  for(constructor* i=&start_ctors;i!=&end_ctors;i++)
    (*i)();
}

extern "C" void kernelMain([[maybe_unused]]void *multiboot_structure,[[maybe_unused]]uint_32 magicnumber){
  cout<<"Hardware initialize"<<endl;
  Service::initialize();
  cout<<"service started"<<endl;
  Service::run();
  cout<<"Service: Activated"<<endl;

  while(1)
    Service::loop();
}
