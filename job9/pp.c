#include<stdio.h>
#include<pthread.h>
#define COUNT 100
typedef struct{
	int value;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}sema_t;
void sema_init(sema_t *sema,int value)
{
	sema->value=value;
	pthread_mutex_init(&sema->mutex,NULL);
	pthread_cond_init(&sema->cond,NULL);
}
void sema_wait(sema_t *sema,int status)
{
	pthread_mutex_lock(&sema->mutex);
	while(sema->value!=status)
	{
		pthread_cond_wait(&sema->cond,&sema->mutex);
	}
	pthread_mutex_unlock(&sema->mutex);
}
void sema_signal(sema_t *sema,int status)
{
	pthread_mutex_lock(&sema->mutex);
	sema->value=status;
	pthread_cond_signal(&sema->cond);
	pthread_mutex_unlock(&sema->mutex);
}
sema_t mutex_sema;
void *ping(void *arg)
{
	int i=COUNT;
	while(i--)
	{
		sema_wait(&mutex_sema,1);
		printf("ping\n");
		sema_signal(&mutex_sema,0);
	}
}
void *pong(void *arg)
{
        int i=COUNT;
        while(i--)
        {
                sema_wait(&mutex_sema,0);
                printf("pong\n");
                sema_signal(&mutex_sema,1);
        }
}
 
int main()
{
	pthread_t pong_tid;
	sema_init(&mutex_sema,0);
	pthread_create(&pong_tid,NULL,pong,NULL);
	ping(NULL);
	pthread_join(pong_tid,NULL);
	return 0;
}
