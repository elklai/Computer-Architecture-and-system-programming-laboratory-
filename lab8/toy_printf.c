/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int print_int(int n, int radix, const char * digit);

/* the states in the printf state-machine */
enum printf_state {
  st_printf_init,
  st_printf_percent,
  st_printf_octal2,
  st_printf_octal3
};

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

int toy_printf(char *fs, ...);
char command [3] ;
int f=1;
const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int get_int_size(int int_value){
    int output=0;
    if(int_value<0)
    {
        output=1;
        int_value=int_value*-1;
    }
    while(int_value>0)
    {
        int_value=int_value/10;
        output++;
    }
    return output;
}

int get_str_size(char *string_value){
    int output=0;
    while(*string_value)
    {
        output++;
        string_value++;
    }
    return output;
}

int power10(int a){
    int output=1;
    for(int i=0;i<a;i++)
        output=output*10;
    return output;
}

void int_print(int int_value,int width, int size){
    if(width>size)
        print_int(int_value,10,digit);
    else
    {
        int_value=int_value/power10(size-width);
        print_int(int_value,10,digit);
    }
}

void str_print(char *string_value,int width,int size){
    for(int i=0;i<width&&i<size;i++){
		putchar(*string_value);
		string_value++;
	}
}

void padd(char c,int num){
   if(c=='s')
   {
       for(int i=0;i<num;i++)
           putchar(' ');
   }
    else
    {
        for(int i=0;i<num;i++)
           putchar('0');
    }
}

void  my_int_print(int width,int int_value,int size){/*fs may use to remove duplicate code*/
    if(int_value<0)
    {
        putchar('-');
        padd('d',width-size);
        int_print(-1*int_value,width,size);
    }
    else
    {
        padd('d',width-size);
         int_print(-1*int_value,width,size);
    }
    
}

void  my_string_print(int f,int width,char *string_value,int size){/*fs may use to remove duplicate code*/
    if(f==0)
    {
        padd('s',width-size);
        str_print(string_value,width,size);
    }
    else
    {
        str_print(string_value,width,size);
        padd('s',width-size);
        putchar('#');
    }
}

void arr_print(char key_char,int* arr,int size){
     command[0]='%';
    command[1]=key_char;
    command[2]='\0';
    putchar('{');
    for(int i=0;i<size-1;i++){
       
        toy_printf(command,arr[i]);
        putchar(',');
    }
    toy_printf(command,arr[size-1]);
    putchar('}');

    
}


void arr_print_c(char key_char,char* arr,int size){
     command[0]='%';
    command[1]=key_char;
    command[2]='\0';
    putchar('{');
    for(int i=0;i<size-1;i++){
       
        toy_printf(command,arr[i]);
        putchar(',');
    }
    toy_printf(command,arr[size-1]);
    putchar('}');

    
}


void arr_print_s(char key_char,char** arr,int size){
     command[0]='%';
    command[1]=key_char;
    command[2]='\0';
    putchar('{');
    for(int i=0;i<size-1;i++){
       
        toy_printf(command,arr[i]);
        putchar(',');
    }
    toy_printf(command,arr[size-1]);
    putchar('}');

    
}
int print_unsigned_int_helper(unsigned int n, int radix, const char *digit) {
  int result;

  if (n < radix) {
    putchar(digit[n]);
    return 1;
  }
  else {
    result = print_unsigned_int_helper(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}


int print_int_helper(int n, int radix, const char *digit) {
  int result;

  if (n < radix) {
    putchar(digit[n]);
    return 1;
  }
  else {
    result = print_int_helper(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}
int print_unsigned_int(unsigned int n, int radix, const char * digit) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

  if (n > 0) {
    return print_unsigned_int_helper(n, radix, digit);
  }
  if (n == 0) {
    putchar('0');
    return 1;
  }
  else {
    putchar('-');
    return 1 + print_unsigned_int_helper(n, radix, digit);
  }
}

int print_int(int n, int radix, const char * digit) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

  if (n > 0) {
    return print_int_helper(n, radix, digit);
  }
  if (n == 0) {
    putchar('0');
    return 1;
  }
  else {
    putchar('-');
    return 1 + print_int_helper(-n, radix, digit);
  }
}

/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
  int chars_printed = 0;
  int int_value = 0;
  int width =0;
  int size=0;
  unsigned int unsigned_int_value=0;
  char *string_value;
  char char_value;
  va_list args;
  enum printf_state state;

  va_start(args, fs);

  state = st_printf_init; 

  for (; *fs != '\0'; ++fs) {
    switch (state) {
    case st_printf_init:
      switch (*fs) {
      case '%':
	state = st_printf_percent;
	break;

      default:
	putchar(*fs);
	++chars_printed;
      }
      break;

    case st_printf_percent:
      switch (*fs) {
      case '%':
	putchar('%');
	++chars_printed;
	state = st_printf_init;
	break;

      case 'd':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 10, digit);
	state = st_printf_init;
	break;

      case 'b':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 2, digit);
	state = st_printf_init;
	break;

      case 'o':
	unsigned_int_value = va_arg(args, int);
	chars_printed += print_unsigned_int(unsigned_int_value, 8, digit);
	state = st_printf_init;
	break;
	
      case 'x':
	unsigned_int_value = va_arg(args, int);
	chars_printed += print_unsigned_int(unsigned_int_value, 16, digit);
	state = st_printf_init;
	break;

      case 'X':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 16, DIGIT);
	state = st_printf_init;
	break;

      case 's':
	string_value = va_arg(args, char *);
	while(*string_value){
		chars_printed++;
		putchar(*string_value);
		string_value++;
	}
	state = st_printf_init;
	break;

      case 'c':
	char_value = (char)va_arg(args, int);
	putchar(char_value);
	++chars_printed;
	state = st_printf_init;
	break;
        
      case 'u':
        unsigned_int_value = va_arg(args, int);
	chars_printed += print_unsigned_int(unsigned_int_value, 10, digit);
	state = st_printf_init;
	break;
        
      case 'A':
          ++fs;
        char key_char=*fs;
        
        if(key_char=='s')
        {////////////////////////why it is not print string?
            char** arr=va_arg(args,char**);
            int size = va_arg(args, int);
            arr_print_s(key_char,arr,size);
        }
        else if(key_char=='c')
        {
            char* arr=va_arg(args,char*);
            int size = va_arg(args, int);
            arr_print_c(key_char,arr,size);
        }
        else   
        {
            int* arr = va_arg(args, int*);
            int size = va_arg(args, int);
            arr_print(key_char,arr,size);
        }

	state = st_printf_init;
	break;
        
     case '0' ... '9'://to check how to write or '-'!
         
         while((*fs<='9')&&(*fs>='0'))
	{
        width=width*10+(*fs-'0');
	fs++;
	}
	width = width;
        if(*fs=='s')//to check how to remove if
        {
            string_value=va_arg(args,char*);
            size=get_str_size(string_value);
            my_string_print(f,width,string_value,size);
        }
        else
        {
            int_value=va_arg(args,int);
            size=get_int_size(int_value);
            my_int_print(width,int_value,size);
        }
        state=st_printf_init;
        break;
        
    case '-'://to check how to write or '-'!
                 
        f=0;
        break;

      default:
	toy_printf("Unhandled format %%%c...\n", *fs);
	exit(-1);
      }
      break;

    default:
      toy_printf("toy_printf: Unknown state -- %d\n", (int)state);
      exit(-1);
    }
  }

  va_end(args);

  return chars_printed;
}


