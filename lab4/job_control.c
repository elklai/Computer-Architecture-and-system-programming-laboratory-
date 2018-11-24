#include "job_control.h"
#include <termios.h>
#define DONE -1
#define RUNNING 1
#define SUSPENDED 0


/**
* Receive a pointer to a job list and a new command to add to the job list and adds it to it.
* Create a new job list if none exists.
**/
job* add_job(job** job_list, char* cmd){
	job* job_to_add = initialize_job(cmd);
	
	if (*job_list == NULL){
		*job_list = job_to_add;
		job_to_add -> idx = 1;
	}	
	else{
		int counter = 2;
		job* list = *job_list;
		while (list -> next !=NULL){
			printf("adding %d\n", list->idx);
			list = list -> next;
			counter++;
		}
		job_to_add ->idx = counter;
		list -> next = job_to_add;
	}
	return job_to_add;
}


/**
* Receive a pointer to a job list and a pointer to a job and removes the job from the job list 
* freeing its memory.
**/
void remove_job(job** job_list, job* tmp){
	if (*job_list == NULL)
		return;
	job* tmp_list = *job_list;
	if (tmp_list == tmp){
		*job_list = tmp_list -> next;
		free_job(tmp);
		return;
	}
		
	while (tmp_list->next != tmp){
		tmp_list = tmp_list -> next;
	}
	tmp_list -> next = tmp -> next;
	free_job(tmp);
	
}

/**
* receives a status and prints the string it represents.
**/
char* status_to_str(int status)
{
  static char* strs[] = {"Done", "Suspended", "Running"};
  return strs[status + 1];
}


/**
*   Receive a job list, and print it in the following format:<code>[idx] \t status \t\t cmd</code>, where:
    cmd: the full command as typed by the user.
    status: Running, Suspended, Done (for jobs that have completed but are not yet removed from the list).
  
**/
void print_jobs(job** job_list){

	job* tmp = *job_list;
	update_job_list(job_list, FALSE);
	while (tmp != NULL){
		printf("[%d]\t %s \t\t %s", tmp->idx, status_to_str(tmp->status),tmp -> cmd); 
		
		if (tmp -> cmd[strlen(tmp -> cmd)-1]  != '\n')
			printf("\n");
		job* job_to_remove = tmp;
		tmp = tmp -> next;
		if (job_to_remove->status == DONE)
			remove_job(job_list, job_to_remove);
		
	}
 
}


/**
* Receive a pointer to a list of jobs, and delete all of its nodes and the memory allocated for each of them.
*/
void free_job_list(job** job_list){
	while(*job_list != NULL){
		job* tmp = *job_list;
		*job_list = (*job_list) -> next;
		free_job(tmp);
	}
	
}


/**
* receives a pointer to a job, and frees it along with all memory allocated for its fields.
**/
void free_job(job* job_to_remove)
{
    if(job_to_remove)
    {
        if(job_to_remove->cmd)
            free(job_to_remove->cmd);
        if(job_to_remove->tmodes)
            free(job_to_remove->tmodes);/*important to check how to deep free*///tcgetattr(epoll(),newJob->termios);//struct cmdline {int s_in;int s_out;cmdline_parse_ctx_t *ctx;struct rdline rdl;char prompt[RDLINE_PROMPT_SIZE];struct termios oldterm;
        job_to_remove->next='\0';
        free(job_to_remove);
    }
}



/**
* Receive a command (string) and return a job pointer. 
* The function needs to allocate all required memory for: job, cmd, tmodes
* to copy cmd, and to initialize the rest of the fields to NULL: next, pigd, status 
**/

job* initialize_job(char* cMd){
	job* newJob =malloc(sizeof(job));
        newJob->cmd=cMd;
        newJob->idx=-1;
        newJob->pgid='\0';//newJob->pgid=getpgid();
        newJob->status=RUNNING;
        newJob->tmodes='\0';//tcgetattr(epoll(),newJob->termios);//struct cmdline {int s_in;int s_out;cmdline_parse_ctx_t *ctx;struct rdline rdl;char prompt[RDLINE_PROMPT_SIZE];struct termios oldterm;
        newJob->next='\0';
        return newJob;
}

 
/**
* Receive a job list and and index and return a pointer to a job with the given index, according to the idx field.
* Print an error message if no job with such an index exists.
**/
job* find_job_by_index(job* job_list, int idx){
    if(job_list!='\0')
    {
        int counter=1;
        while(job_list!='\0')
        {
            if(idx==counter)
                return job_list;
            job_list=job_list->next;
            counter++;
        }
    }
perror(" no job with such an index exists");
return '\0';
}


/**
* Receive a pointer to a job list, and a boolean to decide whether to remove done
* jobs from the job list or not. 
**/
void update_job_list(job **job_list, int remove_done_jobs)
{
    job* tmp = *job_list;
    while(tmp!='\0')
    {
        if(waitpid(-tmp->pgid,&tmp->status,WNOHANG)>0);
            tmp->status=-1;
        if(remove_done_jobs==TRUE)
        {
            if(tmp->status==-1)
            {
                printf("[%d]\t %s \t\t %s", tmp->idx, status_to_str(tmp->status),tmp -> cmd);
                remove_job(job_list,tmp);
            } 
        }
        tmp=tmp->next;
    }
}

/** 
* Put job j in the foreground.  If cont is nonzero, restore the saved terminal modes and send the process group a
* SIGCONT signal to wake it up before we block.  Run update_job_list to print DONE jobs.
**/

void run_job_in_foreground (job** job_list, job *j, int cont, struct termios* shell_tmodes, pid_t shell_pgid){
     if(waitpid(-j->pgid,&j->status,WNOHANG)>0)
     {
        printf("[%d]\t %s \t\t %s", j->idx, status_to_str(j->status),j -> cmd);
        remove_job(job_list,j);
     }
     if(j->status==DONE)
         tcsetpgrp (STDIN_FILENO,j->pgid);
     if(cont==1&&j->status==SUSPENDED)
         if(waitpid(-j->pgid,&j->status,WUNTRACED|WIFSTOPPED(j->status))>0)
         {
                if(WIFSTOPPED(j->status))
                {
                    j->status=SUSPENDED;
                }
                if(WIFSIGNALED(j->status))
                {
                    j->status=DONE;
                }
         }
         
}

/** 
* Put a job in the background.  If the cont argument is nonzero, send
* the process group a SIGCONT signal to wake it up.  
**/

void run_job_in_background (job *j, int cont){	
 
}
