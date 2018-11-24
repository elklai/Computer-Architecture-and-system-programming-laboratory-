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
    job** jobs;  
    pid_t shell_pgid;
    struct termios * shell_t;
    void sigHandler(int sig);
    int* pipe3;
    void sigDfl()
    {
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
    }
    void signalToHandler()
    {
        
        signal(SIGTTOU, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGQUIT, sigHandler);
        signal(SIGCHLD, sigHandler);
        signal(SIGSTOP, sigHandler);
    }
    
    void execute(cmd_line *parsed,job* mainJob);
    
    int * newPipe(){
        int* ans = malloc(2*sizeof(int));
        pipe(ans);
        if(*ans==-1){
            perror("failed");
            exit(1);
        }
        return ans;
    }
    
    int executeHelper (cmd_line* parsed,int* pipe1,int *pipe2, job* mainJob,pid_t firstChildPid){

    pid_t childPro1;
    childPro1=fork();
    if(childPro1==0)
    {
        
    if(firstChildPid==0)
        firstChildPid=getpid();
        
        setpgid(getpgid(getpid()),firstChildPid);
        mainJob->pgid=firstChildPid;
        sigDfl();
        if(pipe2!='\0'&& parsed->next!='\0'){//why we need the second cond?
            close(pipe2[0]);
            fclose(stdout);
            dup(pipe2[1]);
            close(pipe2[1]);
            
        }
        if(pipe1!='\0')
        {
            close(pipe1[1]);
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
        setpgid(getpgid(childPro1),firstChildPid);//double check again
        mainJob->pgid=firstChildPid;
        if(pipe2!='\0')
            close(pipe2[1]);
        if(pipe1!='\0'){
            close(pipe1[0]);
            free(pipe1);
        }
        if(parsed->next!='\0'){
            pipe3=newPipe();
            executeHelper(parsed->next,pipe2,pipe3,mainJob,firstChildPid);
        }
        if(parsed!='\0' && parsed->blocking!=0)
        {
            run_job_in_foreground(jobs,mainJob,1,shell_t,shell_pgid);
            waitpid(getpid(),0,0);

            
            
        }
        
    }
return childPro1;
}
        
    int main()
    {
        jobs=malloc(sizeof(job));
        shell_t = malloc(sizeof(struct termios));
        job* mainJob;
        char buf [PATH_MAX];
        cmd_line *parsed;
        char command[2048];
        signalToHandler();
        
        getcwd(buf,sizeof(buf));
        printf("%s:",buf);
        
        setpgid(0,0);
        tcgetattr(STDIN_FILENO,shell_t) ;
        
	shell_pgid = getpgid(getpid());
        fgets(command,sizeof(command),stdin);
        while(strcmp(command,"quit\n")!=0)
        {
            mainJob=add_job(jobs,command);
            mainJob->status=1;
            if(strcmp(command,"jobs\n")==0){
                
                mainJob->status=-1;
                print_jobs(jobs);
            }
             else if (strcmp(command,"\n")!=0)
            {
                
                    parsed=parse_cmd_lines(command);
                    if (strcmp(parsed->arguments[0], "fg")==0)
                    {
                        job* job_fg = find_job_by_index(*jobs, atoi(parsed->arguments[1]));
                        run_job_in_foreground(jobs, job_fg,1 ,shell_t, shell_pgid);
                    }
                    else if (strcmp(parsed->arguments[0], "bg")==0)
                    {
                                    job* job_bg = find_job_by_index(*jobs, atoi(parsed->arguments[1]));
                                    run_job_in_background(job_bg, 1);		
                    }
                    else
                        execute(parsed,mainJob);
                    free_cmd_lines(parsed);
                
            }
            getcwd(buf,sizeof(buf));
            printf("%s:",buf);
            fgets(command,sizeof(command),stdin);
        }
        free_job_list(jobs);
        free(shell_t);
        return 0;   
    }


void execute(cmd_line *parsed,job* mainJob){
    pid_t firstChildPid =0;
    if (parsed->next!='\0'){
            int* pipe1 = newPipe();
            executeHelper(parsed, NULL, pipe1,mainJob,firstChildPid);

    }
    else 
        executeHelper(parsed, NULL, NULL,mainJob,firstChildPid);
}

void sigHandler(int sig)
{
  // printf("%s -signal is ignored\n",strsignal(sig)); 
}
 

