#include <stdio.h>
#include "queue.h"
#include <string.h>
#include <unistd.h>

int main() {

  /*RequestQueue queue = createRequestQueue();
  Request request = createRequest(1, 3, 4, createUser(432342, "dsflkjsdl", admin));
  enqueueRequest(&queue, request);

  Request newreq = dequeueRequest(&queue);

  printf("Email: %s\n", newreq.user.email);*/

  /*Request request1 = createRequest(1, 3, 4, createUser(432342, "dsflkjsdl", admin));
  sleep(2);
  Request request2 = createRequest(1, 3, 4, createUser(53436, "hello", student));

  printf("Request1 time: %ld\n", request1.timeCreated);
  printf("Request2 time: %ld\n", request2.timeCreated);*/

  /*RequestQueue queue;

  Request request1 = createRequest(1, 3, 4, createUser(432342, "number3", faculty));
  sleep(1);
  Request request2 = createRequest(1, 3, 4, createUser(432342, "number1", student));
  sleep(1);
  Request request3 = createRequest(1, 3, 4, createUser(432342, "number4", faculty));
  sleep(1);
  Request request4 = createRequest(1, 3, 4, createUser(432342, "number2", student));
  sleep(1);
  Request request5 = createRequest(1, 3, 4, createUser(432342, "number5", faculty));

  printf("Queue.size: %d\n", queue.size);
  enqueueRequest(&queue, request1);
  printf("Queue.size: %d\n", queue.size);
  enqueueRequest(&queue, request2);
  printf("Queue.size: %d\n", queue.size);
  enqueueRequest(&queue, request3);
  printf("Queue.size: %d\n", queue.size);
  enqueueRequest(&queue, request4);
  printf("Queue.size: %d\n", queue.size);
  enqueueRequest(&queue, request5);
  printf("Queue.size: %d\n", queue.size);

  Request req1 = dequeueRequest(&queue);
  Request req2 = dequeueRequest(&queue);
  Request req3 = dequeueRequest(&queue);
  Request req4 = dequeueRequest(&queue);
  Request req5 = dequeueRequest(&queue);

  printf("%s\n", req1.user.email);
  printf("%s\n", req2.user.email);
  printf("%s\n", req3.user.email);
  printf("%s\n", req4.user.email);
  printf("%s\n", req5.user.email);*/

  /*Reservation r1 = createReservation(210, 3, 4, 5, createUser(34534, "number1", faculty));
  sleep(1);
  Reservation r2 = createReservation(210, 3, 4, 5, createUser(34534, "number2", student));
  sleep(1);
  Reservation r3 = createReservation(210, 3, 4, 5, createUser(34534, "number3", faculty));
  sleep(1);
  Reservation r4 = createReservation(210, 3, 4, 5, createUser(34534, "number4", student));
  sleep(1);

  ReservationQueue resQueue = createReservationQueue();
  enqueueReservation(&resQueue, r1);
  enqueueReservation(&resQueue, r2);
  enqueueReservation(&resQueue, r3);
  enqueueReservation(&resQueue, r4);

  Reservation rs1 = dequeueReservation(&resQueue);
  Reservation rs2 = dequeueReservation(&resQueue);
  Reservation rs3 = dequeueReservation(&resQueue);
  Reservation rs4 = dequeueReservation(&resQueue);

  printf("%s\n", rs1.user.email);
  printf("%s\n", rs2.user.email);
  printf("%s\n", rs3.user.email);
  printf("%s\n", rs4.user.email);*/

  printf("%s\n", dbFilename);

  return 0;
}
