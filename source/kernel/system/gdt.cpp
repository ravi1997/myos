#ifndef _GDT_HPP_
#include<system//gdt.hpp>
#endif


GlobalDescriptorTableManager::GlobalDescriptor GlobalDescriptorTableManager::gdt[MAXGDT];
uint_32 GlobalDescriptorTableManager::GDT_Entries=0;
