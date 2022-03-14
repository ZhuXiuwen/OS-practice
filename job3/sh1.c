#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define SIZE 256
#define MAX_ARGC 16
struct Command{
	int argc;
	char *argv[MAX_ARGC];
}commands;
void parse_command(char *command){
	char *line = (char*)malloc(sizeof(command));
	strcpy(line, command);
	commands.argc = 0;
	for(int i = 0; i < MAX_ARGC; i++)
		commands.argv[i] = NULL;
	char *word;
	char *delim = " \n";
	word = strtok(line, delim);
	while(word != NULL){
		commands.argv[commands.argc++] = word;
		word = strtok(NULL, delim);
	}
}
void handle_built_in_command(){
	if(strcmp(commands.argv[0], "cd") == 0){
		int error = chdir(commands.argv[1]);
		if(error < 0)
			perror("cd");

		else{
			char *path = getcwd(NULL, 0);
			printf("current working directory: %s\n", path);
			free(path);
		}
	}
	else if(strcmp(commands.argv[0],"pwd") == 0){
		char *path = getcwd(NULL, 0);
		printf("current working directory: %s\n", path);
		free(path);
	}
	else if(strcmp(commands.argv[0], "exit") == 0)
		exit(0);
}
void mysys(char *command){
	parse_command(command);
	if(strcmp(commands.argv[0], "cd") == 0 || strcmp(commands.argv[0], "pwd") == 0 || strcmp(commands.argv[0], "exit") == 0){
		handle_built_in_command();
		return;
	}
	pid_t pid;
	pid = fork();
	if(pid == 0){
		int error = execvp(commands.argv[0], commands.argv);
		if(error < 0)
			perror("execvp");
	}
	wait(NULL);
}
int main(){
	char command[SIZE];
	int count;
	while(1){
		write(1, ">", sizeof(">"));
		count = read(0, command, sizeof(command));
		command[count] = 0;
		mysys(command);
	}
	return 0;
}
