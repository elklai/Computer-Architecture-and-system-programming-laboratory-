#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
// C program for array implementation of stack
extern int _asm_add(int a,int b);
extern int _asm_sub(int a,int b);
extern int _asm_mult(int a,int b);

#define INITIAL_CAPACITY   (16)
#define toDigit(ch)          ((ch) - '0')
#define toChar(in)        (char)(in+48)
typedef struct _bignum
{
    unsigned number_of_digits;
    unsigned capacity;
    int        sign;
    char     *digits;
} Bignum_t;
typedef struct _stack
{
    int       top;
    unsigned   capacity;
    Bignum_t**     array;
} Stack_t;
// A structure to represent a stack

Bignum_t* ultimateSub(Bignum_t* a,Bignum_t* b);
Bignum_t* ultimateAdd(Bignum_t* a,Bignum_t* b);
Bignum_t* ultimateMult(Bignum_t* a,Bignum_t* b);
Bignum_t* ultimateDiv(Bignum_t* a,Bignum_t* b);
Bignum_t*  createBignum(unsigned capacity);
Bignum_t*        fixSign(Bignum_t* result);
void        swap(char* digits ,int a,int b);
void      freeBignum(Bignum_t* bignum);
void  backwards(char* digits);
Bignum_t*     clearBignum(Bignum_t* bignum);
Bignum_t*  cloneBignum (Bignum_t* b);
Bignum_t*  addToBignum (Bignum_t* b,char item);
Stack_t*   createStack(unsigned capacity);
int       isFull(Stack_t* stack);
int       isEmpty(Stack_t* stack);
void      push(Stack_t* stack,Bignum_t* item);
Bignum_t*  pop(Stack_t* stack);
Bignum_t*  peek(Stack_t* stack);
int    call_asm_operation(char action,int a,int b);
Bignum_t* incBignum(Bignum_t* toInc);
void updateBignum(Bignum_t* toUpdate);
int compare(Bignum_t* a,Bignum_t* b);
Bignum_t* divHelper(Bignum_t* result,Bignum_t* ten,Bignum_t* a,Bignum_t* b);
Bignum_t* mult10(Bignum_t* toMult);

Bignum_t* ultimateSub(Bignum_t* op_a,Bignum_t* op_b)
{
    int ans=0;
    int carry=0;
    Bignum_t* result=createBignum(INITIAL_CAPACITY);
    if(op_a->sign*op_b->sign==-1)
    {
        if (op_b->sign == -1) {
            op_b->sign = 1;
            result = ultimateAdd(op_a, op_b);
        }
        else
        {
            op_a->sign=1;
            result=ultimateAdd(op_a,op_b);
            result->sign=-1;
        }
    }
    else if (op_a->sign==-1)
    {
        op_a->sign=1;
        op_b->sign=1;
        result=ultimateSub(op_b,op_a);

    }
    else
    {
        int maxLength = (op_a->number_of_digits > op_b->number_of_digits) ? op_a->number_of_digits
                                                                          : op_b->number_of_digits;
        for (int i = 0; i < maxLength; i++) {
            if (i < op_a->number_of_digits && i < op_b->number_of_digits)
                ans = call_asm_operation('-', toDigit(op_a->digits[i]) - carry, toDigit(op_b->digits[i]));

            else if (i < op_a->number_of_digits)
                ans = call_asm_operation('-', toDigit(op_a->digits[i]) - carry, 0);
            else
                ans = call_asm_operation('-', -1 * carry, toDigit(op_b->digits[i]));
            if (ans < 0) {
                ans = ans + 10;
                carry = 1;
            } else
                carry = 0;
            result = addToBignum(result, toChar(ans));
        }
        if (carry == 1) {
            result = fixSign(result);

        }
        backwards(result->digits);
    }
    return result;

}

Bignum_t* ultimateAdd(Bignum_t* a,Bignum_t* b)
{
    if(a->digits=="")
        addToBignum(a,'0');
    if(b->digits=="")
        addToBignum(b,'0');
    int ans=0;
    int carry=0;
    Bignum_t* result=createBignum(INITIAL_CAPACITY);
    if(a->sign*b->sign==-1)
    {
        if(a->sign==-1)
        {
            a->sign=1;
            result=ultimateAdd(b,a);
        }
        else
        {
            b->sign=1;
            result=ultimateSub(a,b);
        }
    }
    else {
        int maxLength = (a->number_of_digits > b->number_of_digits) ? a->number_of_digits : b->number_of_digits;
        for (int i = 0; i < maxLength; i++) {
            if (i < a->number_of_digits && i < b->number_of_digits)
                ans = call_asm_operation('+', toDigit(a->digits[i]), toDigit(b->digits[i]) + carry);
            else if (i < a->number_of_digits)
                ans = call_asm_operation('+', toDigit(a->digits[i]), carry);
            else
                ans = call_asm_operation('+', toDigit(b->digits[i]), carry);
            carry = ans / 10;
            ans = ans % 10;
            result = addToBignum(result, toChar(ans));
        }
        if (carry == 1)
            result = addToBignum(result, '1');
        backwards(result->digits);
        result->sign = a->sign;
    }
    return result;
}

Bignum_t* ultimateMult(Bignum_t* a,Bignum_t* b)
{
    if(a->digits=="")
        addToBignum(a,'0');
    if(b->digits=="")
        addToBignum(b,'0');
    int ans=0;
    int carry=0;
    Bignum_t* firstResult=createBignum(INITIAL_CAPACITY);
    Bignum_t* secondResult=createBignum(INITIAL_CAPACITY);
    for(int i=0;i<a->number_of_digits;i++)
    {
        for(int j=0;j<i;j++)
            firstResult=addToBignum(firstResult,'0');
        for(int x=0;x<b->number_of_digits;x++)
        {
            ans=call_asm_operation('*',toDigit(b->digits[x]),toDigit(a->digits[i]));
            int mem=ans+carry;
            carry=mem/10;
            ans=mem%10;
            firstResult=addToBignum(firstResult,toChar(ans));
        }
        if(carry!=0)
        firstResult=addToBignum(firstResult,toChar(carry));
        carry=0;
        secondResult=ultimateAdd(firstResult,secondResult);
        backwards(secondResult->digits);
        freeBignum(firstResult);
        firstResult=createBignum(INITIAL_CAPACITY);
    }
    freeBignum(firstResult);
    secondResult->sign=a->sign*b->sign;
    return secondResult;
}

Bignum_t* ultimateDiv(Bignum_t* a,Bignum_t* b) {
    int sign=a->sign*b->sign;
    a->sign=1;
    b->sign=1;
    Bignum_t *result = createBignum(INITIAL_CAPACITY);
    Bignum_t *ten = createBignum(INITIAL_CAPACITY);
    addToBignum(ten,'0');
    addToBignum(ten,'1');
    result= divHelper(result,ten,a,b);
    result->sign=sign;
    backwards(result->digits);
    return result;


}
Bignum_t* divHelper(Bignum_t* result,Bignum_t* ten,Bignum_t* a,Bignum_t* b)
{
    Bignum_t* helpMult10=cloneBignum(b);
    Bignum_t* helpSub=cloneBignum(b);

    int count=0;
    while(compare(a,helpSub)>0) {
        helpMult10=cloneBignum(helpSub);
        backwards(helpMult10->digits);
        helpSub=mult10(helpSub);
    }
    while(compare(a,helpMult10)>=0)
    {
        a=ultimateSub(a,helpMult10);
        backwards(a->digits);
        count++;
    }
    if(count==0&&(compare(helpMult10,b)==0))
        return result;
    addToBignum(result,toChar(count));
    freeBignum(helpMult10);
    freeBignum(helpSub);
    return divHelper(result,ten,a,b);


}
Bignum_t* mult10(Bignum_t* toMult)
{
    Bignum_t* output=createBignum(toMult->capacity);
    addToBignum(output,'0');
    for(int i=0;i<toMult->number_of_digits;i++)
        addToBignum(output,toMult->digits[i]);
    freeBignum(toMult);
    return output;

}

int compare(Bignum_t* a,Bignum_t* b){
    if(a->number_of_digits==0&&b->number_of_digits==0)
        return 0;
    int x=0;
    if(a->number_of_digits>b->number_of_digits)
    {
        x=a->number_of_digits;
        for(x;x>b->number_of_digits;x)
        {
            x--;
            if(a->digits[x]!='0')
                return 1;
        }
    }
    else if(a->number_of_digits<b->number_of_digits)
    {
        x=b->number_of_digits;
        for(x;x>a->number_of_digits;x) {
            x--;
            if (b->digits[x] != '0')
                return -1;
        }
    }
    else
        x=a->number_of_digits;
    for(x;x>=0;x)
    {
        x--;
        if(a->digits[x]>b->digits[x])
            return 1;
        else if(a->digits[x]<b->digits[x])
            return -1;
    }
    return 0;
}
void updateBignum(Bignum_t* toUpdate)
{
    toUpdate->number_of_digits=strlen(toUpdate->digits);
    while(toUpdate->capacity<toUpdate->number_of_digits)
        toUpdate->capacity=toUpdate->capacity*2;
}

Bignum_t* incBignum(Bignum_t* toInc)
{
    Bignum_t*  num1=createBignum(INITIAL_CAPACITY);
    num1=addToBignum(num1,'1');
    Bignum_t* output=ultimateAdd(toInc,num1);
    freeBignum(num1);
    return output;
}

Bignum_t* fixSign(Bignum_t* result)
{
    for(int i=0;i<result->number_of_digits;i++)
        result->digits[i]=toChar('9'-result->digits[i]);
    result=incBignum(result);
    result->sign=-1;
}

void swap(char* digits ,int a,int b)
{
    char mem;
    mem=digits[a];
    digits[a]=digits[b];
    digits[b]=mem;

}

void  backwards(char* digits)
{
    int i=0;
    int j=strlen(digits)-1;
    while(i<j)
        swap(digits,i++,j--);
}
Bignum_t* createBignum(unsigned capacity)
{
    Bignum_t* bignum= (Bignum_t*)malloc(sizeof(Bignum_t));

    bignum->digits=(char*)malloc(capacity*sizeof(char));
    memset(bignum->digits,0,sizeof(bignum->digits));

    bignum->number_of_digits=0;
    bignum->capacity=capacity;
    bignum->sign=1;

    return bignum;
}

void freeBignum(Bignum_t* bignum)
{
    free(bignum->digits);
    bignum->digits = NULL;
    free(bignum);
}

Bignum_t* clearBignum(Bignum_t* bignum)
{
    freeBignum(bignum);
    bignum=createBignum(INITIAL_CAPACITY);
    return bignum;
}

Bignum_t* cloneBignum(Bignum_t* b)
{
    Bignum_t* clon= createBignum(b->capacity);
    clon->number_of_digits=b->number_of_digits;
    clon->capacity=b->capacity;
    memcpy(clon->digits,b->digits,b->number_of_digits);
    backwards(clon->digits);
    clon->sign=b->sign;
    return clon;
}

Bignum_t* addToBignum (Bignum_t* b,char item)
{
    if(b->number_of_digits < b->capacity){
        b->digits[b->number_of_digits++]=item;
        return b;
    }
    else
    {
        Bignum_t* newBignum=createBignum(b->capacity*2);
        newBignum->number_of_digits=b->number_of_digits;
        memcpy(newBignum->digits,b->digits,b->number_of_digits);
        newBignum->digits[newBignum->number_of_digits++]=item;
        newBignum->sign=b->sign;
        freeBignum(b);
        return newBignum;
    }
}




// function to create a stack of given capacity. It initializes size of
// stack as 0
Stack_t* createStack(unsigned capacity)
{
    Stack_t* stack = (Stack_t*) malloc(sizeof(Stack_t));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (Bignum_t**) malloc((stack->capacity+1) * sizeof(Bignum_t*));
    return stack;
}

// Stack is full when top is equal to the last index
int isFull(Stack_t* stack)
{   return stack->top == stack->capacity - 1; }

// Stack is empty when top is equal to -1
int isEmpty(Stack_t* stack)
{   return stack->top == -1;  }

// Function to add an item to stack.  It increases top by 1
void push(Stack_t* stack,Bignum_t* item)
{
    if (isFull(stack))
        return;
    Bignum_t* newbignum=cloneBignum(item);
    stack->array[++stack->top] = newbignum;
}

// Function to remove an item from stack.  It decreases top by 1
Bignum_t* pop(Stack_t* stack)
{
    if (isEmpty(stack))
        return (Bignum_t*)0;
    return stack->array[stack->top--];
}

// Function to peek at the topo of stack
Bignum_t* peek(Stack_t* stack)
{
    if (isEmpty(stack))
        return (Bignum_t*)0;
    return stack->array[stack->top];
}

void clear(Stack_t* stack)
{
    Bignum_t *item;

    while(!isEmpty(stack))
    {
        item = pop(stack);
        free(item->digits);
        free(item);
    }
}

int call_asm_operation(char action,int a,int b)
{
    int resDig=0;
    switch(action)
    {
        case '+' :
            resDig=_asm_add(a, b);
            break;
        case '-' :
            resDig=_asm_sub(a, b);
            break;
        case '*' :
            resDig= _asm_mult(a, b);
            break;
    }
    return resDig;
}

// Driver program to test above functions
int main()
{
    Stack_t*   stack = createStack(1024);
    Bignum_t*  num1=createBignum(INITIAL_CAPACITY);
    addToBignum(num1,'1');
    char      reader;
    Bignum_t*  num=createBignum(INITIAL_CAPACITY);
    Bignum_t*  resultToPush;
    Bignum_t*  op_a;
    Bignum_t*   op_b;
    Bignum_t*   toPrint;
    int carry =0;
    int ans=0;
    int end=0;
    int firstZeroes=1;
    int isPrinted=0;

    unsigned maxLength=0;


    do
    {
        reader=fgetc(stdin);
        switch(reader){

            case '0'...'9':

                num=addToBignum(num,reader);
                break;

            case '_' :

                if(num->number_of_digits == 0)
                {
                    num->sign=-1;
                }
                break;

            case '+' :

                if(num->number_of_digits > 0)
                {
                    push(stack,num);
                    freeBignum(num);
                    num=createBignum(INITIAL_CAPACITY);
                }
                op_b = pop(stack);
                op_a = pop(stack);
                resultToPush=ultimateAdd(op_a,op_b);
                push(stack, resultToPush);
                freeBignum(op_a);
                freeBignum(op_b);
                freeBignum(resultToPush);
                break;

            case '-' :

                if(num->number_of_digits > 0)
                {
                    push(stack,num);
                    freeBignum(num);
                    num=createBignum(INITIAL_CAPACITY);
                }
                op_b = pop(stack);
                op_a = pop(stack);
                resultToPush=ultimateSub(op_a,op_b);
                push(stack, resultToPush);
                freeBignum(op_a);
                freeBignum(op_b);
                freeBignum(resultToPush);
                break;

            case '/' :
                if(num->number_of_digits > 0)
                {
                    push(stack,num);
                    clearBignum(num);
                }
                op_b = pop(stack);
                op_a = pop(stack);
                resultToPush = ultimateDiv(op_a, op_b);
                backwards(resultToPush->digits);
                push(stack, resultToPush);
                freeBignum(op_a);
                freeBignum(op_b);

                break;


            case '*' :
                    if(num->number_of_digits > 0)
                    {
                        push(stack,num);
                        clearBignum(num);
                    }
                    op_b = pop(stack);
                    op_a = pop(stack);
                    resultToPush = ultimateMult(op_a, op_b);
                    backwards(resultToPush->digits);
                    push(stack, resultToPush);
                    freeBignum(op_a);
                    freeBignum(op_b);

                break;

            case 'p' :

                isPrinted=0;
                firstZeroes=1;
                toPrint = peek(stack);
                if(toPrint->sign==-1)
                    printf("-");
                end = strlen(toPrint->digits) - 1;
                for (int x = end; x >= 0; --x)
                    if(toPrint->digits[x]!='0'||firstZeroes==0)
                    {
                        printf("%c", toPrint->digits[x]);
                        firstZeroes = 0;
                        isPrinted=1;
                    }
                (isPrinted==0)?printf("%c\n",'0'):printf("\n");

                break;

            case 'c' :

                clear(stack);
                break;

            case 'q' :
                break;

            default:
                if((reader <= 32) && (num->number_of_digits > 0))
                {
                    push(stack,num);
                    freeBignum(num);
                    num=createBignum(INITIAL_CAPACITY);

                }
                break;

        }
    }
    while(reader!='q');
    return 0;
}


