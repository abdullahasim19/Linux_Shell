#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include "shell.h"
using namespace std;
void printPrompt()
{
    cout<<"abdullah@abdullah:~$ "; //machine name 
}
void readCommand(char*buffer)//get input 
{
    cin.getline(buffer,100);
}
void parsePath(char path[],char*dirs[],int &size)//parse the possible file path 
{
    char *parsed=strtok(path,":");
    for (int i=0;parsed!=nullptr;i++)
    {
        dirs[i]=strdup(parsed);
        parsed=strtok(NULL,":");
        size++;
    }
}   
void parseCommand(char*command,command_t *obj,int &commandsize) //parse the command into parameters 
{
    char*parsed=strtok(command," ");
    int i;
    for(i=0;parsed!=nullptr;i++)
    {
        obj->argv[i]=strdup(parsed);
        parsed=strtok(NULL," ");
        commandsize++;
    }
    obj->argv[i]=NULL;
}
char* lookupPath(char**path,char**dir,int size) //checking if path exists or not 
{
    char*result;
    if(path[0][0]=='/')
        return path[0];
    if(access(path[0],F_OK)==0)
        return path[0];
    for(int i=0;i<size;i++)
    {
        result=strdup(dir[i]);
        strcat(result,path[0]);
        if(access(result,F_OK)==0)
            return result;
    }
    cout<<"Command "<<path[0]<<" not found\n";
    return nullptr;
}
    
void internalCommandCD(char **path) //internal command
{
    if(path[1]!=nullptr)
     {
        if(chdir(path[1])==0)
        {
             char arr[100];
             cout<<getcwd(arr,100)<<endl;
        }
         else
             cout<<"No such File or directory\n";
    }
    else
        cout<<"No File Path Entered\n";
}
void showHelp() //internal command 
{
    cout<<"GNU bash, version 5.0.17(1)-release (x86_64-pc-linux-gnu)"<<endl;
    cout<<"These shell commands are defined internally.  Type help to see this list."<<endl;
    cout<<"Type help name to find out more about the function name."<<endl;
    cout<<"Use info bash to find out more about the shell in general."<<endl;
    cout<<"Use man -k' or `info' to find out more about commands not in this list."<<endl;
}
bool searchPipe(char*comand)
{
   for(int i=0;i<strlen(comand);i++)
   {
      if(comand[i]=='|')
         return true;
   }
   return false;
}
void parsePipe (char*comand,char**arr,int &count) //takes command name then see for | 
{
   char*parsed=strtok(comand,"|");
   for(int i=0;parsed!=nullptr;i++)
   {
        arr[i]=strdup(parsed);
       parsed=strtok(NULL,"|");
       count++;
   }
}
void parsePipeCommand(char**arr,int size,char***temp)//converts 2d pointer to 3d pointer
{
   int i;
   for(i=0;i<size;i++)
   {
      char*parse=strtok(arr[i]," ");
      for(int j=0;parse!=NULL;j++)
      {
         temp[i][j]=strdup(parse);
         parse=strtok(NULL," ");
      }
   }
}

void executePipe(char***path,int n)//run the pipe
{
   int **pipes=new int*[n];
   for(int l=0;l<n;l++)
   {
        pipes[l]=new int [2];
        pipe(pipes[l]);
   }
   for(int i=0;i<n;i++)
   {
      pid_t h=fork();
      if(h==0)
      {
         if(i==0) //if first command redirect output only
         {
            dup2(pipes[i][1],STDOUT_FILENO);
         }
         else if (i==n-1)//if last command redirect input from last command only 
         {
             dup2(pipes[i-1][0],STDIN_FILENO);
         }
         else //if a middle command redirect input from previous and output from new 
         {
             dup2(pipes[i-1][0],STDIN_FILENO);
             dup2(pipes[i][1],STDOUT_FILENO);
         }
         if(execvp(path[i][0],path[i])==-1)
         {
             cout<<"Invalid Command\n";
             exit(EXIT_FAILURE);
         }
      }
      else
      {
        wait(nullptr);
        //now closing all the used pipes
          if(i==0)
          {
              close(pipes[i][1]);
          }
          else if(i==n-1)
          {
              close(pipes[i-1][0]);
          }
          else
          {
              close(pipes[i-1][0]);
              close(pipes[i][1]);
          }
      }
   }  
     
}
bool checkforRedirectionExists(char* comand)
{
    for(int i=0;i<strlen(comand);i++)
    {
        if(comand[i]=='<'||comand[i]=='>')
            return true;
    }
    return false;
}
int checkRedirection(char**path,int n)//searches for the index
{
    for(int i=0;i<n;i++)
    {
        if(strcmp(path[i],"<")==0 || strcmp(path[i],">")==0)
            return i;
    }
    return -1;
}
void executeRedirection(char**path,int n)
{
    int pos=checkRedirection(path,n);
    char**name=new char*[pos+1];
    int i;
    if(pos!=-1)
    {
        for(i=0;i<pos;i++)
        {
            name[i]=strdup(path[i]);
        }
        name[i]=NULL;
        pid_t h=fork();
        if(h==0)
        {
            for(int j=i;j<n;j++)
            {
                if(strcmp(path[j],"<")==0)
                {
                    
                    j++;
                    int fd=open(path[j],O_RDWR);
                    if(fd==-1)//if file don't exist exit the function
                    {
                        cout<<path[j]<<" not found\n";
                        return;
                    }
                    dup2(fd,0);  //input
                    close(fd);
                }
                else if(strcmp(path[j],">")==0)
                {
                    j++;
                    int dl=open(path[j],O_RDWR);
                    if(dl==-1)
                    {
                        cout<<path[j]<<" not found\n";
                    return;
                    }
                    dup2(dl,1);
                    close(dl);
                }  
            }
            execvp(path[0],name);
        }
        else
        {
                wait(nullptr);
        }
    }
}
bool checkBackgroundProcess(char * arr) //checks if the process ends with &
{
   if(arr[strlen(arr)-1]!='&')
        return false;
    arr[strlen(arr)-1]='\0';//removes the & so command can run 
    return true;
}