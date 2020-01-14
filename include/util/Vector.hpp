
#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

#ifndef _EXCEPTION_OUT_OF_BOUND_HPP_
#include<lang//ExceptionOutOfBound.hpp>
#endif

#ifndef _MEMORY_MANAGER_HPP_
#include<system//MemoryManager.hpp>
#endif

#ifndef _CONCEPTS_HPP_
#include<lang//concepts.hpp>
#endif


template<typename t>
class Vector{
	public:
		typedef t valuetype;
		typedef t* pointer;
		typedef const t* const_pointer;

		typedef t& reference;
		typedef const t& const_reference;

		class Iterator{
			public:
				Iterator(Vector& T,bool):data(T.data+T.size){}
				Iterator(Vector& T):data(T.data)
				{}

				Iterator(const Iterator& I):data(I.data){}
				Iterator(const Iterator&& I):data(I.data){}

				Iterator& operator=(const Iterator& I){
					data=I.data;
					return *this;
				}

				Iterator& operator=(const Iterator&& I){
					data=I.data;
					return *this;
				}

				~Iterator(){data=nullptr;}

				bool operator==(const Iterator& I){
					return data==I.data;
				}

				bool operator!=(const Iterator& I){
					return data!=I.data;
				}

				Iterator& operator++(){
					data++;
					return *this;
				}

				Iterator operator++(int){
					Iterator I=*this;
					data++;
					return I;
				}

				Iterator operator--(int){
					Iterator I=*this;
					data--;
					return I;
				}

				Iterator& operator--(){
					data--;
					return *this;
				}

                reference operator*(){
                    return *data;
                }
			private:
				pointer data;
		};

		class const_Iterator{
			public:
				const_Iterator(Vector& T,bool):data(T.data+T.size){}
				const_Iterator(Vector& T):data(T.data)
				{}

				const_Iterator(const const_Iterator& I):data(I.data){}
				const_Iterator(const const_Iterator&& I):data(I.data){}

				const_Iterator& operator=(const const_Iterator& I){
					data=I.data;
					return *this;
				}

				const_Iterator& operator=(const const_Iterator&& I){
					data=I.data;
					return *this;
				}

				~const_Iterator(){data=nullptr;}

				bool operator==(const const_Iterator& I){
					return data==I.data;
				}

				bool operator!=(const const_Iterator& I){
					return data!=I.data;
				}

				const_Iterator& operator++(){
					data++;
					return *this;
				}

				const_Iterator operator++(int){
					Iterator I=*this;
					data++;
					return I;
				}

				const_Iterator operator--(int){
					Iterator I=*this;
					data--;
					return I;
				}

				const_Iterator& operator--(){
					data--;
					return *this;
				}

                const_reference operator*(){
                    return *data;
                }
			private:
				pointer data;
		};



		Iterator begin(){
			return Iterator(*this);
		}

		Iterator end(){
			return Iterator(*this,true);
		}

		const_Iterator begin() const{
			return const_Iterator(*this);
		}

		const_Iterator end() const{
			return const_Iterator(*this,true);
		}


		reference operator[](const size_t i){
				if(i<capacity){
					size=(size<(i+1))?(i+1):size;
					return data[i];
				}
				throw ExceptionOutOfBound();
		}
		const_reference operator[](const size_t i)const{
				if(i<capacity){
					size=(size<(i+1))?(i+1):size;
					return data[i];
				}
				throw ExceptionOutOfBound();
		}


			template<Iterable itr>
			Vector(itr start,itr finish){
				size=capacity=0;
				data=nullptr;
				for(auto x=start;x!=finish;x++)
					insert_back(*x);
			}

        Vector(const size_t c){
            capacity=c;
            data=new valuetype[c];
            size=0;
        }

        Vector(){
            capacity=3;
            data=new valuetype[3];
            size=0;
        }

        Vector(const Vector& v){
            capacity=v.capacity;
            data=new valuetype[v.capacity];
            size=v.size;

            auto i=0;
            for(auto a:v)
                data[i++]=*a;
        }

        Vector(const Vector&& v):data(v.data),
                                 capacity(v.capacity),
                                 size(v.size)
                                 {}

        Vector& operator=(const Vector& v){
            if(capacity!=v.capacity){
                delete[] data;
                capacity=v.capacity;
                data=new valuetype[v.capacity];
            }
            size=v.size;

            auto i=0;
            for(auto a:v)
                data[i++]=*a;
            return *this;
        }

        Vector& operator=(const Vector&& v){
            if(data!=v.data && data!=nullptr){
                delete[] data;
            }
            capacity=v.capacity;
            data=v.data;
            size=v.size;
            return *this;
        }

        ~Vector(){
            if(data!=nullptr){
                size=capacity=0;
                delete[] data;
            }
        }

				bool operator==(const Vector& c){
					auto x=begin(),y=c.begin();
					while(x!=end()&&y!=c.end()&&*x==*y){
						x++;
						y++;
					}
					return x==end()&&y==c.end()&&*x!=*y;
				}

				bool operator!=(const Vector& y){
					return !(*this==y);
				}

				void resize(const size_t x){
					if(x==capacity)
						return;

					Vector v(x);
					for(size_t i=0;i<capacity&&i<x;i++)
						v[i]=data[i];

					delete data;
					capacity=v.capacity;
					size=v.size;
					data=v.data;

					v.data=nullptr;
				}


				void insert_back(valuetype v){
					if(size<capacity)
						*this[size]=v;
					else {
						Vector temp(capacity+1);
						size_t i=0;
						for(auto x:*this)
							temp[i++]=*x;
						temp[i]=v;
						data=temp.data;
						capacity=temp.capacity;
						size=temp.size;
						temp.data=nullptr;
						temp.capacity=temp.size=0;
					}
				}

				void insert_begin(valuetype v){
					if(size<capacity){
						for(size_t i=size;i>0;i--)
							data[i]=data[i-1];
						data[0]=v;
					}
					else {
						Vector temp(capacity+1);
						size_t i=0;
						temp[i++]=v;
						for(auto x:*this)
							temp[i++]=*x;
						data=temp.data;
						capacity=temp.capacity;
						size=temp.size;
						temp.data=nullptr;
						temp.capacity=temp.size=0;
					}
				}
	private:
		pointer data;
		size_t capacity,size;
};

#endif
