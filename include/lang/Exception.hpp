#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_ 1

#ifndef _TYPES_HPP_
#include<lang//types.hpp>
#endif


class Exception{
private:
  string ExceptionName;
  uint_8 ExceptionNumber;
public:
  Exception(string EName,uint_8 ENumber):
      ExceptionName(EName),
      ExceptionNumber(ENumber)
      {}
  Exception(const Exception& e):
    ExceptionName(e.ExceptionName),
    ExceptionNumber(e.ExceptionNumber)
    {}

  Exception(const Exception&& e):
    ExceptionName(e.ExceptionName),
    ExceptionNumber(e.ExceptionNumber)
    {}

  Exception& operator=(const Exception& e){
    ExceptionName=e.ExceptionName;
    ExceptionNumber=e.ExceptionNumber;
    return *this;
  }

  Exception& operator=(const Exception&& e){
    ExceptionName=e.ExceptionName;
    ExceptionNumber=e.ExceptionNumber;
    return *this;
  }

  ~Exception(){}

  string getExceptionName()const{
    return ExceptionName;
  }

  virtual string toString()=0;
};

#endif
