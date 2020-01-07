#ifndef _OUTPUT_STREAM_HPP_
#define _OUTPUT_STREAM_HPP_ 1

#ifndef _TYPES_HPP_
#include<lang//types.hpp>
#endif


  template<typename t>
  struct isIntegeral{
    static constexpr bool value=false;
  };


  template<>
  struct isIntegeral<uint_8>{
    static constexpr bool value=true;
  };
  template<>
  struct isIntegeral<uint_16>{
    static constexpr bool value=true;
  };
  template<>
  struct isIntegeral<uint_32>{
    static constexpr bool value=true;
  };
  template<>
  struct isIntegeral<int_8>{
    static constexpr bool value=true;
  };
  template<>
  struct isIntegeral<int_16>{
    static constexpr bool value=true;
  };

  template<>
  struct isIntegeral<int_32>{
    static constexpr bool value=true;
  };

  template<typename t>
  concept bool Integeral=isIntegeral<t>::value;

class OutputStream{
  uint_8 x=0,y=0;
  bool printHex=false;
  unsigned short* VideoMemory = (uint_16 *)0xb8000;
public:
  enum class type{
    DEC,
    HEX,
    BINARY
  };

  template<Integeral t>
  OutputStream& operator<<(t u){
    if(printHex)
      printfHex(u);
    else
      printf(u);
    return *this;
  }

  OutputStream& operator<<(char const* str){
    printf(str);
    return *this;
  }

  OutputStream& operator<<(type t){
    if(t==type::HEX)
      printHex=true;
    else
      printHex=false;
    return *this;
  }

  template<Integeral t>
  void printf(t u){
    char y[8];
    auto f=[](t num, char* str){
      int i = 0;
      int base=10;
      bool isNegative = false;

      /* Handle 0 explicitely, otherwise empty string is printed for 0 */
      if (num == 0)
      {
          str[i++] = '0';
          str[i] = '\0';
          return str;
      }

      // In standard itoa(), negative numbers are handled only with
      // base 10. Otherwise numbers are considered unsigned.
      if (num < 0 && base == 10)
      {
          isNegative = true;
          num = -num;
      }

      // Process individual digits
      while (num != 0)
      {
          int rem = num % base;
          str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
          num = num/base;
      }

      // If number is negative, append '-'
      if (isNegative)
          str[i++] = '-';

      str[i] = '\0'; // Append string terminator

      auto reverse=[](char str[], int length){
          int start = 0;
          int end = length -1;
          while (start < end)
          {
              auto swap=[](char& a,char& b){
                auto x=a;
                a=b;
                b=x;
              };
              swap(*(str+start), *(str+end));
              start++;
              end--;
          }
      };

      // Reverse the string
      reverse(str, i);

      return str;

    };
    f(u,y);
    printf(y);
  }
  void printf(char const* str){
  for(int i=0;str[i]!='\0';i++){
    switch(str[i]){
      case '\n':
        y++;
        x=0;
        break;
/*      case '%':
                switch(str[i+1]){
                  case 'c':
                  case ''
                }
*/      default:
        VideoMemory[80*y+x]=(VideoMemory[80*y+x]&0xFF00)|str[i];
        x++;
        break;
    }
    if(x==80){
      x=0;
      y++;
    }
    if(y==25){
      for(int i=0;i<80;i++)
      for(int j=0;j<25;j++)
      VideoMemory[80*j+i]=(VideoMemory[80*j+i]&0xFF00)|' ';
      x=0,y=0;
    }
  }
}

  template<Integeral t>
  void printfHex(t key)
  {
      char foo[16];
      auto f=[](t num, char* str){
        int i = 0;
        int base=16;
        bool isNegative = false;

        /* Handle 0 explicitely, otherwise empty string is printed for 0 */
        if (num == 0)
        {
            str[i++] = '0';
            str[i] = '\0';
            return str;
        }

        // In standard itoa(), negative numbers are handled only with
        // base 10. Otherwise numbers are considered unsigned.
        if (num < 0 && base == 10)
        {
            isNegative = true;
            num = -num;
        }

        // Process individual digits
        while (num != 0)
        {
            int rem = num % base;
            str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
            num = num/base;
        }

        // If number is negative, append '-'
        if (isNegative)
            str[i++] = '-';

        str[i] = '\0'; // Append string terminator

        auto reverse=[](char str[], int length){
            int start = 0;
            int end = length -1;
            while (start < end)
            {
                auto swap=[](char& a,char& b){
                  auto x=a;
                  a=b;
                  b=x;
                };
                swap(*(str+start), *(str+end));
                start++;
                end--;
            }
        };

        // Reverse the string
        reverse(str, i);

        return str;

      };
      f(key,foo);
      printf(foo);
  }

};

extern OutputStream cout;
extern const char* endl;

#endif
