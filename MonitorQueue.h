#ifndef MONITORQUEUE_H
#define MONITORQUEUE_H
#include "Monitor.h"
#include <iostream>
#define MAX_QUEUE_SIZE 7

class MonitorQueue: public Monitor{
    Condition FULL, EMPTY;
    int* queue;
    int queueSize;
    
    public:
    
    MonitorQueue(){
        this->queue = new int [MAX_QUEUE_SIZE];
        clearQueue();
    }

    ~MonitorQueue(){
        delete [] this->queue;
    }

    void clearQueue(){
        this->queueSize = 0;
        for(int i=0; i<MAX_QUEUE_SIZE; i++){
            this->queue[i] = NULL;
        }
    }

    void addValue(int value){
        enter();
        if(this->queueSize == MAX_QUEUE_SIZE) 
        {
            wait(FULL);
        }
        this->queue[this->queueSize] = value;
        this->queueSize++;
        if(this->queueSize == 1){
            signal(EMPTY);
        }
        leave();
    }

    int popValue(){
        int value;
        enter();
        if(this->queueSize == 0){
            wait(EMPTY);
        }
        value = this->queue[0];
        for(int i=0;i<this->queueSize-1; i++){
            this->queue[i] = this->queue[i+1];
        }
        this->queue[queueSize-1] = NULL;
        this->queueSize--;
        if(this->queueSize == MAX_QUEUE_SIZE -1){
            signal(FULL);
        }
        leave();
        return value;
    }

    void printQueue(){
        std::cout<<"|";
        for(int i=0;i<this->queueSize;i++){
            std::cout<<this->queue[i]<<"->";
        }
        std::cout<<"|"<<std::endl;
    }

};
#endif //MONITORQUEUE_H