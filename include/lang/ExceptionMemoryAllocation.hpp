#ifndef _EXCEPTION_MEMORY_ALLOCATION_HPP_
#define _EXCEPTION_MEMORY_ALLOCATION_HPP_ 1

#ifndef _EXCEPTION_HPP_
#include<lang//Exception.hpp>
#endif

class ExceptionMemoryAllocation:Exception{
public:
  ExceptionMemoryAllocation():Exception("ExceptionMemoryAllocation",1){}
  virtual string toString()override final{
    return getExceptionName();
  }
};


#endif
