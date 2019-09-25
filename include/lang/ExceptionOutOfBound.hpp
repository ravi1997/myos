#ifndef _EXCEPTION_OUT_OF_BOUND_HPP_
#define _EXCEPTION_OUT_OF_BOUND_HPP_ 1

#ifndef _EXCEPTION_HPP_
#include<lang//Exception.hpp>
#endif

class ExceptionOutOfBound:Exception{
public:
  ExceptionOutOfBound():Exception("ExceptionOutOfBound",3){}
  virtual string toString()override final{
    return getExceptionName();
  }
};


#endif
