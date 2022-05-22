#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 256
#define MAX_ARGC 16
#define MAX_COMMANDS 16
#define MAX_SIZE 256

struct command {
  int argc;
  char *argv[MAX_ARGC];
  char *input;    //用于重定向输入
  char *output;   //用于重定向输出
};
int command_count;
struct command commands[MAX_COMMANDS];


void panic(char *message) {
  perror(message);
  exit(EXIT_FAILURE);
}

// 用 ' ' 作为分隔符切割 line, 将一条命令按空格分割
void parse_command(char *line, int index) {
  char *field = malloc(sizeof(line));
  strcpy(field, line);
  char *delim = " \n";
  commands[index].argc = 0;
  char *ptr = NULL;

  for(int i = 0; i < MAX_ARGC; i++) 
    commands[index].argv[i] = NULL;
  commands[index].input = NULL;
  commands[index].output = NULL;

  char *word = strtok_r(field, delim, &ptr);
  while(word) {
    if(strchr(word, '>'))
      commands[index].output = word + 1;
    else if(strchr(word, '<')) 
      commands[index].input = word + 1;
    else
      commands[index].argv[commands[index].argc++] = word;
    word = strtok_r(NULL, delim, &ptr);
  }
}

// 用 '|' 作为分隔符切割 line, 将多条命令按 | 分割
void parse_commands(char *line) {
  // char *fields = malloc(sizeof(line));
  char *fields = (char *)malloc(MAX_SIZE);
  strcpy(fields, line);
  char *delim = "|\n";
  char *ptr = NULL;

  command_count = 0;
  char *field = strtok_r(fields, delim, &ptr);
  while(field) {
    parse_command(field, command_count);
    command_count++;
    field = strtok_r(NULL, delim, &ptr);
  }
}

// 打印 command 的 argc 和 argv, 验证字符串处理是否正确
void command_dump(struct command command) {
  printf("   argc = %d\n", command.argc);
  printf("   argv = {");
  for(int i = 0; i < command.argc; i++) {
    printf("\"%s\"", command.argv[i]);
    if(i < command.argc - 1)
      printf(", ");
  }
  printf("}\n");
}

// 打印 commands 中的每一个 command, 验证字符串处理是否正确
void commands_dump() {
  printf("\n-------------------parse commands start-------------------\n");
  for(int i = 0; i < command_count; i++) {
    printf("\ncommand[%d]\n", i);
    command_dump(commands[i]);
  }
  printf("\n-------------------parse commands end---------------------\n\n");
}


// 处理内置命令cd、pwd、exit
void handle_built_in_command(int index) {
  if(strcmp(commands[index].argv[0], "cd") == 0) {
    int error = chdir(commands[index].argv[1]);
    if(error < 0) 
      perror("cd");
    else {
      char *path = getcwd(NULL, 0);
      printf("current working directory: %s\n", path);
      free(path);
    }
  }
  else if(strcmp(commands[index].argv[0], "pwd") == 0) {
    char *path = getcwd(NULL, 0);
    printf("current working directory: %s\n", path);
    free(path);
  }
  else if(strcmp(commands[index].argv[0], "exit") == 0) {
    exit(0);
  }
}

// 处理文件重定向
void handle_redirect(int index) {
  if(commands[index].output) {
    int fd = open(commands[index].output, O_CREAT | O_RDWR);
    if(fd < 0) 
      panic("open");
    dup2(fd, 1);
    close(fd);
  }
  if(commands[index].input) {
    int fd = open(commands[index].input, O_CREAT | O_RDWR);
    if(fd < 0) 
      panic("open");
    dup2(fd, 0);
    close(fd);
  }
}

// 执行非管道命令
void exec_simple(int index) {
  // 处理需要文件重定向的情况
  if(commands[index].input || commands[index].output) {
    handle_redirect(index);
  }
  // 装入程序执行
  int error = execvp(commands[index].argv[0], commands[index].argv);
    if(error < 0)
  perror("execvp");
}

// 执行管道命令
void exec_pipe() {
  int fd[2];
  int pid;

  pipe(fd);         // 先创建管道, 再创建进程
  pid = fork();
  if(pid == 0) {
    dup2(fd[1], 1); // 标准输出定向到管道的写端
    close(fd[0]);
    close(fd[1]);
    exec_simple(0);
  }
  wait(NULL);
  dup2(fd[0], 0);   // 标准输入定向到管道的读端
  close(fd[0]);
  close(fd[1]);
  exec_simple(1);
}


void mysys(char *command) {
  parse_commands(command);
  // commands_dump();

  if(command_count == 0)
    return ;
  else if(command_count == 1) {
    // 单独处理内置命令cd、pwd、exit
    if(strcmp(commands[0].argv[0], "cd") == 0 || strcmp(commands[0].argv[0], "pwd") == 0 || strcmp(commands[0].argv[0], "exit") == 0) {
      handle_built_in_command(0);
      return ;
    }
    pid_t pid = fork();
    if(pid == 0) 
      exec_simple(0);
    wait(NULL);
  }
  else if(command_count == 2) {
    pid_t pid = fork();
    if(pid == 0)
      exec_pipe();
    wait(NULL);
  }
}


int main() {
  char command[SIZE];
  int count;

  while(1) {
    write(1, ">", sizeof(">"));
    count = read(0, command, sizeof(command));
    command[count] = 0;
    mysys(command);
  }
  return 0;
}

