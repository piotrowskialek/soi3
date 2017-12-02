//
// Created by Aleksander Piotrowski on 02.12.2017.
//

#ifndef SOI_SOLUTION_H
#define SOI_SOLUTION_H

#include <iostream>
#include <algorithm>
#include <string>
#include <semaphore.h>
#include "dto/Data.h"
#include "dto/Variable.h"
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>


class Solution {

private:
    const static int NUMBER_OF_CONSUMERS = 5;
    const static int NUMBER_OF_PRODUCERS = 5;
    const static int sizeOfBuffer = 10;

    size_t sizeOfData = sizeOfBuffer * sizeof(Data);

    sem_t* mutex;
    sem_t* read[3];
    sem_t* consumers[NUMBER_OF_CONSUMERS];
    sem_t* producers[NUMBER_OF_PRODUCERS];

    pid_t pid;
    pid_t children[5];

    void* memory;

    Variable* variables;

public:
    Solution();

    void solution();

    void sharedMemory();

    void updateSize(int diff);

    void printVariable(Variable& variable);

    char generateRandomCharacter();

    static const int getNUMBER_OF_CONSUMERS();

    static const int getNUMBER_OF_PRODUCERS();

    static const int getSizeOfBuffer();

    size_t getSizeOfData() const;

    void setSizeOfData(size_t sizeOfData);

    sem_t *getMutex() const;

    void setMutex(sem_t *mutex);

    sem_t *const *getRead() const;

    sem_t *const *getConsumers() const;

    sem_t *const *getProducers() const;

    void *getMemory() const;

    void setMemory(void *memory);

    Variable *getVariables() const;

    void setVariables(Variable *variables);
};

#endif //SOI_SOLUTION_H
