
#include <kernel//system//core//hardware//interrupts.hpp>
#include <kernel//system//core//Service.hpp>

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}
extern "C" void __cxa_pure_virtual() { while (1); }


extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    cout<<"Hello World! --- http://www.AlgorithMan.de"<<endl;
    Service s;
    s.Initialize();
    s.Activate();

    while (1) {
      s.Loop();
    }
    /*
    GlobalDescriptorTable gdt;
//    GlobalDescriptorTable::initialize();
    //cout<<OutputStream::type::HEX<<GlobalDescriptorTable::CodeSegmentSelector()<<endl;
    InterruptManager interrupts{gdt};
    interrupts.Activate();

    while(1);*/
}
