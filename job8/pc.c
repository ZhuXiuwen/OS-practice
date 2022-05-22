#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

#define CAPACITY 4

char buffer1[4];
char buffer2[4];

int crea; //生产者 
int comp1;//计算者 
int comp2;//计算者 
int cons; //消费者 


int buffer1_is_empty(){
    return crea==comp1;
}

int buffer1_is_full(){
    return (crea+1)%CAPACITY==comp1;
}

int buffer2_is_empty(){
    return comp2==cons;
}

int buffer2_is_full(){
    return (cons+1)%CAPACITY==comp2;
}

int get_item1(){
    int item;

    item=buffer1[comp1];
    comp1=(comp1+1)%CAPACITY;
    return item;
}

int get_item2(){
    int item;

    item=buffer2[cons];
    cons=(cons+1)%CAPACITY;
    return item;
}

int put_item1(int item){
        buffer1[crea]=item;
        crea=(crea+1)%CAPACITY;
}

int put_item2(int item){
        buffer2[comp2]=item;
        comp2=(comp2+1)%CAPACITY;
}

pthread_mutex_t mutex1;
pthread_cond_t wait_empty_buffer1;
pthread_cond_t wait_full_buffer1;

pthread_mutex_t mutex2;
pthread_cond_t wait_empty_buffer2;
pthread_cond_t wait_full_buffer2;

#define ITEM_COUNT  (CAPACITY *2)

void *consumer(void *arg){
    int i;
    int item;

    for(i=0;i<ITEM_COUNT;i++){
            pthread_mutex_lock(&mutex2);
            while(buffer2_is_empty())
                    pthread_cond_wait(&wait_full_buffer2,&mutex2);

            item=get_item2();
            printf("                  consume item:%c\n",item);

            pthread_cond_signal(&wait_empty_buffer2);
            pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}

void *computer(void *arg){
    int i;
    int item;

    for(i=0;i<ITEM_COUNT;i++){
            pthread_mutex_lock(&mutex1);
            while(buffer1_is_empty())
                    pthread_cond_wait(&wait_full_buffer1,&mutex1);
            item=get_item1();

            printf("          computer get item:%c\n",item);
            item-=32;

            pthread_cond_signal(&wait_empty_buffer1);
            pthread_mutex_unlock(&mutex1);


            pthread_mutex_lock(&mutex2);
            while(buffer2_is_full())
                    pthread_cond_wait(&wait_empty_buffer2,&mutex2);

            put_item2(item);
            printf("          computer put item:%c\n",item);

            pthread_cond_signal(&wait_full_buffer2);
            pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}

void *create(void *arg){
        int i;
        int item;

        for(i=0;i<ITEM_COUNT;i++){
                pthread_mutex_lock(&mutex1);
                while(buffer1_is_full())
                        pthread_cond_wait(&wait_empty_buffer1,&mutex1);
                item='a'+i;
                put_item1(item);
                printf("create item:%c\n",item);

                pthread_cond_signal(&wait_full_buffer1);
                pthread_mutex_unlock(&mutex1);
        }
        return NULL;
}

int main(){
        pthread_t consumer_tid;
        pthread_t computer_tid;

        pthread_mutex_init(&mutex1,NULL);
        pthread_mutex_init(&mutex2,NULL);

        pthread_cond_init(&wait_empty_buffer1,NULL);
        pthread_cond_init(&wait_full_buffer1,NULL);
        pthread_cond_init(&wait_empty_buffer2,NULL);
        pthread_cond_init(&wait_full_buffer2,NULL);

        pthread_create(&consumer_tid,NULL,consumer,NULL);
        pthread_create(&computer_tid,NULL,computer,NULL);
        create(NULL);

        pthread_join(consumer_tid,NULL);
        pthread_join(computer_tid,NULL);

        pthread_mutex_destroy(&mutex1);
        pthread_mutex_destroy(&mutex2);
        return 0;
}
