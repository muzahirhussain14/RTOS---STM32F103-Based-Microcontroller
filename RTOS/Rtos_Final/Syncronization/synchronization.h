#ifndef SYNCHRONIZATION_H_
#define SYNCHRONIZATION_H_

extern const int SEM_LIMIT;

typedef struct
{
  int mut_var;
} Mutex;

void mutex_init(Mutex* obj);
void mutex_lock(Mutex* obj);
void mutex_unlock(Mutex* obj);



typedef struct
{
  int sem_var;
} Semaphore;


int sem_init(Semaphore* sem_obj, int value);
void sem_wait(Semaphore* sem_obj);
void sem_signal(Semaphore* sem_obj);
int sem_getValue(Semaphore* sem_obj);

#endif