#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
using namespace std;
struct command_t 
{
  char *name;
  int argc;
  char *argv[20];
};
void printPrompt();
void readCommand(char*buffer);//get input 
void parsePath(char path[],char*dirs[],int &size);//parse the possible file path 
void parseCommand(char*command,command_t *obj,int &commandsize); //parse the command into parameters 
char* lookupPath(char**path,char**dir,int size); //checking if path exists or not 
void internalCommandCD(char **path); //internal command
void showHelp(); //internal command 
bool searchPipe(char*comand);
void parsePipe (char*comand,char**arr,int &count); //takes command name then see for | 
void parsePipeCommand(char**arr,int size,char***temp);//converts 2d pointer to 3d pointer
void executePipe(char***path,int n);//run the pipe
bool checkforRedirectionExists(char* comand);
int checkRedirection(char**path,int n);//searches for the index
void executeRedirection(char**path,int n);
bool checkBackgroundProcess(char * arr); //checks if the process ends with &
