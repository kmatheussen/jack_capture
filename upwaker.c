#include "upwaker.h"
#include <stdlib.h>

upwaker_t *create_upwaker(void) {
  upwaker_t *upwaker = calloc(1,sizeof(upwaker_t));
  upwaker->please_wakeup = 0;
  pthread_mutex_init(&upwaker->mutex,NULL);
  pthread_cond_init(&upwaker->cond,NULL);
  return upwaker;
}

void upwaker_sleep(upwaker_t *upwaker) {
  if(upwaker->please_wakeup==0)
    pthread_cond_wait(&upwaker->cond,&upwaker->mutex);

  upwaker->please_wakeup = 0;
}

void upwaker_wake_up(upwaker_t *upwaker) {
  upwaker->please_wakeup = 1;
  pthread_cond_broadcast(&upwaker->cond); // Must call, in case the other thread had finished the 'if' test before this function was called.
                                          // (i.e. it is not safe to trylock the mutex to check wether to call pthread_cond_broadcast).
}
