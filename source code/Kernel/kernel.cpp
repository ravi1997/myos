typedef struct // http://www.lowlevel.eu/wiki/Multiboot
{
    uint32_t flags;
    uint32_t memLower;
    uint32_t memUpper;
    uint32_t bootdevice;
    uint32_t cmdline;
    uint32_t modsCount;
    void*    mods;
    uint32_t syms[4];
    uint32_t mmapLength;
    void*    mmap;
    uint32_t drivesLength;
    void*    drives;
    uint32_t configTable;
    char*    bootloaderName;
    uint32_t apmTable;
    uint32_t vbe_controlInfo;
    uint32_t vbe_modeInfo;
    uint16_t vbe_mode;
    uint16_t vbe_interfaceSeg;
    uint16_t vbe_interfaceOff;
    uint16_t vbe_interfaceLen;
} __attribute__((packed)) multiboot_t;

extern char _bss_start; // Linker script
extern char _bss_end;   // Linker script


void init(multiboot_t* mb_struct){
  // Set .bss to zero
  memset(&_bss_start, 0, (uintptr_t)&_bss_end - (uintptr_t)&_bss_start);

  Descriptor::GDT_Install();
  
}

void main(multiboot_t* mb_struct)
{
  init(mb_struct);
}
