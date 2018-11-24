#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "elf-1.h"

void (*menu[5])();
char* fileName;
unsigned char* display;
int unitSize;
unsigned  char* displayCopy;
int currentFd=0;
Elf64_Ehdr *header;
 void *map_start; /* will point to the start of the memory mapped file */

void examine_elf_file();

void quit();
void init();

void examine_elf_file()
{
	printf("please Enter file name\n");
	scanf("%s",fileName);
       // if(currentFd!=0);
            //need to close any open file 
   int fd;
  
   struct stat fd_stat; /* this is needed to  the size of the file */
    /* this will point to the header structure */
    if( (fd = open(fileName, O_RDWR)) < 0 ) {
      perror("error in open");
      exit(-1);
   }

   if( fstat(fd, &fd_stat) != 0 ) {
      perror("stat failed");
      exit(-1);
   }

   if ( (map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      exit(-4);
   }

   /* now, the file is mapped starting at map_start.
    * all we need to do is tell *header to point at the same address:
    */

   header = (Elf64_Ehdr *) map_start;
   /* now we can do whatever we want with header!!!!
    * for example, the number of section header can be obtained like this:
    */
   //num_of_section_headers = header->e_shnum;

   /* now, we unmap the file */
  // munmap(map_start, fd_stat.st_size);
  
  char* megic=malloc(4);
  for(int i=1;i<4;i++)
  {
      megic[i-1]=header->e_ident[i];
  }
  megic[3]='\0';
  printf("%s\n",megic);
  printf("%x\n",(int)header->e_entry);
  if(strcmp(megic, "ELF")!=0){
      perror("Error not an ELF file");
      exit(0);
}

printf("The data encoding scheme of the object file:\n");
printf("%x\n",(int)header->e_ident[5]);
printf("Entry point (hexadecimal address):\n");
printf("%x\n",(int)header->e_entry);
printf("The file offset in which the section header table resides:\n");
printf("%d\n",(int)header->e_shoff);
printf("The number of section header entries:\n");
printf("%d\n",(int)header->e_shnum);
printf("The size of each section header entry:\n");
printf("%d\n",(int)header->e_shentsize);
printf("The file offset in which the program header table resides:\n");
printf("%d\n",(int)header->e_phoff);
printf("The number of program header entries:\n");
printf("%d\n",(int)header->e_phnum);
printf("The size of each program header entry:\n");
printf("%d\n",(int)header->e_phentsize);

  
}

            
        
void print_section_names()
{
    	Elf64_Shdr *curr_sec;
	int i;
	Elf64_Shdr *section_Str_Tbl= (Elf64_Shdr *)(map_start+(header->e_shoff)+(header->e_shstrndx)*(header->e_shentsize));
	char* str_tbl=(char *)map_start+section_Str_Tbl->sh_offset;
	printf("[Nr] Name\t\t\tAddr\t off\tSize\tType\n");
	for(i=0; i< header->e_shnum;i++){
		curr_sec=(Elf64_Shdr *)(map_start+(header->e_shoff)+i*(header->e_shentsize));
		if(i==0)
			printf("[ %d] %s \t\t\t\t%08x %06x %06x  %d\n", i, (str_tbl+curr_sec->sh_name),(unsigned int) curr_sec->sh_addr, (unsigned int)curr_sec->sh_offset,(unsigned int) curr_sec->sh_size, curr_sec->sh_type);
		else if(strlen((str_tbl+curr_sec->sh_name)) > 8)
			printf("[ %d] %s \t\t%08x %06x %06x  %d\n", i, (str_tbl+curr_sec->sh_name),(unsigned int) curr_sec->sh_addr, (unsigned int)curr_sec->sh_offset,(unsigned int) curr_sec->sh_size, curr_sec->sh_type);
		else
			printf("[ %d] %s \t\t\t%08x %06x %06x  %d\n", i, (str_tbl+curr_sec->sh_name),(unsigned int) curr_sec->sh_addr, (unsigned int)curr_sec->sh_offset,(unsigned int) curr_sec->sh_size, curr_sec->sh_type);
	}
}

void print_symblos()
{
	Elf64_Ehdr *header = (Elf64_Ehdr *) map_start;
	Elf64_Shdr *sectionH;
	Elf64_Shdr *sectionH_D;
	Elf64_Shdr *symbolH;
	Elf64_Sym  *symboleTable;
	char *sectionStrTable;
	char *symbolStrTable;
	int numOfSymbols , numOfSymbols_d;
	
	sectionH = (Elf64_Shdr *)( map_start + ( header -> e_shoff )+( header -> e_shstrndx ) * ( header -> e_shentsize ) );
	sectionStrTable =  ( map_start + ( sectionH -> sh_offset ) );
	
	
        for( int i = 0 ; i < header -> e_shnum ; i++ )
	{
		sectionH = (Elf64_Shdr *)( map_start + ( header -> e_shoff ) +i * ( header -> e_shentsize ) );
                if( ( sectionH -> sh_type ) == SHT_DYNSYM ) 
                {
                    symbolH = (Elf64_Shdr *)( map_start + ( header -> e_shoff ) +( sectionH -> sh_link ) * ( header -> e_shentsize ) );
                    symbolStrTable =  ( map_start + ( symbolH -> sh_offset ) );
                    numOfSymbols_d = sectionH -> sh_size / 24;
                    if( numOfSymbols_d == 0 )
                    {
                        printf("No Symbols To Print!\n");
                        return;
                    }
                    printf("Symbol table '.dynsym' contains %d entries:\n" , numOfSymbols_d );
                    printf("   [Num] Value\t\t Ndx\t Section\t\t Name\n");
                    for( int i = 0 ; i < numOfSymbols_d ; i++ )
                    {
                        symboleTable = (Elf64_Sym *)( map_start + ( sectionH -> sh_offset ) +i * 24 );
                        sectionH_D = (Elf64_Shdr *)( map_start + ( header -> e_shoff )+( symboleTable -> st_shndx ) * ( header -> e_shentsize ) );
                        printf("   [%d]\t %08lx\t UND\t %s\t\t %s \n", i , symboleTable -> st_value ,( sectionStrTable + sectionH_D -> sh_name ) ,( symbolStrTable + symboleTable -> st_name ) );
                    }
                }
                
                if( ( sectionH -> sh_type ) == SHT_SYMTAB )
                {
                    numOfSymbols = sectionH -> sh_size / 24;
                    symbolH = (Elf64_Shdr *)( map_start + ( header -> e_shoff ) +( sectionH -> sh_link ) * ( header -> e_shentsize ) );
                    symbolStrTable =  ( map_start + ( symbolH -> sh_offset ) );
                    if( numOfSymbols == 0 )
                    {
                        printf("No Symbols To Print!\n");
                        return;
                    }
                    printf("\nSymbol table '.symtab' contains %d entries:\n" , numOfSymbols );
                    printf("   [Num] Value\t\t Ndx\t Section\t\t Name\n");
                    for( int i = 0 ; i < numOfSymbols ; i++ )
                    {
                        symboleTable = (Elf64_Sym *)( map_start + ( sectionH -> sh_offset ) +i * 24 );
                        sectionH_D = (Elf64_Shdr *)( map_start + ( header -> e_shoff )+( symboleTable -> st_shndx ) * ( header -> e_shentsize ) );
                        if( ( symboleTable -> st_shndx <= header -> e_shnum -1 ) &&( symboleTable -> st_shndx != 0)                        )
                        {
                                printf("   [%d]\t %08lx\t %d\t %s\t\t %s \n",i , symboleTable -> st_value , symboleTable -> st_shndx ,
                                       (sectionStrTable + sectionH_D -> sh_name) ,( symbolStrTable + symboleTable ->st_name ) );     
                        }
                        else if( symboleTable -> st_shndx == 0)
                        {
                                printf("   [%d]\t %08lx\t UND\t %s\t\t %s \n", i , symboleTable -> st_value ,( sectionStrTable + sectionH_D -> sh_name ) ,
                                                ( symbolStrTable + symboleTable -> st_name ) );
                        }		
                        else
                        {
                                printf("   [%d]\t %08lx\t ABS\t ABS\t\t %s \n" , i , symboleTable -> st_value ,
                                                        ( symbolStrTable + symboleTable -> st_name ) );
                        }
                    }
                }
        }
}



void quit()
{

}

void init()
{
	fileName=malloc(100);
	menu[1]=examine_elf_file;
        menu[2]=print_section_names;
        menu[3]=print_symblos;
        menu[4]=quit;
}

int main(int argc, char **argv)
 {
	init();
	int choice=0;
    	while(choice!=4)
	{
		printf("Choose action:\n1-Examine ELF File\n2-Print Section Names\n3Print Symblos\n4-Quit\n");
		scanf("%d",&choice);
		menu[choice]();
	}
}
