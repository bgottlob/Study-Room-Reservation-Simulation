typedef struct {
  Request request;
  Request *nextRequest;
} RequestNode;

//RequestQueues will be used to select which Request will be turned into a Reservation first - this queue will be based on priority
typedef struct {
  RequestNode *head;
  int size;
} RequestQueue;

void requestEnqueue(RequestQueue queue, Request request);
Request requestDequeue(RequestQueue queue);

typedef struct {
  Reservation reservation;
  Reservation *nextReservation;
} ReservationNode;

/*
  ReservationQueues will be used which reservation is added to the database first
  This queue will not be based on priority because no Reservations in the queue will conflict with each other
*/
typedef struct {
  Reservation head;
  int size;
} ReservationQueue;
