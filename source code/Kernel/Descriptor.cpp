#include "Descriptor.hpp"

const GlobalDescriptorTable::GlobalDescriptorTable* object=new GlobalDescriptorTable();

static TSSentry_t tss = {};

GlobalDescriptorTable::GlobalDescriptorTable()
    : nullSegmentSelector(0, 0, 0, 0, 0),
        kernelcodeSegmentSelector(0, 0xFFFFF, VALID | RING_0 | CODE_DATA_STACK | CODE_EXEC_READ,  _4KB_ | USE32),
        kerneldataSegmentSelector(  0,   0xFFFFF, VALID | RING_0 | CODE_DATA_STACK | DATA_READ_WRITE, _4KB_ | USE32),
        usercodeSegmentSelector(0,0,   0xFFFFF, VALID | RING_3 | CODE_DATA_STACK | CODE_EXEC_READ,  _4KB_ | USE32),
        userdataSegmentSelector(0,   0xFFFFF, VALID | RING_3 | CODE_DATA_STACK | DATA_READ_WRITE, _4KB_ | USE32),
        taskStateSegmentSelector((uint32_t)&tss,sizeof(tss),0xE9,0x00)
{

  tss.ss0  = 0x10;  // Set the kernel stack segment.
  tss.esp0 = 0x0; // Set the kernel stack pointer.

  tss.cs   = 0x08;
  tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x10;
}

GlobalDescriptorTable::~GlobalDescriptorTable()
{
}

uint16_t GlobalDescriptorTable::DataSegmentSelector()
{
    return (uint8_t*)&dataSegmentSelector - (uint8_t*)this;
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
    return (uint8_t*)&codeSegmentSelector - (uint8_t*)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t access,uint8_t gran)
{

    // Setup the descriptor base address
    this->base_low    =  base & 0xFFFF;
    this->base_middle = (base >> 16) & 0xFF;
    this->base_high   = (base >> 24) & 0xFF;

    // Setup the descriptor limits
    this->limit_low   = limit & 0xFFFF;
    this->granularity = (limit >> 16) & 0x0F;

    // Finally, set up the granularity and access flags
    this->granularity |= (gran & 0xF0);
    this->access      = access;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base()
{
    uint8_t* target = (uint8_t*)this;

    uint32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];

    return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit()
{
    uint8_t* target = (uint8_t*)this;

    uint32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xFFF;

    return result;
}

GlobalDescriptorTable* GlobalDescriptorTable::getInstance(){
  return object;
}

void GlobalDescriptorTable::GDT_Install(){
  uint32_t i[2];
  i[1] = (uint32_t)GlobalDescriptorTable::getInstance();
  i[0] = sizeof(GlobalDescriptorTable) << 16;
  asm volatile("lgdt (%0)": :"p" (((uint8_t *) i)+2));
  asm volatile("ltr 0x2B");     //tss flush
}
