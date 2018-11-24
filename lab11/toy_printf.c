/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int print_int_helper_d(int n, int radix, const char *digit) {
  int result;

  if (n < radix) {
    putchar(digit[n]);
    return 1;
  }
  else {
    result = print_int_helper_d(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}

int print_int_d(int n, int radix, const char * digit) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

  if (n > 0) {
    return print_int_helper_d(n, radix, digit);
  }
  if (n == 0) {
    putchar('0');
    return 1;
  }
  else {
    putchar('-');
    return 1 + print_int_helper_d(-n, radix, digit);
  }
}

int print_int_helper(unsigned int n, int radix, const char *digit) {
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

int print_int(unsigned int n, int radix, const char * digit) {
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
    return print_int_helper(n, radix, digit);
  }
}

/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */
void my_print(char c,int* arr,int size){
   putchar('{');
   int redix=10;
   if(c=='x')
       redix=8;
   for(int i=0;i<size-1;i++)
   {
       print_int_d(arr[i], redix, digit);
       putchar(',');
   }
   print_int_d(arr[size-1], redix, digit);
   putchar('}');
}
void my_print_s(char c,char** arr,int size){
    putchar('{');
    for(int i=0;i<size-1;i++)
    {
       while(*arr[i]){
		/*chars_printed++;*/
		putchar(*arr[i]);
		arr[i]++;
	}
       putchar(',');
    }
    while(*arr[size-1]){
        /*chars_printed++;*/
        putchar(*arr[size-1]);
        arr[size-1]++;
        
    }
    putchar('}');
}
int toy_printf(char *fs, ...) {
  int chars_printed = 0;
  int int_value = 0;
  char *string_value;
  char char_value;
  char octal_char;
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
	chars_printed += print_int_d(int_value, 10, digit);
	state = st_printf_init;
	break;

      case 'b':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 2, digit);
	state = st_printf_init;
	break;

      case 'u':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 10, digit);
	state = st_printf_init;
	break;
        
      case 'o':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 8, digit);
	state = st_printf_init;
	break;
	
      case 'x':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 16, digit);
	state = st_printf_init;
	break;

      case 'X':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 16, DIGIT);
	state = st_printf_init;
	break;
/**/
      case 'A':
          fs++;
          putchar(*fs);
        switch(*fs)
            
        {
            case 'd':
                my_print('d',va_arg(args,int*),va_arg(args,int));
                break;
            case 's':
                my_print_s('s',va_arg(args,char**),va_arg(args,int));
                break;
            case 'x':
                my_print('x',va_arg(args,int*),va_arg(args,int));
                break;
            default:
	        toy_printf("Unhandled format %%%c...\n", *fs);
	        exit(-1);
        }
	break;/**/
        
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


