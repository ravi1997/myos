#ifndef _SERVICE_HPP_
#include<system//service.hpp>
#endif

#include<system//outputstream.hpp>
extern "C" void myInterrupt();


typedef void (*constructor)();

extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){
  for(constructor* i=&start_ctors;i!=&end_ctors;i++)
    (*i)();
}

extern "C" void kernelMain([[maybe_unused]]void *multiboot_structure,[[maybe_unused]]uint_32 magicnumber){

  Service s;

  cout<<"Hardware initialize"<<endl;
  s.initialize();
  cout<<"service started"<<endl;
  s.run();

//  myInterrupt();

  cout<<"Service: Activated"<<endl;

  //GlobalDescriptorTableManager::gdtDebug();

  while(1)
    s.loop();
}
