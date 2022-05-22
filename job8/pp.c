#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
int p=1;
#define COUNT 10
pthread_mutex_t mutex;
pthread_cond_t wait_ping;
pthread_cond_t wait_pong;
 
void *ping(void *arg)
{
	int i=COUNT;
	while(i--){
 
		pthread_mutex_lock(&mutex);
		while(p==0)
			pthread_cond_wait(&wait_pong,&mutex);
		printf("ping\n");
		p=0;
		pthread_cond_signal(&wait_ping);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
void *pong(void *arg)
{
	int i=COUNT;
	while(i--)
	{
		
		pthread_mutex_lock(&mutex);
        	while(p==1)
        	        pthread_cond_wait(&wait_ping,&mutex);
        	printf("pong\n");
        	p=1;
        	pthread_cond_signal(&wait_pong);
        	pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
int main()
{
	pthread_t pong_tid;
 
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&wait_ping,NULL);
	pthread_cond_init(&wait_pong,NULL);
	
	pthread_create(&pong_tid,NULL,pong,NULL);
	ping(NULL);
	pthread_join(pong_tid,NULL);
	return 0;
}
