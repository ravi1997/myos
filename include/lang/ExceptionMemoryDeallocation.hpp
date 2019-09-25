#ifndef _EXCEPTION_MEMORY_DEALLOCATION_HPP_
#define _EXCEPTION_MEMORY_DEALLOCATION_HPP_ 1

#ifndef _EXCEPTION_HPP_
#include<lang//Exception.hpp>
#endif

class ExceptionMemoryDeallocation:Exception{
public:
  ExceptionMemoryDeallocation():Exception("ExceptionMemoryDeallocation",2){}
  virtual string toString()override final{
    return getExceptionName();
  }
};


#endif
