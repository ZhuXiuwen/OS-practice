#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
int main(int argc, char *argv[]){
	int fd;
	if(argc == 1)
		fd = 0 ;
	else
		fd = open(argv[1], O_CREAT | O_RDWR, 0666);
	while(1){
		char buf[512];
		int count;
		count = read(fd, buf, sizeof(buf));
		if(count < 0){
			perror("read");
			exit(EXIT_FAILURE);
		}
		if(count == 0)
			break;
		write(1, buf, count);
	}
	close(fd);
	return 0;
}
