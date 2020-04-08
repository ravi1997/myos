#ifndef _PAGING_HPP_
#define _PAGING_HPP_

#include <common//types.hpp>
#include <kernel//system//tasking//synchronisation.hpp>
#include <kernel//system//core//outputstream.hpp>
#include <util//util.hpp>


#define PAGESIZE   0x1000 // Size of one page in bytes
#define PAGE_COUNT 1024   // Number of pages per page table
#define PT_COUNT   1024   // Number of page tables per page directory

#define CR0_PAGINGENABLED    BIT(31)
#define CR0_WRITEPROTECT     BIT(16);

#define IA32_MTRRCAP 0xFE
#define IA32_MTRR_DEF_TYPE 0x2FF
#define IA32_MTRR_FIX64K_00000 0x250
#define IA32_MTRR_FIX16K_80000 0x258
#define IA32_MTRR_FIX4K_C0000 0x268
#define IA32_MTRR_PHYSBASE0 0x200
#define IA32_MTRR_PHYSMASK0 0x201

typedef enum
{
    MEM_PRESENT      = BIT(0),
    MEM_WRITE        = BIT(1),
    MEM_USER         = BIT(2),
    MEM_WRITETHROUGH = BIT(3),
    MEM_NOCACHE      = BIT(4),
    MEM_NOTLBUPDATE  = BIT(8),
    MEM_ALLOCATED    = BIT(9), // PrettyOS-specific. Indicates if a page is allocated or not
    MEM_CONTINUOUS   = BIT(10) // PrettyOS-specific. Used as function argument to request physically continuous memory
} MEMFLAGS_t;

typedef enum {
    MTRR_UNCACHABLE     = 0x00,
    MTRR_WRITECOMBINING = 0x01,
    MTRR_WRITETHROUGH   = 0x04,
    MTRR_WRITEPROTECTED = 0x05,
    MTRR_WRITEBACK      = 0x06
} MTRR_CACHETYPE;

// Memory Map
typedef struct
{
    uint32_t mysize; // Size of this entry
    uint64_t base;   // The region's address
    uint64_t size;   // The region's size
    uint32_t type;   // Is "1" for "free"
} __attribute__((packed)) memoryMapEntry_t;

// Paging
typedef struct
{
    uint32_t pages[PAGE_COUNT];
} __attribute__((packed)) pageTable_t;

typedef struct
{
    uint32_t     codes[PT_COUNT];
    pageTable_t* tables[PT_COUNT];
    uint32_t     physAddr;
    mutex_t      mutex;
} __attribute__((packed)) pageDirectory_t;



class Paging{
public:
  uint32_t paging_install(memoryMapEntry_t* memoryMapBegin, size_t memoryMapLength)
  {
      uint32_t ram_available = physMemInit(memoryMapBegin, memoryMapLength);

      // Setup the kernel page directory
      kernelPageDirectory = malloc(sizeof(pageDirectory_t), PAGESIZE, "pag-kernelPD");
      memset(kernelPageDirectory, 0, sizeof(pageDirectory_t) - 4);
      kernelPageDirectory->physAddr = (uintptr_t)kernelPageDirectory;
      mutex_construct(&kernelPageDirectory->mutex);

      // Setup the page tables for 0 MiB - 12 MiB, identity mapping
      uintptr_t addr = 0;
      for (uint8_t i=0; i<IDMAP; i++)
      {
          // Page directory entry, virt=phys due to placement allocation in id-mapped area
          kernelPageDirectory->tables[i] = malloc(sizeof(pageTable_t), PAGESIZE, "pag-kernelPT");
          kernelPageDirectory->codes[i]  = (uint32_t)kernelPageDirectory->tables[i] | MEM_PRESENT | MEM_WRITE;

          // Page table entries, identity mapping
          for (uint32_t j = 0; j < PAGE_COUNT; ++j)
          {
              uint32_t flags;
              if (addr < 0x100000) // The first MiB has to be flushed from TLB, since VM86 adds MEM_USER and breaks identity mapping
                  flags = MEM_PRESENT | MEM_WRITE | MEM_ALLOCATED;
              else
                  flags = MEM_PRESENT | MEM_WRITE | MEM_NOTLBUPDATE | MEM_ALLOCATED;
              kernelPageDirectory->tables[i]->pages[j] = addr | flags;
              addr += PAGESIZE;
          }
      }
      kernelPageDirectory->tables[0]->pages[0] = 0 | MEM_PRESENT | MEM_ALLOCATED; // Make first page read-only

      // Setup the page tables for kernel heap (3,5 - 4 GiB), unmapped
      size_t kernelpts = min(PT_COUNT / 8, ram_available / PAGESIZE / PAGE_COUNT); // Do not allocate more PTs than necessary (limited by available memory)
      pageTable_t* heap_pts = malloc(kernelpts*sizeof(pageTable_t), PAGESIZE, "kheap_pts");
      memset(heap_pts, 0, kernelpts * sizeof(pageTable_t));
      for (uint32_t i = 0; i < kernelpts; i++)
      {
          kernelPageDirectory->tables[KERNEL_HEAP_START/PAGESIZE/PAGE_COUNT + i] = heap_pts + i;
          kernelPageDirectory->codes [KERNEL_HEAP_START/PAGESIZE/PAGE_COUNT + i] = (uint32_t)(heap_pts + i) | MEM_PRESENT;
      }

      // Make some parts of the kernel (Sections text and rodata) read-only
      uint32_t startpt = ((uintptr_t)&_ro_start)/PAGESIZE/PAGE_COUNT; // Page table, where read-only section starts
      uint32_t startp = ((uintptr_t)&_ro_start)/PAGESIZE%PAGE_COUNT; // Page, where read-only section starts
      if ((uintptr_t)&_ro_start%PAGESIZE != 0)
      {
          startp++;
          if (startp > PAGE_COUNT)
          {
              startpt++;
              startp = 0;
          }
      }
      uint32_t endpt = ((uintptr_t)&_ro_end)/PAGESIZE/PAGE_COUNT; // Page table, where read-only section ends
      uint32_t endp = ((uintptr_t)&_ro_end)/PAGESIZE%PAGE_COUNT; // Page, where read-only section ends
      if (endp > 0)
          endp--;
      else
      {
          endp = PAGE_COUNT-1;
          endpt--;
      }
      for (uint32_t i = startpt; i <= endpt; i++)
      {
          for (uint32_t j = startp; j <= endp; j++)
          {
              kernelPageDirectory->tables[i]->pages[j] &= (~MEM_WRITE); // Forbid writing
          }
      }

      // Tell CPU to enable paging
      paging_switch(kernelPageDirectory);
      uint32_t cr0;
      __asm__("mov %%cr0, %0": "=r"(cr0)); // read CR0
      cr0 |= CR0_PAGINGENABLED | CR0_WRITEPROTECT;
      __asm__("mov %0, %%cr0":: "r"(cr0)); // write CR0

      return (ram_available);
  }
private:
  pageDirectory_t* kernelPageDirectory;
  uint32_t physMemInit(memoryMapEntry_t* memoryMapBegin, size_t memoryMapLength)
  {
      memoryMapEntry_t* memoryMapEnd = (memoryMapEntry_t*)((char*)memoryMapBegin + memoryMapLength);

      // Prepare the memory map entries, since we work with max 4 GB only. The last entry in the entry-array has size 0.
      for (memoryMapEntry_t* entry = memoryMapBegin; entry < memoryMapEnd; entry = (memoryMapEntry_t*)((char*)entry + entry->mysize + 4))
      {

          // We will completely ignore memory above 4 GB or with size of 0, move following entries backward by one then
          if (entry->base < FOUR_GB && entry->size != 0)
          {
              // Eventually reduce the size so the the block doesn't exceed 4 GB
              if (entry->base + entry->size >= FOUR_GB)
              {
                  entry->size = FOUR_GB - entry->base;
              }

              if (entry->type == 1)
                  MAX_DWORDS = max(MAX_DWORDS, (entry->base+entry->size)/PAGESIZE/32); // Calculate required size of bittables
          }
      }

      // Check that 6 MiB-12 MiB is free for use
      if (!isMemoryMapAvailable(memoryMapBegin, memoryMapEnd, PLACEMENT_BEGIN, IDMAP*PAGE_COUNT*PAGESIZE))
      {
          cout<<"The memory between 6 MiB and 12 MiB is not free for use. OS halted!"<<endl;
          cli();
          hlt();
      }

      // We store our data here, initialize all bits to "reserved"
      phys_reservationTable = malloc(MAX_DWORDS * 4, 0, "phys_reservationTable");
      memset(phys_reservationTable, 0xFF, MAX_DWORDS * 4);
      phys_blockingTable = malloc(MAX_DWORDS * 4, 0, "phys_blockingTable");

      // Set the bitmap bits according to the memory map now. "type==1" means "free".
      for (const memoryMapEntry_t* entry = memoryMapBegin; entry < memoryMapEnd; entry = (memoryMapEntry_t*)((char*)entry + entry->mysize + 4))
      {
          if (entry->type == 1 && entry->base < FOUR_GB) // Set bits to "free"
          {
              physSetBits(entry->base, entry->base+entry->size, false);
          }
      }

      // Find the number of dwords we can use, skipping the last, "reserved"-only ones
      uint32_t dwordCount = 0;

      for (uint32_t i=0; i<MAX_DWORDS; i++)
      {
          if (phys_reservationTable[i] != 0xFFFFFFFF)
          {
              dwordCount = i + 1;
          }
      }

      // Reserve first 12 MiB
      physSetBits(0x00000000, PLACEMENT_END-1, true);

      // Reserve the region of the kernel code
      if ((uintptr_t)&_kernel_end >= PLACEMENT_END)
          physSetBits((uint32_t)&_kernel_beg, (uint32_t)&_kernel_end, true);

      // Return the amount of memory available (or rather the highest address)
      return (dwordCount * 32 * PAGESIZE);
  }
};

#endif
