
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
    st_printf_percent
    //
   // st_printf_octal2,
    //st_printf_octal3
};

typedef struct {
    char* fs;
    /* Any extra required args */
}state_args;

typedef struct state_result {
    int printed_chars;
    int neg;
    int width;
    enum printf_state new_state;
}state_result;

int print_int(unsigned int n, int radix, const char * digit);
int print_int_d(int n, int radix, const char * digit);
void my_print(char c,int* arr,int size);
void printSpaces(int width);
void my_print_s(char c,char** arr,int size);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";
    int int_value=0;
    int* arr;
    char** sArr;
    int arrSize;
    int neg=1;
    char *string_value;
    char char_value;
    
int toy_printf(char *fs, ...);
void default_han_mash();
state_result* percent_state_handler(va_list args, state_args* onlyFs,state_result* result);
state_result* init_state_handler(va_list args, state_args* onlyFs,state_result* result);
void default_handler(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_num(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_per(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_d(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_b(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_u(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_o(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_x(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_X(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_A(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_sub(va_list args, state_args* onlyFs,state_result* result);
void percent_handler_s(va_list args, state_args* onlyFs,state_result* result);


void default_handler(va_list args, state_args* onlyFs,state_result* result)
{
    toy_printf("Unhandled format %%%c...\n", *(onlyFs->fs));
    exit(-1);
}

void percent_handler_num(va_list args, state_args* onlyFs,state_result* result)
{
            while( (*(onlyFs->fs)=='-') ||( (*(onlyFs->fs)<='9')&&(*(onlyFs->fs)>='0')))
            {
                if(*(onlyFs->fs)=='-')
                    neg=-1;
                else
                    result->width=result->width*10+(*(onlyFs->fs)-'0');
                (onlyFs->fs)++;
            }
            result->width =result-> width*neg;
            if(*(onlyFs->fs)=='d')
            {
                int value=va_arg(args,int);
                if(value<0)
                {
                    putchar('-');
                    value=value*(-1);
                }
                for(int i=0;i<result->width;i++)
                    putchar('0');
                print_int_d(value, 10, digit);
            }

            else
            {

                char *value=va_arg(args,char*);
                if(result->width<0)
                {
                    result->width=result->width*(-1);
                    printSpaces(result->width);
                    while(*value)
                    {
                        result->printed_chars++;
                        putchar(*value);
                        value++;
                    }
                }
                else
                {
                    while(*value)
                    {

                        result->printed_chars++;
                        putchar(*value);
                        value++;
                    }
                    printSpaces(result->width);
                    putchar('#');
                }

            }
            result->width=0;
            neg=1;
            result->new_state =st_printf_init;
}
void percent_handler_per(va_list args, state_args* onlyFs,state_result* result){
            putchar('%');
            result->printed_chars++;
            result->new_state = st_printf_init;
}
void percent_handler_d(va_list args, state_args* onlyFs,state_result* result){
            int_value = va_arg(args, int);
            result->printed_chars += print_int_d(int_value, 10, digit);
            result->new_state  = st_printf_init;
}
void percent_handler_b(va_list args, state_args* onlyFs,state_result* result){
            int_value = va_arg(args, int);
            result->printed_chars += print_int(int_value, 2, digit);
            result->new_state  = st_printf_init;
}
void percent_handler_u(va_list args, state_args* onlyFs,state_result* result){
            int_value = va_arg(args, int);
            result->printed_chars += print_int(int_value, 10, digit);
            result->new_state  = st_printf_init;
}
void percent_handler_o(va_list args, state_args* onlyFs,state_result* result){
            int_value = va_arg(args, int);
            result->printed_chars += print_int(int_value, 8, digit);
            result->new_state  = st_printf_init;
}
void percent_handler_x(va_list args, state_args* onlyFs,state_result* result){
            int_value = va_arg(args, int);
            result->printed_chars += print_int(int_value, 16, digit);
            result->new_state  = st_printf_init;
}
void percent_handler_X(va_list args, state_args* onlyFs,state_result* result){
            int_value = va_arg(args, int);
            result->printed_chars += print_int(int_value, 16, DIGIT);
            result->new_state  = st_printf_init;
}
void percent_handler_A(va_list args, state_args* onlyFs,state_result* result){
            (onlyFs->fs)++;

            switch(*(onlyFs->fs))

            {
                case 'd':
                    arr=va_arg(args,int*);
                    arrSize=va_arg(args,int);
                    my_print('d',arr,arrSize);
                    result->new_state =st_printf_init;
                    break;
                case 's':
                    sArr=va_arg(args,char**);
                    arrSize=va_arg(args,int);
                    my_print_s('s',sArr,arrSize);
                    result->new_state =st_printf_init;
                    break;
                case 'x':
                    arr=va_arg(args,int*);
                    arrSize=va_arg(args,int);
                    my_print('x',arr,arrSize);
                    result->new_state =st_printf_init;
                    break;
                default:
                    toy_printf("Unhandled format %%%c...\n", *(onlyFs->fs));
                    exit(-1);
            }
}
void percent_handler_sub(va_list args, state_args* onlyFs,state_result* result){
               result->width=0;
            neg=1;
            while( (*(onlyFs->fs)=='-') ||( (*(onlyFs->fs)<='9')&&(*(onlyFs->fs)>='0')))
            {
                if(*(onlyFs->fs)=='-')
                    neg=-1;
                else
                    result->width=result->width*10+(*(onlyFs->fs)-'0');
                (onlyFs->fs)++;
            }

            result->width = result->width*neg;

            if(*(onlyFs->fs)=='d')
            {
                int value=va_arg(args,int);
                if(value<0)
                {
                    putchar('-');
                    value=value*(-1);
                }
                for(int i=0;i<result->width;i++)
                    putchar('0');
                print_int_d(value, 10, digit);
            }

            else
            {
                char *value=va_arg(args,char*);
                if(result->width<0)
                {
                    result->width=result->width*(-1);
                    printSpaces(result->width);
                    while(*value)
                    {
                        result->printed_chars++;
                        putchar(*value);
                        value++;
                    }
                }
                else
                {
                    while(*value)
                    {
                        result->printed_chars++;
                        putchar(*value);
                        value++;
                    }
                    printSpaces(result->width);
                }

            }
            result->width=0;
            neg=1;
            result->new_state =st_printf_init;
}
void percent_handler_s(va_list args, state_args* onlyFs,state_result* result){
               string_value = va_arg(args, char *);
            while(*string_value){
                result->printed_chars++;
                putchar(*string_value);
                string_value++;
            }
            result->new_state  = st_printf_init;
}
void percent_handler_c(va_list args, state_args* onlyFs,state_result* result){
            char_value = (char)va_arg(args, int);
            putchar(char_value);
            ++result->printed_chars;
            result->new_state  = st_printf_init;
    
}
struct state_result* (*handlers[2])(va_list args, state_args* onlyFs,state_result* result)={init_state_handler, percent_state_handler};
void (*handlers_per[128])(va_list args, state_args* onlyFs,state_result* result);
void default_han_mash(){
    for (int i=0; i<128; i++)
    {
        handlers_per[i]=default_handler;
    }
    handlers_per['0']=percent_handler_num;
    handlers_per['1']=percent_handler_num;
    handlers_per['2']=percent_handler_num;
    handlers_per['3']=percent_handler_num;
    handlers_per['4']=percent_handler_num;
    handlers_per['5']=percent_handler_num;
    handlers_per['6']=percent_handler_num;
    handlers_per['7']=percent_handler_num;
    handlers_per['8']=percent_handler_num;
    handlers_per['9']=percent_handler_num;
    handlers_per['%']=percent_handler_per;
    handlers_per['-']=percent_handler_sub;
    handlers_per['d']=percent_handler_d;
    handlers_per['b']=percent_handler_b;
    handlers_per['u']=percent_handler_u;
    handlers_per['o']=percent_handler_o;
    handlers_per['x']=percent_handler_x;
    handlers_per['X']=percent_handler_X;
    handlers_per['A']=percent_handler_A;
    handlers_per['s']=percent_handler_s;

}


#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')



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
state_result* percent_state_handler(va_list args, state_args* onlyFs,state_result* result){
    handlers_per[*(onlyFs->fs)](args,onlyFs,result);
    return result ;
}

state_result* init_state_handler(va_list args, state_args* onlyFs,state_result* result)
{
    result->new_state=st_printf_init;
    result->printed_chars=0;
    result->width=0;
    result->neg=1;

    switch (*(onlyFs->fs)) {

        case '%':
            result->new_state = st_printf_percent;
            break;

        default:
            putchar(*(onlyFs->fs));
            ++result->printed_chars;
    }
    return result;
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

int getWidth(char *fs){
    int width=0;
    int neg=1;
    while( (*fs=='-') ||( (*fs<='9')&&(*fs>='0')))
    {
        if(*fs=='-')
            neg=-1;
        else
            width=width*10+(*fs-'0');
        fs++;
    }

    return width*neg;
}

void printSpaces(int width){
    for(int i=0;i<width;i++)
        putchar(' ');
}
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
    if(size>0)
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
    default_han_mash();


    va_list args;

    state_args onlyFs;


    va_start(args, fs);
    onlyFs.fs=fs;
    state_result* result;
    result->new_state= st_printf_init;

    for (; *(onlyFs.fs) != '\0'; ++(onlyFs.fs))
        result =handlers[result->new_state](args,&onlyFs,result);

    va_end(args);

    return result->printed_chars;
}


