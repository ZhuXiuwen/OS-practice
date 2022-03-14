#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
int main(int argc, char *argv[])
{
	int fd, fdb, fc, fcb;
	int w;
	char buf[1000];
	fd = open(argv[1], O_RDONLY);
	fdb = creat(argv[2], O_RDWR);
	if(fd >= 0 && fdb >= 0)
	{
		while(1){
			if(read(fd, buf, sizeof(buf)) > 0){
				w = write(fdb, buf,sizeof(buf));
			}
			else
				break;
		}
	}
	else
		printf("fail to open or create");
	fc = close(fd);
	fcb = close(fdb);
}
