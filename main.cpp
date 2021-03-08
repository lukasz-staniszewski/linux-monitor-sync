#include <iostream>
#include "MonitorQueue.h"

struct ProducentArgStruct{
    MonitorQueue* queue;
    char name;
    int color;
};

struct ConsumentArgStruct{
    MonitorQueue* queueA;
    MonitorQueue* queueB;
    char name;
    int color;

};

void* producent(void* arg){
    ProducentArgStruct* argStruct = (ProducentArgStruct*) arg;
    int k=1;
    if (argStruct->name == 'B')  k*=100;
    for(int i=1; i<=100;i++){
        argStruct->queue->addValue(i*k);
        std::cout<<"\033[1;"<<argStruct->color<<"mPRODUCENT "<<argStruct->name<<" APPENDED: "<<i*k<<" | QUEUE_"<<argStruct->name<<": \033[0m";
        argStruct->queue->printQueue();
        sleep((rand() % 3));
    }
    return NULL;
}

void* consument (void* arg){
    int value;
    ConsumentArgStruct* argStruct = (ConsumentArgStruct*) arg;
    for(int i=0; i<100;i++){
        if(i%2 == 0){
            value = argStruct->queueA->popValue();
            std::cout<<"\033[1;"<<argStruct->color<<"mCONSUMENT "<<argStruct->name<<" POPPED FROM QUEUE A: "<<value<<" | QUEUE_A: \033[0m";
            argStruct->queueA->printQueue();
        }
        else{
            value = argStruct->queueB->popValue();
            std::cout<<"\033[1;"<<argStruct->color<<"mCONSUMENT "<<argStruct->name<<" POPPED FROM QUEUE B: "<<value<<" | QUEUE_B: \033[0m";
            argStruct->queueB->printQueue();
        } 
        sleep((rand() % 3));
    }
    return NULL;
}

void allocateQueues(MonitorQueue *&queueA, MonitorQueue *&queueB){
    queueA = new MonitorQueue();
    queueB = new MonitorQueue();
    std::cout<<"QUEUES ALLOCATED!"<<std::endl;
}

void deallocateQueues(MonitorQueue *&queueA, MonitorQueue *&queueB){
    delete queueA;
    delete queueB;
    std::cout<<"QUEUES DEALLOCATED!"<<std::endl;
}

void testOnlyProducents(ProducentArgStruct *&argProdA, ProducentArgStruct *&argProdB){
    std::cout<<"TEST WITH 2 ProducentS AND 0 CONSUMENT: "<<std::endl<<std::endl;
    pthread_t t0, t1;
    void* argument;
    pthread_create(&t0, NULL, producent, argProdA);
    pthread_create(&t1, NULL, producent, argProdB);

    pthread_join(t0, &argument);
    pthread_join(t1, &argument);
}

void testOnlyConsuments(ConsumentArgStruct *&argConsA, ConsumentArgStruct *&argConsB){
    std::cout<<"TEST WITH 0 ProducentS AND 2 CONSUMENTS: "<<std::endl<<std::endl;

    void* argument;    
    pthread_t t2, t3;
    pthread_create(&t2, NULL, consument, argConsA);
    pthread_create(&t3, NULL, consument, argConsB);
    
    pthread_join(t2, &argument);
    pthread_join(t3, &argument);
}

void testOneConsument(ProducentArgStruct *&prodArgA, ProducentArgStruct *&prodArgB, ConsumentArgStruct *&consArgA){
    std::cout<<"TEST WITH 2 ProducentS AND 1 CONSUMENT: "<<std::endl<<std::endl;
    void* argument;
    pthread_t t0, t1, t2;
    pthread_create(&t0, NULL, producent, prodArgA);
    pthread_create(&t1, NULL, producent, prodArgB);
    pthread_create(&t2, NULL, consument, consArgA);

    pthread_join(t0, &argument);
    pthread_join(t1, &argument);
    pthread_join(t2, &argument);
}

void testConsumentsProducents(ProducentArgStruct *&prodArgA, ProducentArgStruct *&prodArgB, ConsumentArgStruct *&consArgA, ConsumentArgStruct *&consArgB){
    std::cout<<"TEST WITH 2 ProducentS AND 2 CONSUMENTS:"<<std::endl<<std::endl;
    void *argument;
    pthread_t t0, t1, t2, t3;
    pthread_create(&t0, NULL, producent, prodArgA);
    pthread_create(&t1, NULL, producent, prodArgB);
    pthread_create(&t2, NULL, consument, consArgA);
    pthread_create(&t3, NULL, consument, consArgB);

    pthread_join(t0, &argument);
    pthread_join(t1, &argument);
    pthread_join(t2, &argument);
    pthread_join(t3, &argument);
}

void prepareArguments(MonitorQueue *&queueA, MonitorQueue *&queueB, ProducentArgStruct *&prodA, ProducentArgStruct *&prodB, ConsumentArgStruct *&consA, ConsumentArgStruct *&consB){
    prodA = new ProducentArgStruct;
    prodB = new ProducentArgStruct;
    consA = new ConsumentArgStruct;
    consB = new ConsumentArgStruct;
    prodA->queue = queueA;
    prodB->queue = queueB;
    consA->queueA = queueA;
    consB->queueA = queueA;
    consA->queueB = queueB;
    consB->queueB = queueB;
    prodA->name = consA->name = 'A';
    prodB->name = consB->name = 'B';
    prodA->color = 31;
    prodB->color = 32;
    consA->color = 34;
    consB->color = 35;
}

void deallocateArguments(ProducentArgStruct *&prodA, ProducentArgStruct *&prodB, ConsumentArgStruct *&consA, ConsumentArgStruct *&consB){
    delete prodA;
    delete prodB;
    delete consA;
    delete consB;
}

int main(){
    MonitorQueue* queueA;
    MonitorQueue* queueB;
    ProducentArgStruct *prodA, *prodB; 
    ConsumentArgStruct *consA, *consB; 
    allocateQueues(queueA, queueB);
    prepareArguments(queueA, queueB, prodA, prodB, consA, consB);
    srand(time(NULL));


    testConsumentsProducents(prodA, prodB, consA, consB);
    // testOneConsument(prodA, prodB, consA);
    // testOnlyConsuments(consA, consB);
    // testOnlyProducents(prodA, prodB);
    
    deallocateQueues(queueA, queueB);
    deallocateArguments(prodA, prodB, consA, consB);
    return 0;
}
