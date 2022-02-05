#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include "shell.h"
using namespace std;
int main()
{
    char name[100];//input from user
    char *dirs[20];//paths
    char path[100]=".:/bin/:/usr/bin/:/home/:/etc/:/var/:/dev/:/proc/";
    int pathSize=0;
    command_t command;
    char *check;//for checking internal commands
    parsePath(path,dirs,pathSize);
    cout<<"Welcome to Ubuntu's Shell now supporting Pipes, I\\O Redirection and Background Processes \n";
    int commandsize; //size of the 
    char*fullname;
    bool background;
   while(true)
   {
    commandsize=0;
     printPrompt();
     readCommand(name);//read a command from user
     background=checkBackgroundProcess(name);
     fullname=strdup(name);
     parseCommand(name,&command,commandsize);//parse it into parameters
     if(command.argv[0]!=NULL)
     {
        check=strdup(command.argv[0]);
        if(strcmp(check,"exit")==0)//terminating the program 
            break;
        else if (strcmp(check,"cd")==0) //for e.g cd ..
        {
            pid_t h=fork(); 
            if(h==0)
            {
                internalCommandCD(command.argv);
                return 0;
            }
            else 
            {
                int status;
                if(!background)
                   waitpid(h,&status,0);
            }
        }
        else if (strcmp(check,"help")==0) 
        {
            pid_t h=fork(); 
            if(h==0)
            {
                showHelp();
                return 0;
            }
            else
            {
                int status;
                if(!background)
                    waitpid(h,&status,0);
            }
        }
        else if(checkforRedirectionExists(fullname))
        {
           if(lookupPath(command.argv,dirs,pathSize)!=nullptr)
           {
                pid_t proc=fork();
                if(proc==0)
                {
                        executeRedirection(command.argv,commandsize);
                        return 0;
                }
                else 
                {
                    int status;
                    if(!background)
                            waitpid(proc,&status,0);
                }
           }
        }
        else if(searchPipe(fullname))
        {
            
            pid_t proc=fork();
            if(proc==0)
            {
                int countPipes=0;
                char*allpipes[20];
                char**solution[20];
                for(int i=0;i<20;i++)
                    solution[i]=new char*[20];
                parsePipe(fullname,allpipes,countPipes);//parse the pipe
                parsePipeCommand(allpipes,countPipes,solution);//further parse in arrays
                executePipe(solution,countPipes);//execute the pipe
                return 0;
            }
            else
            {
                int status;
                if(!background)
                    waitpid(proc,&status,0);
            }
        
        }
        else
        {
            command.name=lookupPath(command.argv,dirs,pathSize);//checking if file exist or not 
            if(command.name!=nullptr)
            {
                pid_t child=fork();
                if(child==0)
                {
                    execv(command.name,command.argv);
                    return 0;
                }
                else if(child<0)
                    cout<<"Child Process Creation Failed\n";
                else 
                {
                    int status;
                    if(!background)
                        waitpid(child,&status,0);
                       
                }
            }
        }
     }
     else
        cout<<"No command entered\n";
   }
    cout<<"Exiting the shell\n";   
    return 0;
}
