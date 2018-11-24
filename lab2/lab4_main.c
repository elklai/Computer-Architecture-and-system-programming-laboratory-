#include "lab4_util.h"
#include <sys/syscall.h>

#define BUF_SIZE 1024
#define SYS_EXIT 60
#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_LSEEK 8
#define SEEK_CUR 1
#define SYS_GETDENTS 78
#define RW 2 /* open for both reading and writing */
#define R 0
#define STDOUT 1
#define DT_DIR 4

int system_call();

 struct linux_dirent {
	long d_ino;
	long d_off;
	unsigned short d_reclen;
	char d_name[];
       };

int rec(int fd,char* path,char* file_name,char* command){
        int output=0;
	int nread;
	char buf[BUF_SIZE];
	char d_type;

	int pos=0;
	struct linux_dirent *d;

    if (fd == -1)
      system_call(SYS_EXIT, 0);
    while(1)
    {
        nread = system_call(SYS_GETDENTS, fd, buf, BUF_SIZE);
        if (nread == -1)
      		system_call(SYS_EXIT, 0);
	if(nread==0)
		break;
        for (pos = 0; pos < nread&&nread!=0;) {
			d = (struct linux_dirent *) (buf + pos);
			d_type = *(buf + pos + d->d_reclen - 1);
				int dot=simple_strcmp(d->d_name,".");
				int twoDots=simple_strcmp(d->d_name,"..");
				if(dot!=0 && twoDots!=0)
                                {
					char new_path [simple_strlen(path)+simple_strlen(d->d_name)+2];
				    	new_path[0] = '\0';	
				    	simple_strcat(new_path,path);
				    	simple_strcat(new_path,"/");
				    	simple_strcat(new_path,d->d_name);
                                        if(command==0)
                                        {
                                            if(file_name==0||simple_strcmp(file_name,d->d_name)==0)
                                            {
                                                system_call(SYS_WRITE, STDOUT, new_path, simple_strlen(new_path));
                                                system_call(SYS_WRITE, STDOUT, "\n", 1);
                                            }
                                        }
                                        else if(simple_strcmp(file_name,d->d_name)==0)
                                        {
                                            char new_command [simple_strlen(command)+simple_strlen(new_path)+2];
                                            new_command[0]='\0';
                                            simple_strcat(new_command,command);
                                            simple_strcat(new_command," ");
                                            simple_strcat(new_command,new_path); 
                                            simple_system(new_command);
                                            output=1;
                                        }
                                        
					if (d_type == DT_DIR){
						output=rec(system_call(SYS_OPEN,new_path, R , 0777),new_path,file_name,command );
					}
					
				}
		pos += d->d_reclen;
              

        }
    }
      return output;
}

int main (int argc, char* argv[]){
        int output=0;
        if(argc==3&&simple_strcmp(argv[1],"-n")==0){
            rec(system_call(SYS_OPEN,".", R , 0777), ".",argv[2],0);
        }
        else if(argc==4&&simple_strcmp(argv[1],"-e")==0){
            output=rec(system_call(SYS_OPEN,".", R , 0777), ".",argv[2],argv[3]);
            if(output==0)
            {
                char str[simple_strlen("The file ")+simple_strlen(argv[2])+simple_strlen(" Does not exist")];
                str[0]='\0';          
                simple_strcat(str,"The file ");
                simple_strcat(str,argv[2]);
                simple_strcat(str," Does not exist");
                system_call(SYS_WRITE, STDOUT,str, simple_strlen(str));
                system_call(SYS_WRITE, STDOUT,"\n", 1);
                
                
            }
            
        }
        else
        {
            system_call(SYS_WRITE, STDOUT, ".", 1);
            system_call(SYS_WRITE, STDOUT, "\n", 1);
            rec(system_call(SYS_OPEN,".", R , 0777), ".",0,0);
        }
  return 0;
}
