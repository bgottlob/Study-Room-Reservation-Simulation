#include <stdio.h>
#include "queue.h"
#include <string.h>

extern char *dbFilename;

int main() {

  RequestQueue queue = createRequestQueue();
  Request request = createRequest(1, 3, 4, createUser(432342, "dsflkjsdl", admin));
  enqueueRequest(&queue, request);

  Request newreq = dequeueRequest(&queue);

  printf("Email: %s\n", newreq.user.email);

  return 0;
}
