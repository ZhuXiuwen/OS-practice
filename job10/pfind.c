#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdlib.h>
 
#define WORKER_NUMBER 4
#define CAPACITY 100
int in=0,out=0;
 
//任务队列
struct Task{
	int is_end;
	char path[128];
	char string[128];
}buffer[CAPACITY];
 
//取任务
struct Task get_item()
{
	struct Task item;
	item=buffer[out];
	out=(out+1)%CAPACITY;
	return item;
}
 
//放任务
void put_item(struct Task item)
{
	buffer[in]=item;
	in=(in+1)%CAPACITY;
}
 
//信号量
typedef struct{
	int value;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}sema_t;
 
sema_t mutex_sema;//互斥信号量
sema_t empty_buffer_sema;//空信号量
sema_t full_buffer_sema;//满信号量
 
//信号量初始化
void sema_init(sema_t *sema,int value)
{
	sema->value=value;
	pthread_mutex_init(&sema->mutex,NULL);
	pthread_cond_init(&sema->cond,NULL);
}
 
//p操作
void sema_wait(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	while(sema->value<=0)
	{
		pthread_cond_wait(&sema->cond,&sema->mutex);
	}
	sema->value--;
	pthread_mutex_unlock(&sema->mutex);
}
 
//v操作
void sema_signal(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	++sema->value;
	pthread_cond_signal(&sema->cond);
	pthread_mutex_unlock(&sema->mutex);
}

//找子串函数
void find_file(char *path,char *target)
{
        //printf("enter find file fun\n");
        FILE *file=fopen(path,"r");
        char line[256];
        while(fgets(line,sizeof(line),file)){
                if(strstr(line,target))
                        printf("%s:%s",path,line);
        }
        fclose(file);
}
 
//消费者
void *worker_entry(void *arg)
{
	struct Task worker;
	while(1)
	{
		sema_wait(&full_buffer_sema);
		sema_wait(&mutex_sema);
 
		worker=get_item();
		if(worker.is_end)
		{
			break;
		}
		find_file(worker.path,worker.string);
 
		sema_signal(&mutex_sema);
		sema_signal(&empty_buffer_sema);
	}
	if(worker.is_end)
	{
	 	sema_signal(&mutex_sema);
 	        sema_signal(&empty_buffer_sema);
	}
}
 
 
//生产者
void find_dir(char *path,char *target)
{
	//printf("enter find dir fun\n");
	DIR *dir=opendir(path);//open the dir
	struct dirent *entry;
	while(entry=readdir(dir))//read the dir & point to next obj
	{
		if(strcmp(entry->d_name,".")==0)
			continue;
		if(strcmp(entry->d_name,"..")==0)
			continue;
		char base[80]={0};
		strcpy(base,path);
		strcat(base,"/");
		strcat(base,entry->d_name);//fill the path
		//printf("base %s\n",base);
		if(entry->d_type==DT_DIR)//file type is dir
		{
			//printf("dir %s\n",entry->d_name);
			find_dir(base,target);
		}
        //将叶子路径放入任务队列
		//put the leaf path into task list
		else if(entry->d_type==DT_REG)
		{
			//printf("file %s\n",entry->d_name);
			sema_wait(&empty_buffer_sema);
			sema_wait(&mutex_sema);
			
			struct Task item;
			item.is_end=0;
			strcpy(item.path,base);
			strcpy(item.string,target);
			put_item(item);
 
			sema_signal(&mutex_sema);
			sema_signal(&full_buffer_sema);
 
			
		}
	}
	closedir(dir);
}
int main(int argc,char *argv[])
{
	char *path=argv[1];
	char *target=argv[2];
	if(argc!=3)
	{
		puts("usage: sfind file string");
		return 0;
	}
	struct stat info;
	stat(path,&info);
//如果是目录，就是生产者消费者问题，
	if(S_ISDIR(info.st_mode))
	{
    //初始化信号量
		pthread_t consumer_tid[WORKER_NUMBER];
		sema_init(&mutex_sema,1);
		sema_init(&empty_buffer_sema,CAPACITY-1);
		sema_init(&full_buffer_sema,0);
		
		find_dir(path,target);
	    
        //开子线程
		for(int i=0;i<WORKER_NUMBER;i++)
        {
 
                    pthread_create(&consumer_tid[i],NULL,worker_entry,NULL);
        } 
 
        //结束工作
		for(int i=0;i<WORKER_NUMBER;i++)
        {
			        sema_wait(&empty_buffer_sema);
               		sema_wait(&mutex_sema);
                	struct Task item;
                	item.is_end=1;
                	put_item(item);
                	sema_signal(&mutex_sema);
                	sema_signal(&full_buffer_sema);
         }
        //让没运行完的运行完
		for(int i=0;i<WORKER_NUMBER;i++)
        {
 
                    pthread_join(consumer_tid[i],NULL);
        }
		return 0;
 
	}
    //如果是文件，就直接找子串
	else find_file(path,target);
	return 0;
}
