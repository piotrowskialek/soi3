//
// Created by Aleksander Piotrowski on 02.12.2017.
//

#include "../model_headers/Solution.h"


Solution::Solution() {

    mutex = sem_open("/mutex", O_CREAT, S_IRWXU | S_IRWXO, 1);

    for(int i = 0; i < 3; ++i) {
        std::string string = std::to_string(i);
        auto s = "/read"+string;
        read[i] = sem_open(s.c_str(), O_CREAT, S_IRWXU | S_IRWXO, 1);
    }

    for(int i = 0; i < NUMBER_OF_PRODUCERS; ++i) {
        std::string string = std::to_string(i);
        auto s = "/prod"+string;
        producers[i] = sem_open(s.c_str(), O_CREAT, S_IRWXU | S_IRWXO, 0);
    }

    for(int i = 0; i < NUMBER_OF_CONSUMERS; ++i) {
        std::string string = std::to_string(i);
        auto s = "/cons"+string;
        consumers[i] = sem_open(s.c_str(), O_CREAT, S_IRWXU | S_IRWXO, 0);
    }

}

void Solution::solution() {

    time_t t;
    srand((unsigned) time(&t));

    std::cout << "Forks starting now...\n";

    switch (pid = fork()) {
        case 0:
            printf("ProducerA...\n");
            sharedMemory();
//            producerA();
            return;
        case -1:
            printf("Could not fork, errno %d\n", errno);
            break;
        default:
            children[0] = pid;
            break;
    }

    switch (pid = fork()) {
        case 0:
            printf("ProducerB...\n");
            sharedMemory();
//            producerB();
            return;
        case -1:
            printf("Could not fork, errno %d\n", errno);
            break;
        default:
            children[1] = pid;
            break;
    }

    switch (pid = fork()) {
        case 0:
            printf("ConsumerA...\n");
            sharedMemory();
//            consumerA();
            return;
        case -1:
            printf("Could not fork, errno %d\n", errno);
            break;
        default:
            children[2] = pid;
            break;
    }

    switch (pid = fork()) {
        case 0:
            printf("ConsumerB... \n");
            sharedMemory();
//            consumerB();
            return;
        case -1:
            printf("Could not fork, errno %d\n", errno);
            break;
        default:
            children[3] = pid;
            break;
    }

    switch (pid = fork()) {
        case 0:
            printf("ConsumerC... \n");
            sharedMemory();
//            consumerC();
            return;
        case -1:
            printf("Could not fork, errno %d\n", errno);
            break;
        default:
            children[4] = pid;
            break;
    }


//    /* CLEANUP SECTION */
    while (getchar() != 'q');

    kill(children[0], SIGKILL);
    kill(children[1], SIGKILL);
    kill(children[2], SIGKILL);
    kill(children[3], SIGKILL);
    kill(children[4], SIGKILL);

    munmap(memory, sizeOfData);
    munmap(variables, sizeof(Variable));

    sem_unlink("/mutex");
    sem_unlink("/read1");
    sem_unlink("/read2");
    sem_unlink("/read3");

    sem_unlink("/cons1");
    sem_unlink("/cons2");
    sem_unlink("/cons3");
    sem_unlink("/cons4");
    sem_unlink("/cons5");

    sem_unlink("/prod1");
    sem_unlink("/prod2");
    sem_unlink("/prod3");
    sem_unlink("/prod4");
    sem_unlink("/prod5");

    shm_unlink("/shm");
    shm_unlink("/eshm");

}

void Solution::printVariable(Variable &variable) {
    int size, i;
    size = variable.size;
    i = 0;

    printf("SIZE: %d HEAD: %d TAIL: %d\n", size, variable.head, variable.tail);

    while (i < size) {
        printf("%c ", ((Data *) memory + (i + variable.head) % sizeOfBuffer)->val);
        ++i;
    }

    while (size < sizeOfBuffer) {
        printf("-- ");
        ++size;
    }

    printf("\n");
}


void Solution::sharedMemory() {

    int shm;
    int eshm;
    void *tmp;

    shm = shm_open("/shm", O_CREAT | O_RDWR, S_IRWXU | S_IRWXO);

    if (shm == -1) {
        printf("Could not create shared memory, errno: %d\n", errno);
        return;
    }

    ftruncate(shm, sizeOfData);
    memory = mmap(NULL, sizeOfData, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);

    if (memory == MAP_FAILED) {
        printf("Could not map shared memory, errno: %d\n", errno);
        return;
    }

    eshm = shm_open("/eshm", O_CREAT | O_RDWR, S_IRWXU | S_IRWXO);

    if (eshm == -1) {
        printf("Could not create shared memory (eshm), errno: %d\n", errno);
        return;
    }

    ftruncate(eshm, sizeof(Variable));
    tmp = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, eshm, 0);
    if (tmp == MAP_FAILED) {
        printf("Could not map shared memory (vars)), errno: %d \n", errno);
        return;
    }
    variables = (Variable *) tmp;
}


void Solution::updateSize(int diff) {

    int prevSize;
    int newSize;

    prevSize = variables->size;
    variables->size += diff;
    newSize = variables->size;


    //logika biznesowa roberta
    if (prevSize == sizeOfBuffer && newSize < sizeOfBuffer) {
        sem_post(producers[0]);
    }

    if (newSize <= sizeOfBuffer - 2 && prevSize > sizeOfBuffer - 2) {
        sem_post(producers[1]);
    }

    // mozna byloby wprowadzic losowanie ktory jako pierwszy zostanie postawiony na nogi
    if (prevSize == 0 && newSize > 0) {
        sem_post(consumers[0]);
        sem_post(consumers[1]);
        sem_post(consumers[2]);
        return;
    }

}


char Solution::generateRandomCharacter() {
    return (char) (rand() % (90 - 65) + 65);
}

const int Solution::getNUMBER_OF_CONSUMERS() {
    return NUMBER_OF_CONSUMERS;
}

const int Solution::getNUMBER_OF_PRODUCERS() {
    return NUMBER_OF_PRODUCERS;
}

const int Solution::getSizeOfBuffer() {
    return sizeOfBuffer;
}

size_t Solution::getSizeOfData() const {
    return sizeOfData;
}

void Solution::setSizeOfData(size_t sizeOfData) {
    Solution::sizeOfData = sizeOfData;
}

sem_t *Solution::getMutex() const {
    return mutex;
}

void Solution::setMutex(sem_t *mutex) {
    Solution::mutex = mutex;
}

sem_t *const *Solution::getRead() const {
    return read;
}

sem_t *const *Solution::getConsumers() const {
    return consumers;
}

sem_t *const *Solution::getProducers() const {
    return producers;
}

void *Solution::getMemory() const {
    return memory;
}

void Solution::setMemory(void *memory) {
    Solution::memory = memory;
}

Variable *Solution::getVariables() const {
    return variables;
}

void Solution::setVariables(Variable *variables) {
    Solution::variables = variables;
}
