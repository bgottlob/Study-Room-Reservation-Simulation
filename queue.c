#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

RequestQueue createRequestQueue()
{
  RequestQueue queue;

  queue.size = 0;
  queue.head = NULL;

  return queue;
}

void enqueueRequest(RequestQueue *queue, Request request)
{
  struct RequestNode *newNode = malloc(sizeof(struct RequestNode));
  newNode->request = request;
  newNode->nextRequest = NULL;

  if (queue->size == 0)
    queue->head = newNode;
  else
  {
    //The request queue 
  }

  queue->size++;
}

Request dequeueRequest(RequestQueue *queue)
{
  if (queue->size == 0)
  {
    printf("There's nothing in this queue, exiting");
    exit(0);
  }
  else
  {
    struct RequestNode removed = *(queue->head);
    queue->head = queue->head->nextRequest;
    queue->size--;

    return removed.request;
  }
}

ReservationQueue createReservationQueue()
{
  ReservationQueue queue;

  queue.size = 0;
  queue.head = NULL;

  return queue;
}

void enqueueReservation(ReservationQueue *queue, Request request);
Reservation dequeueReservation(ReservationQueue *queue);
