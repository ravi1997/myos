#ifndef _MAP_HPP_
#define _MAP_HPP_ 1

#ifndef _PAIR_HPP_
#include<util//Pair.hpp>
#endif

#ifndef _VECTOR_HPP_
#include<util//Vector.hpp>
#endif

template<typename K,typename V>
class Map{
public:
  typedef K keyType;
  typedef V valueType;

  typedef typename Vector<Pair<keyType,valueType>>::Iterator Iterator;
  typedef typename Vector<Pair<keyType,valueType>>::const_Iterator const_Iterator;

  Map()=default;
  Map(const Map&m):map(m.map){}
  Map(const Map&&m):map(m.map){}
  Map& operator=(const Map&m){
    map=m.map;
    return *this;
  }
  Map& operator=(const Map&&m){
    map=m.map;
    return *this;
  }
  ~Map(){}
  valueType& operator[](const keyType&k){
    for(auto& [key,value]:map)
      if(key==k)
        return value;
    map.insert_back(Pair{k,valueType{}});
    return *(map.end()--).second;
  }


private:
  Vector<Pair<keyType,valueType>> map;
};


#endif
