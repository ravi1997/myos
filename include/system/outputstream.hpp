#ifndef _OUTPUT_STREAM_HPP_
#define _OUTPUT_STREAM_HPP_ 1

#ifndef _TYPES_HPP_
#include<types.hpp>
#endif

template<Writeable W,W w>
class OutputStream{
  void println(string);
  void print(string);
};

#endif
