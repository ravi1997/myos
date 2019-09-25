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
concept bool Writeable=requires {
	{ T()  } -> T;
};


#endif
