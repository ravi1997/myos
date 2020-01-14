#ifndef _CONCEPTS_HPP_
#define _CONCEPTS_HPP_ 1

template<typename T>
concept bool Writeable=requires(T a) {
	{ a.write()  } -> void;
};

template<typename T>
concept bool Iterable=requires(T a,T b) {
	{ a!=b } -> bool;
	{ a==b } -> bool;
	{ ++a  } -> T;
	{ a++  } -> T;
	{ a--  } -> T;
	{ --a  } -> T;
};

template<typename T>
concept bool DefaultConstructable=requires {
	{ T()  } -> T;
};


  template<typename t>
  struct isIntegeral{
    static constexpr bool value=false;
  };


  template<>
  struct isIntegeral<uint8_t>{
    static constexpr bool value=true;
  };
  template<>
  struct isIntegeral<uint16_t>{
    static constexpr bool value=true;
  };
  template<>
  struct isIntegeral<uint32_t>{
    static constexpr bool value=true;
  };
  template<>
  struct isIntegeral<int8_t>{
    static constexpr bool value=true;
  };
  template<>
  struct isIntegeral<int16_t>{
    static constexpr bool value=true;
  };

  template<>
  struct isIntegeral<int32_t>{
    static constexpr bool value=true;
  };

  template<typename t>
  concept bool Integeral=isIntegeral<t>::value;
#endif
