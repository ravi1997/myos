#ifndef _PAIR_HPP_
#define _PAIR_HPP_ 1

template<typename T1,typename T2>
class Pair{
public:
  typedef T1 firstType;
  typedef T2 secondType;

  Pair()=default;
  Pair(firstType f,secondType s=secondType{}){}
  Pair(const Pair& x):first(x.first),second(x.second){}
  Pair(const Pair&& x):first(x.first),second(x.second){}
  Pair& operator=(const Pair& x){
    first=x.first;
    second=x.second;
    return *this;
  }
  Pair& operator=(const Pair&& x){
    first=x.first;
    second=x.second;
    return *this;
  }
  ~Pair(){}

  bool operator==(const Pair& t){
    return first==t.first&&second==t.second;
  }

  bool operator!=(const Pair& t){
    return !(*this==t);
  }

  firstType getFirst(){
    return first;
  }

  secondType getSecond(){
    return second;
  }
  
  firstType first;
  secondType second;
};

#endif
