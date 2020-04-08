#ifndef _CPU_HPP_
#define _CPU_HPP_

#include<common//types.hpp>
#include <kernel//system//core//outputstream.hpp>
#include <util/util.hpp>

static inline uint64_t rdtsc(void)
{
    uint64_t val;
    __asm__ volatile ("rdtsc" : "=A"(val)); // "=A" for getting 64 bit value
    return val;
}


typedef enum
{ // Uses bits larger than 5 because the bitnumber inside the registers can be 31 at maximum which needs the first 5 bits
    CR_EAX = 1U<<5,
    CR_EBX = 1U<<6,
    CR_ECX = 1U<<7,
    CR_EDX = 1U<<8
} CPU_REGISTER;

typedef enum
{ // In this enum the last five bits contain the bit in the register, the other bits are the register (see above)
    CF_CPUID        = 0,

    CF_FPU          = CR_EDX|0,
    CF_VME86        = CR_EDX|1,
    CF_IOBREAKPOINT = CR_EDX|2,
    CF_PAGES4MB     = CR_EDX|3,
    CF_RDTSC        = CR_EDX|4,
    CF_MSR          = CR_EDX|5,
    CF_PAE          = CR_EDX|6,
    CF_MCE          = CR_EDX|7,
    CF_X8           = CR_EDX|8,
    CF_APIC         = CR_EDX|9,
    CF_SYSENTEREXIT = CR_EDX|11,
    CF_MTRR         = CR_EDX|12,
    CF_PGE          = CR_EDX|13,
    CF_MCA          = CR_EDX|14,
    CF_CMOV         = CR_EDX|15,
    CF_PAT          = CR_EDX|16,
    CF_PSE36        = CR_EDX|17,
    CF_CLFLUSH      = CR_EDX|19,
    CF_MMX          = CR_EDX|23,
    CF_FXSR         = CR_EDX|24,
    CF_SSE          = CR_EDX|25,
    CF_SSE2         = CR_EDX|26,
    CF_HTT          = CR_EDX|28,

    CF_SSE3         = CR_ECX|0,
    CF_PCLMULQDQ    = CR_ECX|1,
    CF_MONITOR      = CR_ECX|3,
    CF_SSSE3        = CR_ECX|9,
    CF_FMA          = CR_ECX|12,
    CF_CMPXCHG16B   = CR_ECX|13,
    CF_SSE41        = CR_ECX|19,
    CF_SSE42        = CR_ECX|20,
    CF_MOVBE        = CR_ECX|22,
    CF_POPCNT       = CR_ECX|23,
    CF_AESNI        = CR_ECX|25,
    CF_XSAVE        = CR_ECX|26,
    CF_OSXSAVE      = CR_ECX|27,
    CF_AVX          = CR_ECX|28,
    CF_F16C         = CR_ECX|29,
    CF_RDRAND       = CR_ECX|30,
} CPU_FEATURE;

class CPU{
  bool cpuid_available=false;
  bool supports_rdtsc = false;
  int64_t cpu_frequency;

public:

  void Activate(void)
  {
      cpu_calculateFrequency();
      // Test if the CPU supports the CPUID-Command
      uint32_t result = 0;
      __asm__ volatile("pushfl\n"
                       "pop %%ecx\n"
                       "mov %%ecx, %%eax\n"
                       "xor %%eax, 0x200000\n"
                       "push %%eax\n"
                       "popfl\n"
                       "pushfl\n"
                       "pop %%eax\n"
                       "sub %%ecx, %%eax\n" : "=a"(result) : : "ecx");
      cpuid_available = (result == 0);

      if (cpu_supports(CF_PGE)) // We take this to indicate availability of CR4 register
          __asm__("mov %%cr4, %%eax;"
                  "or $0x00000080, %%eax;" // Activate PGE
                  "mov %%eax, %%cr4" : : : "eax");
      if (cpu_supports(CF_FXSR))
          __asm__("mov %%cr4, %%eax;"
                  "or $0x00000200, %%eax;" // Activate OSFXSR
                  "mov %%eax, %%cr4" : : : "eax");
      if (cpu_supports(CF_SSE))
          __asm__("mov %%cr4, %%eax;"
                  "or $0x00000400, %%eax;" // Activate OSXMMEXCPT
                  "mov %%eax, %%cr4" : : : "eax");

      if (cpuid_available)
        supports_rdtsc = cpu_supports(CF_RDTSC);
  }

  void cpu_analyze(void)
  {
      if (!cpuid_available)
      {
          cout<<"   => CPU: ";
          cout<<"Does not support cpuid instruction."<<endl;
          return;
      }

      cout<<"   => CPU:"<<endl;
      cout<<"     => Sysenter : "<<cpu_supports(CF_SYSENTEREXIT)<<endl;
      cout<<"     => PGE : "<<cpu_supports(CF_PGE)<<endl;
      cout<<"     => FXSR : "<<cpu_supports(CF_FXSR)<<endl;
      cout<<"     => RDTSC : "<<supports_rdtsc<<endl;
      cout<<"     => SSE : "<<cpu_supports(CF_SSE)<<endl;
      cout<<"     => MTRR : "<<cpu_supports(CF_MTRR)<<endl;

      // Read out VendorID
      char cpu_vendor[13];
      ((uint32_t*)cpu_vendor)[0] = cpu_idGetRegister(0, CR_EBX);
      ((uint32_t*)cpu_vendor)[1] = cpu_idGetRegister(0, CR_EDX);
      ((uint32_t*)cpu_vendor)[2] = cpu_idGetRegister(0, CR_ECX);
      cpu_vendor[12] = 0;

      cout<<"     => VendorID : "<<cpu_vendor<<endl;
      //ipc_setString("PrettyOS/CPU/VendorID", cpu_vendor);

  }

  void cpu_calculateFrequency(void)
  {
      if (!supports_rdtsc)
          return;

      static uint64_t LastRdtscValue = 0; // rdtsc: read time-stamp counter

      // calculate cpu frequency
      uint64_t Rdtsc = rdtsc();
      uint64_t RdtscKCounts   = (Rdtsc - LastRdtscValue);  // Build difference
      uint32_t RdtscKCountsHi = RdtscKCounts >> 32;        // high dword
      uint32_t RdtscKCountsLo = RdtscKCounts & 0xFFFFFFFF; // low dword
      LastRdtscValue = Rdtsc;

      if (RdtscKCountsHi == 0)
          cpu_frequency = RdtscKCountsLo/1000;
  }

  bool cpu_supports(CPU_FEATURE feature)
  {
      if (feature == CF_CPUID)
          return (cpuid_available);

      if (!cpuid_available)
          return (false);

      CPU_REGISTER r = (CPU_REGISTER)(feature&(CR_EBX|CR_ECX|CR_EDX));

      return (cpu_idGetRegister(0x00000001, r) & (BIT(feature-r)));
  }

  uint32_t cpu_idGetRegister(uint32_t function, CPU_REGISTER reg)
  {
      if (!cpuid_available) return (0);

      switch (reg)
      {
          case CR_EAX:
          {
              uint32_t temp;
              __asm__("cpuid" : "=a"(temp) : "a"(function) : "ebx", "ecx", "edx");
              return (temp);
          }
          case CR_EBX:
          {
              uint32_t temp, temp2;
              __asm__("cpuid" : "=b"(temp), "=a"(temp2) : "a"(function) : "ecx", "edx");
              return (temp);
          }
          case CR_ECX:
          {
              uint32_t temp, temp2;
              __asm__("cpuid" : "=c"(temp), "=a"(temp2) : "a"(function) : "ebx", "edx");
              return (temp);
          }
          case CR_EDX:
          {
              uint32_t temp, temp2;
              __asm__("cpuid" : "=d"(temp), "=a"(temp2) : "a"(function) : "ebx", "ecx");
              return (temp);
          }
          default:
              return (0);
      }
  }

  uint64_t cpu_MSRread(uint32_t msr)
  {
      uint32_t low, high;

      __asm__("rdmsr" : "=a" (low), "=d" (high) : "c" (msr));

      return ((uint64_t)high << 32) | low;
  }

  void cpu_MSRwrite(uint32_t msr, uint64_t value)
  {
      uint32_t low = value & 0xFFFFFFFF;
      uint32_t high = value >> 32;

      __asm__("wrmsr" :: "a"(low), "c"(msr), "d"(high));
  }

};

#endif
