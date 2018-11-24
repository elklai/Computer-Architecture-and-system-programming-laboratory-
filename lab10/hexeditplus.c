
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


void (*menu[7])();
char* fileName;
unsigned char* display;
int unitSize;
unsigned  char* displayCopy;




void set_file_name();
void set_unit_size();
void file_display();
void quit();
void init();
void file_modify();

void file_modify()
{
    int val;
    int location;
    printf("please Enter location and val to place  \n");
    scanf("%x %x",&location,&val);
    if(!(fileName&&unitSize!=0))
    {
        printf("Error please set unit size and set file name first");
        return;
    }
    int fd = open(fileName,O_WRONLY);
    if(fd==-1)
    {
        printf("can't open file correctly");
        return;
    }
    if(lseek(fd,location,SEEK_SET)==-1)
    {
        printf("Error canot get to location in this file");
        return ;
    }  
    write(fd,&val,unitSize);
    close(fd);
}

void set_file_name()
{
	printf("please Enter file name\n");
	scanf("%s",fileName);
}

void set_unit_size()
{
        int temp;
	printf("please Enter unit szie\n");
	scanf("%d",&temp);
	if(temp>5||temp<1)
		printf("not valid size (1,2,4)");/*may be perror*/
	else
            unitSize=temp;
}

void file_display()
{
    int sum,mult;
    int i,j;
    int location;
    int length;
    if(fileName=='\0')
    {
        printf("Error file name is null");
        return;
    }
    int fd=open(fileName,O_RDONLY);
        if(fd==-1){
            printf("Error cannot open file");
            return;
        }
        printf("please enter a location in hex and length in dec");
        scanf("%x %d",&location,&length);
        display=malloc(length*unitSize);
        lseek(fd,location,SEEK_SET);
        read(fd,display,unitSize*length);
        close(fd);
        displayCopy=display;
        printf("Hexadecimal Representation:\n");
	for(i = 0; i < length; i++){
                if(unitSize==1)
			printf("%02x ", display[i]);
		if(unitSize==2)
                    printf("%04x ", ((unsigned short*)display)[i]);
                if(unitSize==4)
                    printf("%08x ",((unsigned int*)display)[i]);
	}
	printf ("\n");
        display=displayCopy;
        sum=0;
        mult=1;
        printf("Decimal Representation:\n");
        for (int i=0; i<unitSize * length; i=i+unitSize){
            unsigned int to_print = (unitSize==1) ? display[i] :
            (unitSize==2) ? (display[i] | display[i+1] << 8) :
            (display[i] | display[i+1] << 8) | (display[i+2] << 16 | display[i+3] << 24);
            printf("%d", to_print);
            printf(" ");
        }
        printf("\n");
        free(displayCopy);
}
 void file_copy()
 {
        char* sourceFileName= malloc(100);
        int srcOffset;
        int destOffset;
        int length;
        char* box=malloc(unitSize*length);
        int srcFd,destFd;
        printf("please enter source file,srcOffset,destOffset and length\n");
        scanf("%s %x %x %d",sourceFileName,&srcOffset,&destOffset,&length);
        if(fileName=='\0'||sourceFileName=='\0')
        {
            printf("Error please enter source and dest file name\n");
            return;
        }
        srcFd=open(sourceFileName,O_RDONLY);
        destFd=open(fileName,O_WRONLY);
        if(srcFd==-1||destFd==-1){
            printf("Error can't open source file or dest file\n");
            return;
        }
        if((lseek(srcFd,srcOffset,SEEK_SET)==-1)||(lseek(destFd,destOffset,SEEK_SET)==-1))
        {
            printf("Error can't go to offset in srcfile or destfile\n");
            return ;
        }
        if(read(srcFd,box,length)==-1)
        {
            printf("Error can't read source file\n");
            return;
        }
        if(write(destFd,box,length)==-1)
        {
            printf("Error can't write to destnation file\n");
            return;
        }
        close(srcFd);
        close(destFd);
        printf("works but need to fix this message\n");
        free(sourceFileName);
        free(box);
 }
            
        
    
void quit()
{

}

void init()
{
	fileName=malloc(100);
	menu[1]=set_file_name;
	menu[2]=set_unit_size;
        menu[3]=file_display;
        menu[4]=file_modify;
	menu[5]=file_copy;
        menu[6]=quit;
}

int main(int argc, char **argv)
 {
	init();
	int choice=0;
    	while(choice!=6)
	{
		printf("Choose action:\n1-Set File Name\n2-Set Unit Size\n3-File Display\n4-File /modify\n5-Copy_From_File\n6-Quit\n");
		scanf("%d",&choice);
		menu[choice]();
	}
}
