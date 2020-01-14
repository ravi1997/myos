#ifndef _MEMORY_MANAGER_HPP_
#define _MEMORY_MANAGER_HPP_ 1

#ifndef _TYPES_HPP_
#include<types.hpp>
#endif

#ifndef _EXCEPTION_MEMORY_ALLOCATION_HPP_
#include<lang//ExceptionMemoryAllocation.hpp>
#endif

#ifndef _EXCEPTION_MEMORY_DEALLOCATION_HPP_
#include<lang//ExceptionMemoryDeallocation.hpp>
#endif

#define FRAGMENTS 20

class MemoryManger{
private:
  struct Memory{
    void* ptr=nullptr;
    size_t size=0;
    Memory()=default;
    Memory& operator=(const Memory& m){
      ptr=m.ptr;
      size=m.size;
      return *this;
    }
  };

  static size_t u,p;
  static Memory pool[FRAGMENTS];
  static Memory used[FRAGMENTS];
public:

  ~MemoryManger()=delete;
  static void* allocate(const size_t x){
    if(u==FRAGMENTS||p==FRAGMENTS)
      return nullptr;
    void* t=nullptr;
    for(size_t i=0;i<p;i++){
      if(pool[i].size>x){
        pool[i].size-=x;
        t=pool[i].ptr;
        pool[i].ptr=((char*)pool[i].ptr)+x;
        break;
      }
      else if(pool[i].size==x){
        t=pool[i].ptr;
        for(size_t j=i;j<p-1;j++)
          pool[j]=pool[j+1];
        p--;
        break;
      }
    }
    if(t==nullptr){
      defragment();
      for(size_t i=0;i<p;i++){
        if(pool[i].size>x){
          pool[i].size-=x;
          t=pool[i].ptr;
          pool[i].ptr=((char*)pool[i].ptr)+x;
          break;
        }
        else if(pool[i].size==x){
          t=pool[i].ptr;
          for(size_t j=i;j<p-1;j++)
            pool[j]=pool[j+1];
          p--;
          break;
        }
      }
      if(t==nullptr)
          return nullptr;
    }
    used[u++]={t,x};
    return t;
  }
  static void deallocate(const void* x){
    for(size_t i=0;i<u;i++)
      if(used[i].ptr==x){
        Memory y=used[i];
        for(size_t j=i;j<u-1;j++)
          used[j]=used[j+1];
        pool[p++]=y;
        return;
      }
    throw ExceptionMemoryDeallocation();
  }
  static void defragment(){
    defragment_again:
    for(size_t i=0;i<p;i++){
      for(size_t j=0;j<p;j++){
        if((char*)pool[i].ptr+pool[i].size==pool[j].ptr){
          pool[i].size+=pool[j].size;
          for(size_t k=j;k<p-1;k++)
            pool[k]=pool[k+1];
          goto defragment_again;
        }
      }
    }
  }
};

size_t MemoryManger::p=1;
MemoryManger::Memory MemoryManger::pool[FRAGMENTS];
size_t MemoryManger::u=1;
MemoryManger::Memory MemoryManger::used[FRAGMENTS];

void* operator new (const size_t x){
  void *mem=MemoryManger::allocate(x);
  if(mem==nullptr)
    throw ExceptionMemoryAllocation();
  return mem;
}
void* operator new[](const size_t x){
  void *mem=MemoryManger::allocate(x);
  if(mem==nullptr)
    throw ExceptionMemoryAllocation();
  return mem;
}
void operator delete(void*x){
  if(x!=nullptr)
  MemoryManger::deallocate(x);
}
void operator delete[](void*x){
  if(x!=nullptr)
  MemoryManger::deallocate(x);
}



#endif
