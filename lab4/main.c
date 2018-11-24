
    #include "line_parser.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <linux/limits.h>
    #include <unistd.h>
    #include <sys/wait.h>
    #include "job_control.h"
    job* jobs;    
    void sigHandler(int sig);
    int* pipe3;
    
    
    
    void execute(cmd_line *parsed,job* mainJob);
    void supportFG(cmd_line* fg);
    
    
    void supportFG(cmd_line* fg)
    {
        job* rejob;
        rejob=find_job_by_index(jobs,atoi(fg->arguments[1]));
        run_job_in_foreground (&rejob, rejob, 1, rejob->tmodes,rejob->pgid);
        
    }
    int * newPipe(){
        int* ans = malloc(2*sizeof(int));
        pipe(ans);
        if(*ans==-1){
            perror("failed");
            exit(1);
        }
        return ans;
    }
    
    int executeHelper (cmd_line* parsed,int* pipe1,int *pipe2, job* mainJob){

    int childPro1;
    childPro1=fork();
    if(childPro1==0)
    {
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
        setpgid(getpid(),getpid());//double check again
        mainJob->pgid=getpid();
        if(pipe2!='\0'&& parsed->next!='\0'){//why we need the second cond?
            fclose(stdout);
            dup(pipe2[1]);
            close(pipe2[1]);
            
        }
        if(pipe1!='\0')
        {
            fclose(stdin);
            dup(pipe1[0]);
            close(pipe1[0]);
        }
        
        if(parsed->output_redirect!='\0')
        {
            fclose(stdout);
            fopen(parsed->output_redirect,"w+");
        }
        if(parsed->input_redirect!='\0')
        {
            fclose(stdin);
            fopen(parsed->input_redirect,"r");
        }
        if (execvp(parsed->arguments[0],parsed->arguments) == -1) {
            perror("failed");
            exit(1);
        
        }

    }
    else{
        setpgid(getpid(),childPro1);//double check again
        mainJob->pgid=getpid();
        if(parsed!='\0'&&parsed->blocking!='\0')
            waitpid(childPro1,0,0);
        if(pipe2!='\0')
            close(pipe2[1]);
        if(pipe1!='\0'){
            close(pipe1[0]);
            free(pipe1);
        }
        if(parsed->next!='\0'){
            pipe3=newPipe();
            executeHelper(parsed->next,pipe2,pipe3,mainJob);
        }
    }
return childPro1;
}
        
    int main()
    {
        struct termios * shell_t = malloc(sizeof(struct termios));
        job* mainJob;
        char buf [PATH_MAX];
        cmd_line *parsed;
        char* command=malloc(2048);
        getcwd(buf,sizeof(buf));
        printf("%s:",buf);
        signal(SIGTTOU, sigHandler);
        signal(SIGTSTP, sigHandler);
        signal(SIGTTIN, sigHandler);
        signal(SIGQUIT, sigHandler);
        signal(SIGCHLD, sigHandler);
        setpgid(getpid(),getpid());
        tcgetattr(STDIN_FILENO,shell_t) ;
        fgets(command,sizeof(command),stdin);
        while(strcmp(command,"quit\n")!=0)
        {
            mainJob=add_job(&jobs,command);
            if(strcmp(command,"jobs\n")==0){
                print_jobs(&jobs);
            }
             else
            {
                parsed=parse_cmd_lines(command);
                execute(parsed,mainJob);
                free_cmd_lines(parsed);
            }
            getcwd(buf,sizeof(buf));
            printf("%s:",buf);
            command=malloc(2048);
            fgets(command,sizeof(command),stdin);
        }
        free_job_list(&jobs);
        return 0;   
    }


void execute(cmd_line *parsed,job* mainJob){
    if(strcmp(parsed->arguments[0],"fg")==0){
        supportFG(parsed);
        if(parsed->next!='\0')
            parsed=parsed->next;
        else
            return;
    }
    else{
        if (parsed->next!='\0'){
                int* pipe1 = newPipe();
                executeHelper(parsed, NULL, pipe1,mainJob);
        }
        else {
            executeHelper(parsed, NULL, NULL,mainJob);
            
        }
    }
}
void sigHandler(int sig)
{
   printf("%s -signal is ignored\n",strsignal(sig)); 
}
 

