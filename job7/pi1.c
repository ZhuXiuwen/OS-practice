#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

#define NUMBER 200

double PI;
double worker_output;
double master_output;

void *worker(void *arg){
		int i;
		double j;
		worker_output=0;
		for(i=1;i<=NUMBER;i++){
				j=i;
				if(i%2==0)
						worker_output-=1/(2*j-1);
				else
						worker_output+=1/(2*j-1);
		}
		return NULL;
}

void master(){
		int i=NUMBER+1;
		double j;
		master_output=0;
		for(;i<=2*NUMBER;i++){
				j=i;
				if(i%2==0)
						master_output-=1/(2*j-1);
				else
						master_output+=1/(2*j-1);
		}
}

int main(){
		pthread_t worker_tid;
		
		pthread_create(&worker_tid,NULL,worker,NULL);
		master();
		pthread_join(worker_tid,NULL);
		PI=(worker_output+master_output)*4;
		printf("PI: %f\n",PI);
		return 0;
}

