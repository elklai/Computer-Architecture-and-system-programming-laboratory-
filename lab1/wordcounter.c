#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    int i;
    FILE * input=stdin;
    for(i=1; i<argc; i++){
        if(strcmp(argv[i],"-i")==0)
            input=fopen(argv[++i],"r");
        
    }
    char c;
    int wordcounter =0,counter=-1, charcounter=0, wordlencounter=0,largestword=0,linecounter=1;
    int foundLetter=1;
    printf( "#> wordcounter\n");
    do
    {
        c=fgetc(input);
        if(c<'!'||c>'~'||c==EOF){  
            foundLetter=1;
            if(wordlencounter>largestword)
                largestword=wordlencounter;
            wordlencounter=0;
            if(c=='\n')
                linecounter++;
        }
        else
        {
            if(c!=EOF)
            {
                if(foundLetter==1)
                    wordcounter++;
            foundLetter=0;
            charcounter++;
            wordlencounter++;
            }
        }
    }
    while(c!=EOF);
    
    
    for(i=1; i<argc; i++)
    {
        if(strcmp(argv[i],"-l")==0)
        {
            counter=largestword;
            printf("%d\n",counter);
        }
        else if(strcmp(argv[i],"-c")==0)
        {
            counter=charcounter;
            printf("%d\n",counter);
        }
        else if(strcmp(argv[i],"-w")==0)
        {
            counter=wordcounter;
            printf("%d\n",counter);
        }
        else if(strcmp(argv[i],"-n")==0)
        {
            counter=linecounter;
            printf("%d\n",counter);

        }
    }
    if(counter==-1)
    {
        counter=wordcounter;
        printf("%d\n",counter);
    }
    return 0;
}







