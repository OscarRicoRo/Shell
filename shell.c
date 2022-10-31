#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <sys/wait.h>
#include <ctype.h>


#define BOLD_BLUE   "\x1b[1;34m"  //color azul en negrita para printf
#define COLOR_RESET   "\x1b[0m"  //reset a color default

#define MAX_LINE_LENGTH 1024	//número máximo de carácteres que puede escribir el usuario en la línea de comandos
#define SPACE_CHARS " \t\n\r"  //espaciadores para la función parse command
#define MAX_ARGS 128	//número máximo de argumentos
#define PATH_MAX 100	//número máximo de carácteres para la dirección

/*
###########################
#   VARIABLES GLOBALES    #
###########################
*/

int len; //variable global para almacenar el tamaño de la array de argumentos.
/*
###########################
#	PARSE COMMAND     #
###########################
*/
bool parse_command(char* command,char* argv[],size_t max_args)
{
  if(!argv || !max_args) return false;
  
  len=0;

  char* token=strtok(command,SPACE_CHARS);
  while(token!=NULL)
  {
    if(len==max_args-1) 
    { argv[len]=NULL; return false; }

    argv[len++]=token;
    //printf("--> %d: \"%s\"\n",token_nbr,token); 
    token=strtok(NULL,SPACE_CHARS);
  }
  
  argv[len]=NULL;
  return true;
}

int main(int argc, char *argv[])
{
  pid_t child_pid;		//creación de un child pid
  char input_line[MAX_LINE_LENGTH]; //variable donde se almacenan los comandos que escribe el usuario en la consola
  char* args[MAX_ARGS];	     //array donde se guardan los argumentos una vez han pasado por el parse command
  char cwd[PATH_MAX];		     //variable donde se guarda la dirección en la que se encuentra el usuario en todo momento

  while(true)
  {
/*
###########################
# PRINT F CON DIRECCIÓN   #
###########################
*/
    getcwd(cwd,PATH_MAX);
    printf(BOLD_BLUE "%s" COLOR_RESET "$ ",cwd);
    if(fgets(input_line,MAX_LINE_LENGTH,stdin)!=NULL)
    {
      if(parse_command(input_line,args,MAX_ARGS))
      {
/*
###########################
#  	MINÚSCULAS        #
###########################
*/
      	char* string = args[0];  //puntero apuntando al primer argumento de la array
        for(string; *string != '\0'; string++){
	  *string = tolower(*string);
	}
/*
###########################
#       QUIT/EXIT         #
###########################
*/
        if(strcmp(args[0],"quit")==0 || strcmp(args[0],"exit")==0  ) { 
          return EXIT_SUCCESS;
        }
/*
###########################
#          CD             #
###########################
*/
        else if(strcmp(args[0],"cd")== 0){
          if(chdir(args[1]) != 0){
            printf("No existe el archivo o directorio %s. \n",args[1]);
          }
        }
/*
###########################
#         EXECUTE         #
###########################
*/
        else{

        switch(child_pid=fork()) 
  	{
    		case -1://Error Proceso
      	{
        	printf("Error...");
      	}

    		case 0: //Proceso hijo
      	{	
      		if(strcmp(args[len-1],"&")==0){
        	args[len-1] = NULL;		
        	}
      		if(execvp(args[0], args)){	
        	printf("Comando invalido. %s no existe.\n",args[0]);
        	exit(1);
        	}
      	}
	default:	//Proceso padre
      	{
        	if(strcmp(args[len-1],"&")==0){ 
        	printf("%d \n",child_pid);
        	}
        	else{	
        	int status;	//creamos un status
        	pid_t wait_pid;
        	if(wait_pid=waitpid(child_pid,&status,0) == -1){ 
        	  printf("Error en el proceso padre \n");
        	  break;
        	  }
        	}
     	      }
            }
          }
        } 
      }
    }
return EXIT_SUCCESS;
}
