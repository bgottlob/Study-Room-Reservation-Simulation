#ifndef QUEUE_H
#define QUEUE_H

#include "reservation.h"

struct RequestNode {
  Request request;
  struct RequestNode *nextRequest;
};

//RequestQueues will be used to select which Request will be turned into a Reservation first - this queue will be based on priority
typedef struct {
  struct RequestNode *head;
  int size;
} RequestQueue;

RequestQueue createRequestQueue();
void enqueueRequest(RequestQueue *queue, Request request);
Request dequeueRequest(RequestQueue *queue);

struct ReservationNode {
  Reservation reservation;
  struct ReservationNode *nextReservation;
};

/*
  ReservationQueues will be used which reservation is added to the database first
  This queue will not be based on priority because no Reservations in the queue will conflict with each other
*/
typedef struct {
  struct ReservationNode *head;
  struct ReservationNode *rear;
  int size;
} ReservationQueue;

ReservationQueue createReservationQueue();
void enqueueReservation(ReservationQueue *queue, Reservation reservation);
Reservation dequeueReservation(ReservationQueue *queue);

//Searches a reservation queue for a specified reservation - if it exists, function returns 1; if not, function returns 0
int searchForRes(ReservationQueue *queue, Reservation reservation);

//Global queues that are going to be used for synchronization
extern RequestQueue reqQueue;
extern ReservationQueue resQueue;

//Global mutex locks for the queues
extern pthread_mutex_t reqQueueLock, resQueueLock;

#endif
