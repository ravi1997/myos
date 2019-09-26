#ifndef _SERVICE_HPP_
#include<system//service.hpp>
#endif


  void printf(char const* str ){
  static unsigned short* VideoMemory = (uint_16 *)0xb8000;
  static uint_8 x=0,y=0;
  for(int i=0;str[i]!='\0';i++){
    switch(str[i]){
      case '\n':
        y++;
        x=0;
        break;
      default:
        VideoMemory[80*y+x]=(VideoMemory[80*y+x]&0xFF00)|str[i];
        x++;
        break;
    }
    if(x==80){
      x=0;
      y++;
    }
    if(y==25){
      for(int i=0;i<80;i++)
      for(int j=0;j<25;j++)
      VideoMemory[80*j+i]=(VideoMemory[80*j+i]&0xFF00)|' ';
      x=0,y=0;
    }
  }
}
void printfHex(uint_8 key)
{
    char *foo = "00";
    char const* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}
void printfHex16(uint_16 key)
{
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}
void printfHex32(uint_32 key)
{
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}

typedef void (*constructor)();

extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){
  for(constructor* i=&start_ctors;i!=&end_ctors;i++)
    (*i)();
}

extern "C" void kernelMain([[maybe_unused]]void *multiboot_structure,[[maybe_unused]]uint_32 magicnumber){
  printf("Hardware initialize\n");
  Service::initialize();
  printf("service started\n");
  Service::run();
  printf("IDT: Activated");

  while(1)
    Service::loop();
}
