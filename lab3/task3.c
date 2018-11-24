
    #include "line_parser.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <limits.h>
    cmd_line *line;
    // execute
    int childPro;
    char command[2048];
    cmd_line * parsed;
    char buf [PATH_MAX];
    pid_t error=0;

    int main()
    {
        getcwd(buf);
        printf("%s:",buf);
        fgets(command,sizeof(command),stdin);
        while(strcmp(command,"quit\n")!=0)
        {
            parsed=parse_cmd_lines(command);
            execute(parsed);
            getcwd(buf);
            printf("%s:",buf);
            fgets(command,sizeof(command),stdin);
        }
        return 0;   
    }


    void execute(cmd_line *parsed){
    int pi[2];
    int childPro1;
    int childPro2;


    if(pipe(pi)==-1){
        perror("failed");
        exit(1);
    }

    childPro1=fork();
    if(childPro1==0)
    {
        if(parsed->next!='\0')
            fclose(stdout);
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
        dup(pi[1]);
        close(pi[1]);
        if (execvp(parsed->arguments[0],parsed->arguments) == -1) {
            perror("failed");
            exit(1);
        }

    }
    else
    {
        close(pi[1]);
        childPro2=fork();
        if(childPro2==0)
        {

            if(parsed->next->output_redirect!='\0')
            {
                fclose(stdout);
                fopen(parsed->next->output_redirect,"w+");
            }
            if(parsed->next->input_redirect!='\0')
            {
                fclose(stdin);
                fopen(parsed->next->input_redirect,"r");
            }
            fclose(stdin);
            dup(pi[0]);
            close(pi[0]);
            if (execvp(parsed->next->arguments[0],parsed->next->arguments) == -1) {
                perror("failed");
                exit(1);
            }

        }
        else
        {
            close(pi[0]);
            waitpid(childPro1,0,0);
            waitpid(childPro2,0,0);
        }
                
    }
}
    
