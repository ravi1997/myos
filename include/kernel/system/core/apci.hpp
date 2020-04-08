#ifndef _APCI_HPP_
#define _APCI_HPP_ 1

#include<util/util.hpp>

#define IA32_APIC_BASE_MSR          0x1B
#define IA32_APIC_BASE_BSP          0x100
#define IA32_APIC_BASE_MSR_ENABLE   0x800

#define APIC_SW_ENABLE              BIT(8)
#define APIC_CPUFOCUS               BIT(9)
#define APIC_NMI                    BIT(10)
#define APIC_LEVEL                  BIT(13)
#define APIC_LOW                    BIT(15)
#define APIC_INTERRUPTDISABLED      BIT(16)

#define TMR_PERIODIC                BIT(17)

#define APIC_EXT_INT                (BIT(8) | BIT(9) | BIT(10))


// APIC registers (as indices of uint32_t*)
enum {
    APIC_APICID             = 0x20  / 4,
    APIC_APICVERSION        = 0x30  / 4,
    APIC_TASKPRIORITY       = 0x80  / 4,
    APIC_EOI                = 0xB0  / 4, // End of Interrupt
    APIC_LDR                = 0xD0  / 4, // Logical Destination
    APIC_DFR                = 0xE0  / 4, // Destination Format
    APIC_SPURIOUSINTERRUPT  = 0xF0  / 4,
    APIC_ESR                = 0x280 / 4, // Error Status
    APIC_ICRL               = 0x300 / 4, // Interrupt Command Lo
    APIC_ICRH               = 0x310 / 4, // Interrupt Command Hi

    APIC_TIMER              = 0x320 / 4, // LVT Timer Register, cf. 10.5.1 Local Vector Table
    APIC_THERMALSENSOR      = 0x330 / 4, // LVT Thermal Monitor Register, cf. 10.5.1 Local Vector Table
    APIC_PERFORMANCECOUNTER = 0x340 / 4, // LVT Performance Counter Register, cf. 10.5.1 Local Vector Table
    APIC_LINT0              = 0x350 / 4, // LVT LINT0 Register, cf. 10.5.1 Local Vector Table
    APIC_LINT1              = 0x360 / 4, // LVT LINT1 Register, cf. 10.5.1 Local Vector Table
    APIC_ERROR              = 0x370 / 4, // LVT Error Register, cf. 10.5.1 Local Vector Table

    APIC_TIMER_INITCOUNT    = 0x380 / 4,
    APIC_TIMER_CURRENTCOUNT = 0x390 / 4,
    APIC_TIMER_DIVIDECONFIG = 0x3E0 / 4,
};

// IO APIC

/* Offset der memory-mapped Register */
#define IOAPIC_IOREGSEL      0x00
#define IOAPIC_IOWIN         0x10

#define IOAPIC_ID            0x00
#define IOAPIC_VERSION       0x01
#define IOAPIC_ARBITRATIONID 0x02


#define ACPI_TABLE_SIG_LEN 4 // Generic table signature length
#define ACPI_OEMID_LEN     6 // OemID length
#define ACPI_OEMTBLID_LEN  8 // Oem table ID length

#define EDBA_BEGIN (uint8_t*)0x0009FC00
#define EDBA_END (uint8_t*)0x000A0000 // Pointer to post-last byte of the edba

#define BIOS_BEGIN (uint8_t*)0x000E0000
#define BIOS_END (uint8_t*)0x00100000 // Pointer to post-last byte of the bios rom

#define ACPI_RDSP_SIG_LEN 8 // Root system descriptor pointer signature length


// Generic header of every table
typedef struct
{
    char signature[ACPI_TABLE_SIG_LEN]; // Table-specific ASCII-Signature (e.g. 'APIC')
    uint32_t len; // Size of the whole table including the header
    uint8_t rev;
    uint8_t checksum; // Whole table must add up to 0 to be valid
    char oemID[ACPI_OEMID_LEN];
    char oemTableID[ACPI_OEMTBLID_LEN];
    uint32_t oemRev;
    uint32_t creatorId;
    uint32_t creatorRev;
} __attribute__((packed)) acpi_header_t; // Generic header for every table

typedef struct
{
    char* signature;
    acpi_header_t* begin;
    uint32_t len;
} acpi_table_t;

typedef struct
{
    acpi_header_t header;
} __attribute__((packed)) acpi_rsdt_t;

typedef struct
{
    char sig[ACPI_RDSP_SIG_LEN]; // "RSD PTR "
    uint8_t checksum; // Checksum of the bytes 0-19 (defined in ACPI rev 1); bytes must add to 0
    char oemID[ACPI_OEMID_LEN];
    uint8_t rev;
    void* rootSystemTable;
    // All the fields above are used to calc the checksum
    //////////////////////////////////////////////////////////////////////////

    uint32_t rootSystemTableLen;
    uint64_t extendedRootSystemTableAddr; // 64 bit phys addr
    uint8_t checksumEx; // Checksum of the whole structure including the other checksum field
    uint8_t reserved[3];
} __attribute__((packed)) acpi_rsdp_t;

#include<kernel//system//core//parser.hpp>

class ACPI{
public:
  acpi_rsdp_t* rdsp=nullptr;
  list_t tables ;
  list_t parsers ;
  void acpi_install(void)
  {
      for (acpi_parser_t* it = acpi_baseParsers; it->parser != 0; ++it)
          list_append(&parsers, it);

      if (rdsp != nullptr)
      {
          acpi_parseRSDT(rdsp->rootSystemTable, &tables);
      }

      acpi_reparse();

      DIAGNOSIS();
  }
  void acpi_parseRSDT(acpi_rsdt_t* tbl, list_t* list)
  {
      // Mapping prologue
      // RSDT not guaranteed to be within mapped space
      // In order to find out, how many pages we have to map for the full RSDT, we first need to map the header...
      bool dealloc = false;
      uint32_t headerPages = (alignUp((uint32_t)tbl + sizeof(acpi_rsdt_t), PAGESIZE)
                              - alignDown((uint32_t)tbl, PAGESIZE)) / PAGESIZE;
      uint8_t* mapped = acpi_tryAlloc((uintptr_t)tbl, headerPages, &dealloc);
      if(!mapped)
        return;
      acpi_rsdt_t* table = (void*)(mapped + (uint32_t)tbl % PAGESIZE);
      // If the RSDT is longer than the original header-mapping, remap it
      uint32_t pages = (alignUp((uint32_t)tbl + table->header.len, PAGESIZE)
                        - alignDown((uint32_t)tbl, PAGESIZE)) / PAGESIZE;

      if (pages != headerPages)
      {
          acpi_tryDealloc(mapped, headerPages, dealloc);
          mapped = acpi_tryAlloc((uintptr_t)tbl, pages, &dealloc);
          table = (void*)(mapped + (uint32_t)tbl % PAGESIZE);
      }
      // Mapping prologue end

      if (memcmp(table->header.signature, ACPI_TABSIG_RSDT, ACPI_TABLE_SIG_LEN) != 0) // Signature failed
      {
          acpi_tryDealloc(table, pages, dealloc);
          return;
      }

      // Iterate over the table entries following the header
      for (uint32_t* it = (uint32_t*)((uint8_t*)table + sizeof(acpi_rsdt_t));
              it != (uint32_t*)((uint8_t*)table + table->header.len); ++it)
      {
          acpi_table_t* entry = acpi_analyzeRSDTEntry((acpi_header_t*)*it);
          list_append(list, entry);
      }

      acpi_tryDealloc(mapped, pages, dealloc);
  }
};

#endif
