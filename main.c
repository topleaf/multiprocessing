// July 11,2021
// example for thread and process sync, using semaphore
// threads can access one global variable
// processes have to use shared memory to access the same resource


#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
// for share memory use
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
// for share memory use


#define SEMAPHORE_MAX_NUM 2

//
//void *work_thread(void *name){
//
//    printf("\nenter thread %s",(char *)name);
//    for (int i = 0; i < 7; i++) {
//        sem_wait(&mutex);
//        printf("\nHello,%s,i=%d,count=%d ",(char *)name,i,count);
//        count++;
//        sem_post(&mutex);
//        sleep(1);
//    }
//
//    //sleep(1);
//    printf("\nexit thread %s",(char *) name);
////    sem_post(&mutex);
//}
// To execute C, please define "int main()"
//
//int main() {
//    sem_init(&mutex,0,SEMAPHORE_MAX_NUM);
//    pthread_t t1,t2;
//    char name1[]="thread 1";
//    char name2[]="thread 2";
//
//    pthread_create(&t1,NULL,work_thread,(void *)name1);
//    sleep(1);
//    pthread_create(&t2,NULL,work_thread,(void *)name2);
//    pthread_join(t1,NULL);
//    pthread_join(t2,NULL);
//    sem_destroy(&mutex);
//    printf("\nend of program");
//    return 0;
//}



int main(){
    printf("\nstart");
    pid_t child_id;
    int shmid,*pId,mutexshmId;
    void *shared_counter,*shared_mutex;
    struct shmid_ds buf;
    sem_t *pMutex;
    int count=0;
    int re;


    shmid = shmget( 1234, sizeof(int), 0666 | IPC_CREAT);
    mutexshmId = shmget( 2345,sizeof(sem_t),0666|IPC_CREAT);
    printf("\n ================== created shmid=%d ,mutexshmId = %d=======================\n",shmid,mutexshmId);

    if(shmid == -1||mutexshmId==-1)
    {
        printf("shmget failed\n");
        exit(EXIT_FAILURE);
    }   //将共享内存连接到当前进程的地址空间
    shared_counter = shmat(shmid, (void*)0, 0);
    if(shared_counter == (void*)-1)
    {
        printf("shmat failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Memory attached at %X\n", (unsigned int)shared_counter);    //设置共享内存
    shared_mutex = shmat(mutexshmId, (void*)0, 0);
    if(shared_mutex == (void*)-1)
    {
        printf("shmat shared_mutex failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Memory attached at %X\n", (unsigned int)shared_mutex);    //设置共享内存
    pMutex = (sem_t *) shared_mutex;  // mutex is now setup in shared memory

    re = sem_init(pMutex,1,SEMAPHORE_MAX_NUM);
    if (re==-1)
    {
        printf("sem_init error");
    }

    sem_wait(pMutex);

    pId = (int *)shared_counter;
    *pId = 0;
    printf("shared_counter=%d",*pId);
    sem_post(pMutex);

    child_id = fork();
    if (child_id){
        printf("\nparent is running,pid=%d",getpid());
    }
    else{
        printf("\nchild is running,pid=%d,ppid=%d",getpid(),getppid());
    }
    for (int i=0;i<7;i++){
        sem_wait(pMutex);
        count++;
        *pId=*pId+1;
        printf("\nhello %d,i=%d,count=%d,shared_count=%d",getpid(),i,count,*pId);

        sem_post(pMutex);
        sleep(2);
    }
    printf("\nend of process %d",getpid());
    sem_destroy(pMutex);

    if (shmdt(shared_mutex)==-1){
        printf("\n shmdt failed for shared_mutex in process %d",getpid());
        return -1;
    }
    else
        printf("\nshmdt succeeded in process %d",getpid());

    if (shmctl(mutexshmId,IPC_STAT, &buf) == -1)  // get status of this shared memory
    {
        printf("\nshmctl(IPC_STAT) failed for process %d\n",getpid());
        return -1;
    }
    else if( buf.shm_nattch == 0){  // this process is the last running process which uses this share memory
        if (shmctl(mutexshmId, IPC_RMID, 0) == -1){
            printf("\nshmctl(IPC_RMID) failed for process %d\n",getpid());
            return -1;
        }  //删除共享内存
        else
            printf("\n succeeded in removing mutex shared memory in process %d\n",getpid());
    }
    else
        printf("\n I am process %d,not the last process to use the shared memory, so leave it alone\n",getpid());



    if (shmdt(shared_counter)==-1){
        printf("\nshmdt failed!! in process %d",getpid());
        return -1;
    }
    else
        printf("\nshmdt succeed in process %d.",getpid());
    if (shmctl(shmid,IPC_STAT, &buf) == -1)  // get status of this shared memory
    {
        printf("\nshmctl(IPC_STAT) failed for process %d\n",getpid());
        return -1;
    }
    else if( buf.shm_nattch == 0){  // this process is the last running process which uses this share memory
        if (shmctl(shmid, IPC_RMID, 0) == -1){
            printf("\nshmctl(IPC_RMID) failed for process %d\n",getpid());
            return -1;
        }  //删除共享内存
        else
            printf("\n succeeded in removing shared memory in process %d\n",getpid());
    }
    else
        printf("\n I am process %d,not the last process to use the shared memory, so leave it alone\n",getpid());

    return 0;
}