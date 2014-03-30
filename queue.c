#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

//Redefinition of global queues to allow global usage
RequestQueue reqQueue;
ReservationQueue resQueue;

//Initializers for global mutex locks
pthread_mutex_t reqQueueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t resQueueLock = PTHREAD_MUTEX_INITIALIZER;

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
    /*
      The first priority value to be checked is whether a user is a student or faculty:
      students have priority over faculty

      The second priority value to be checked is the time that a request was created:
      the request that has the lower timeCreated (earlier timeCreated) will take priority
    */

    struct RequestNode *prev = queue->head;
    struct RequestNode *current = queue->head;

    int stop = 0;

    while (current != NULL && stop == 0)
    {
      if (newNode->request.user.type > current->request.user.type || (newNode->request.user.type == current->request.user.type && newNode->request.timeCreated >= current->request.timeCreated))
      {
        //Go to the next node
        if (current != queue->head)
          prev = prev->nextRequest;

        current = current->nextRequest;
      }
      else
        stop = 1;
    }

    if (current != queue->head)
    {
      newNode->nextRequest = current;
      prev->nextRequest = newNode;
    }
    else
    {
      //The newNode is the head of the queue
      newNode->nextRequest = current;
      queue->head = newNode;
    }

  }

  //Set the newNode pointer to NULL so it doesn't keep pointing to head
  newNode = NULL;
  free(newNode);

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
  queue.rear = NULL;

  return queue;
}

void enqueueReservation(ReservationQueue *queue, Reservation reservation)
{
  struct ReservationNode *newNode = malloc(sizeof(struct ReservationNode));
  newNode->reservation = reservation;
  newNode->nextReservation = NULL;

  if (queue->size == 0)
  {
    queue->head = newNode;
    queue->rear = queue->head;
  }
  else
  {
    queue->rear->nextReservation = newNode;
    queue->rear = queue->rear->nextReservation;
  }

  //Set the newNode pointer to NULL so it doesn't keep pointing to head
  newNode = NULL;
  free(newNode);

  queue->size++;
}

Reservation dequeueReservation(ReservationQueue *queue)
{
  if (queue->size == 0)
  {
    printf("There's nothing in this queue, exiting");
    exit(0);
  }
  else
  {
    struct ReservationNode removed = *(queue->head);
    queue->head = queue->head->nextReservation;
    queue->size--;

    return removed.reservation;
  }
}
