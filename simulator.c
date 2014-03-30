#include <stdio.h>
#include "queue.h"
#include <string.h>
#include <unistd.h>
#include "reservation.h"
#include <pthread.h>
#include <stdlib.h>

int main() {

  //Initializing the global queues for use by child threads
  reqQueue = createRequestQueue();
  resQueue = createReservationQueue();

  /*Request req = createRequest(5, 12, 13, 12, createUser(867530, "smittywerbenjagermanjensen@gmail.com", 1));
  Reservation res = findReservation(req);
  makeReservation(res);*/

  pthread_t t1, t2, t3, t4, t5;

  /*Request *r1 = (Request *)malloc(sizeof(Request));
  *r1 = createRequest(8, 12, 13, 12, createUser(12345, "faculty1@gmail.com", 2));

  Request *r2 = (Request *)malloc(sizeof(Request));
  *r2 = createRequest(8, 12, 13, 12, createUser(23456, "student1@gmail.com", 1));

  Request *r3 = (Request *)malloc(sizeof(Request));
  *r3 = createRequest(8, 12, 13, 12, createUser(34567, "faculty2@gmail.com", 2));

  Request *r4 = (Request *)malloc(sizeof(Request));
  *r4 = createRequest(8, 12, 13, 12, createUser(45678, "student2@gmail.com", 1));

  Reservation *adminRes = (Reservation *)malloc(sizeof(Reservation));
  *adminRes = createReservation(413, 8, 13, 14, createUser(37828, "admin@tcnj.edu", 0));

  pthread_create(&t1, NULL, startRequest, r1);
  pthread_create(&t2, NULL, startRequest, r2);
  pthread_create(&t3, NULL, startRequest, r3);
  pthread_create(&t4, NULL, startRequest, r4);

  sleep(2);

  pthread_create(&t5, NULL, processAdminReservation, adminRes);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);
  pthread_join(t5, NULL);
  r1 = NULL;
  free(r1);
  r2 = NULL;
  free(r2);
  r3 = NULL;
  free(r3);
  r4 = NULL;
  free(r4);
  adminRes = NULL;
  free(adminRes);*/

  /*Reservation res1 = createReservation(311, 8, 13, 14, createUser(23456, "student1@gmail.com", 2));
  enqueueReservation(&resQueue, res1);
  Reservation res2 = createReservation(314, 8, 16, 17, createUser(23456, "student1@gmail.com", 1));
  enqueueReservation(&resQueue, res2);

  Reservation rSearch = createReservation(200, 8, 13, 14, createUser(23456, "student1@gmail.com", 1));

  printf("FoundRes: %d\n", searchForRes(&resQueue, res2));*/

  /*Request deqreq = dequeueRequest(&reqQueue);
  printf("Dequeued req: \nday: %d\nstartTime: %d\nendTime: %d\nseats: %d\nuserEmail: %s\n", deqreq.day, deqreq.startTime, deqreq.endTime, deqreq.seatsNeeded, deqreq.user.email);*/

  /*Reservation deqres = dequeueReservation(&resQueue);
  printf("Dequeued res: \nroom: %d\nday: %d\nstartTime: %d\nendTime: %d\nuserEmail: %s\n", deqres.roomNum, deqres.day, deqres.startTime, deqres.endTime, deqres.user.email);*/


  Request *r1 = (Request *)malloc(sizeof(Request));
  *r1 = createRequest(8, 12, 13, 12, createUser(12345, "faculty1@gmail.com", 2));

  pthread_create(&t1, NULL, startRequest, r1);

  sleep(1);

  Reservation *adminRes = (Reservation *)malloc(sizeof(Reservation));
  *adminRes = createReservation(220, 8, 13, 14, createUser(37828, "admin@tcnj.edu", 0));

  pthread_create(&t5, NULL, processAdminReservation, adminRes);

  pthread_join(t1, NULL);
  pthread_join(t5, NULL);

  cancelReservation(createReservation(220, 8, 13, 14, createUser(37828, "admin@tcnj.edu", 0)));

  return 0;
}
