#include <stdio.h>
#include "queue.h"
#include <string.h>
#include <unistd.h>
#include "reservation.h"

int main() {

  //Initializing the global queues for use by child threads
  reqQueue = createRequestQueue();
  resQueue = createReservationQueue();

  Request req = createRequest(5, 12, 13, 12, createUser(867530, "smittywerbenjagermanjensen@gmail.com", 1));
  findReservation(req);

  return 0;
}
