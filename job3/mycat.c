#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
int main(int argc, char *argv[])
{
	int fd;
	char buf[100]; 
	int fr;
	fd = open(argv[1], O_RDONLY);
	if(fd >= 0)
	{
		while(1){
			fr = read(fd, buf, sizeof(buf));	
			if(fr > 0)
				printf("%s", buf);
			else
				break;
		}
	}
	else
		printf("fail to open\n");
	int fc = close(fd);
}

