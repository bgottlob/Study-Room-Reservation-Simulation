#include <stdio.h>
#include "queue.h"
#include <string.h>
#include <unistd.h>
#include "reservation.h"
#include <pthread.h>
#include <stdlib.h>
#include "sqlite3.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  return 0;
}

int main() {

  //Initializing the global queues for use by child threads
  reqQueue = createRequestQueue();
  resQueue = createReservationQueue();

  //Deletes .txt output files from previous program execution
  system("rm *.txt");

  //Clearing the data in the datbase before testing
  sqlite3 *db;
  int err;
  char *errMsg;
  const char *data = "Callback function called";

  //Opening the database
  //Check if the file at dbFilename exists
  if(access(dbFilename, F_OK) != -1) {
    //If the file exists, open the database
    err = sqlite3_open(dbFilename, &db);
    if (err)
    {
      printf("The database could not be opened, exiting\n");
      exit(0);
    }
  }
  else
  {
    //If the file doesn't exist, exit
    printf("The database could not be found: %s does not exist, exiting\n", dbFilename);
    exit(0);
  }

  //Delete the data in the Reservation table
  char sql[150];
  sprintf(sql, "DELETE FROM Reservation");

  err = sqlite3_exec(db, sql, callback, (void*)data, &errMsg);

  if( err != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
    exit(0);
  }

  //Delete the data in the User table
  /*sprintf(sql, "DELETE FROM User");

  err = sqlite3_exec(db, sql, callback, (void*)data, &errMsg);
  if( err != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
    exit(0);
  }*/

  sqlite3_close(db);

  //----------------TEST CASES BEGIN HERE--------------------

  //----------------TEST CASE #1--------------------
  //Priority queue demonstration

  /*pthread_t t1, t2, t3, t4;

  //Creating two faculty requests followed by two student requests
  Request *fac1 = (Request *)malloc(sizeof(Request));
  *fac1 = createRequest(8, 12, 13, 6, createUser(122345, "faculty1@tcnj.edu", 2));

  //Used to ensure that each request has a different created time, which would be true in a real system
  sleep(2);

  Request *fac2 = (Request *)malloc(sizeof(Request));
  *fac2 = createRequest(8, 12, 13, 6, createUser(342567, "faculty2@tcnj.edu", 2));

  Request *stud1 = (Request *)malloc(sizeof(Request));
  *stud1 = createRequest(8, 12, 13, 6, createUser(232456, "student1@tcnj.edu", 1));

  sleep(2);

  Request *stud2 = (Request *)malloc(sizeof(Request));
  *stud2 = createRequest(8, 12, 13, 6, createUser(425678, "student2@tcnj.edu", 1));

  enqueueRequest(&reqQueue, *fac1);
  enqueueRequest(&reqQueue, *fac2);
  enqueueRequest(&reqQueue, *stud1);
  enqueueRequest(&reqQueue, *stud2);

  //Begin servicing requests concurrently
  pthread_create(&t1, NULL, startRequest, fac1);
  pthread_create(&t2, NULL, startRequest, fac2);
  pthread_create(&t3, NULL, startRequest, stud1);
  pthread_create(&t4, NULL, startRequest, stud2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);

  fac1 = NULL;
  free(fac1);

  fac2 = NULL;
  free(fac2);

  stud1 = NULL;
  free(stud1);

  stud2 = NULL;
  free(stud2);*/

  //----------------TEST CASE #2--------------------
  //Priority queue demonstration

  /*pthread_t t1, t2, t3, t4;

  //Creating two faculty requests followed by two student requests
  Request *fac1 = (Request *)malloc(sizeof(Request));
  *fac1 = createRequest(8, 12, 13, 12, createUser(122345, "faculty1@tcnj.edu", 2));

  sleep(2);

  Request *fac2 = (Request *)malloc(sizeof(Request));
  *fac2 = createRequest(8, 12, 13, 12, createUser(342567, "faculty2@tcnj.edu", 2));

  Request *stud1 = (Request *)malloc(sizeof(Request));
  *stud1 = createRequest(8, 12, 13, 12, createUser(232456, "student1@tcnj.edu", 1));

  sleep(2);

  Request *stud2 = (Request *)malloc(sizeof(Request));
  *stud2 = createRequest(8, 12, 13, 12, createUser(425678, "student2@tcnj.edu", 1));

  enqueueRequest(&reqQueue, *fac1);
  enqueueRequest(&reqQueue, *fac2);
  enqueueRequest(&reqQueue, *stud1);
  enqueueRequest(&reqQueue, *stud2);

  pthread_create(&t1, NULL, startRequest, fac1);
  pthread_create(&t2, NULL, startRequest, fac2);
  pthread_create(&t3, NULL, startRequest, stud1);
  pthread_create(&t4, NULL, startRequest, stud2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);

  fac1 = NULL;
  free(fac1);

  fac2 = NULL;
  free(fac2);

  stud1 = NULL;
  free(stud1);

  stud2 = NULL;
  free(stud2);*/

  //----------------TEST CASE #3-------------------

  /*pthread_t t1, t2, t3, t4;

  //Creating two faculty requests followed by two student requests
  Request *fac1 = (Request *)malloc(sizeof(Request));
  *fac1 = createRequest(8, 12, 13, 6, createUser(122345, "faculty1@tcnj.edu", 2));

  //Used to ensure that each request has a different created time, which would be true in a real system
  sleep(2);

  Request *fac2 = (Request *)malloc(sizeof(Request));
  *fac2 = createRequest(8, 12, 13, 6, createUser(342567, "faculty2@tcnj.edu", 2));

  Request *stud1 = (Request *)malloc(sizeof(Request));
  *stud1 = createRequest(8, 12, 13, 6, createUser(232456, "student1@tcnj.edu", 1));

  sleep(2);

  Request *stud2 = (Request *)malloc(sizeof(Request));
  *stud2 = createRequest(8, 12, 13, 6, createUser(425678, "student2@tcnj.edu", 1));

  enqueueRequest(&reqQueue, *fac1);
  enqueueRequest(&reqQueue, *fac2);
  enqueueRequest(&reqQueue, *stud1);
  enqueueRequest(&reqQueue, *stud2);

  //Begin servicing requests concurrently
  pthread_create(&t1, NULL, startRequest, fac1);
  pthread_create(&t2, NULL, startRequest, fac2);
  pthread_create(&t3, NULL, startRequest, stud1);
  pthread_create(&t4, NULL, startRequest, stud2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);

  fac1 = NULL;
  free(fac1);

  fac2 = NULL;
  free(fac2);

  stud1 = NULL;
  free(stud1);

  stud2 = NULL;
  free(stud2);

  cancelReservation(createReservation(202, 8, 12, 13, createUser(425678, "student2@tcnj.edu", 1)));*/

  //-----------TEST CASE #4--------------
  /*pthread_t t1, t2, t3, t4, t5;

  //Creating two faculty requests followed by two student requests
  Request *fac1 = (Request *)malloc(sizeof(Request));
  *fac1 = createRequest(8, 12, 13, 6, createUser(122345, "faculty1@tcnj.edu", 2));

  //Used to ensure that each request has a different created time, which would be true in a real system
  sleep(2);

  Request *fac2 = (Request *)malloc(sizeof(Request));
  *fac2 = createRequest(8, 12, 13, 6, createUser(342567, "faculty2@tcnj.edu", 2));

  Request *stud1 = (Request *)malloc(sizeof(Request));
  *stud1 = createRequest(8, 12, 13, 6, createUser(232456, "student1@tcnj.edu", 1));

  sleep(2);

  Request *stud2 = (Request *)malloc(sizeof(Request));
  *stud2 = createRequest(8, 12, 13, 6, createUser(425678, "student2@tcnj.edu", 1));

  enqueueRequest(&reqQueue, *fac1);
  enqueueRequest(&reqQueue, *fac2);
  enqueueRequest(&reqQueue, *stud1);
  enqueueRequest(&reqQueue, *stud2);

  Reservation *adminRes = (Reservation *)malloc(sizeof(Reservation));
  *adminRes = createReservation(110, 8, 12, 13, createUser(852365, "admin@tcnj.edu", 0));

  //Begin servicing requests concurrently
  pthread_create(&t1, NULL, startRequest, fac1);
  pthread_create(&t2, NULL, startRequest, fac2);
  pthread_create(&t3, NULL, startRequest, stud1);
  pthread_create(&t4, NULL, startRequest, stud2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);

  pthread_create(&t5, NULL, processAdminReservation, adminRes);
  pthread_join(t5, NULL);

  fac1 = NULL;
  free(fac1);

  fac2 = NULL;
  free(fac2);

  stud1 = NULL;
  free(stud1);

  stud2 = NULL;
  free(stud2);

  adminRes = NULL;
  free(adminRes);*/


  //-----------------TEST CASE #5----------------
  /*pthread_t t1, t2, t3, t4, t5;

  //Creating two faculty requests followed by two student requests
  Request *fac1 = (Request *)malloc(sizeof(Request));
  *fac1 = createRequest(8, 12, 13, 6, createUser(122345, "faculty1@tcnj.edu", 2));

  //Used to ensure that each request has a different created time, which would be true in a real system
  sleep(2);

  Request *fac2 = (Request *)malloc(sizeof(Request));
  *fac2 = createRequest(8, 12, 13, 6, createUser(342567, "faculty2@tcnj.edu", 2));

  Request *stud1 = (Request *)malloc(sizeof(Request));
  *stud1 = createRequest(8, 12, 13, 6, createUser(232456, "student1@tcnj.edu", 1));

  sleep(2);

  Request *stud2 = (Request *)malloc(sizeof(Request));
  *stud2 = createRequest(8, 12, 13, 6, createUser(425678, "student2@tcnj.edu", 1));

  enqueueRequest(&reqQueue, *fac1);
  enqueueRequest(&reqQueue, *fac2);
  enqueueRequest(&reqQueue, *stud1);
  enqueueRequest(&reqQueue, *stud2);

  Reservation *adminRes = (Reservation *)malloc(sizeof(Reservation));
  *adminRes = createReservation(110, 8, 12, 13, createUser(852365, "admin@tcnj.edu", 0));

  //Begin servicing requests concurrently
  pthread_create(&t1, NULL, startRequest, fac1);
  pthread_create(&t2, NULL, startRequest, fac2);
  pthread_create(&t3, NULL, startRequest, stud1);
  pthread_create(&t4, NULL, startRequest, stud2);
  pthread_create(&t5, NULL, processAdminReservation, adminRes);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);
  pthread_join(t5, NULL);

  fac1 = NULL;
  free(fac1);

  fac2 = NULL;
  free(fac2);

  stud1 = NULL;
  free(stud1);

  stud2 = NULL;
  free(stud2);

  adminRes = NULL;
  free(adminRes);*/

  //-----------------TEST CASE #6----------------
  /*pthread_t t1, t2, t3, t4, t5;

  //Creating two faculty requests followed by two student requests
  Request *fac1 = (Request *)malloc(sizeof(Request));
  *fac1 = createRequest(8, 12, 13, 12, createUser(122345, "faculty1@tcnj.edu", 2));

  //Used to ensure that each request has a different created time, which would be true in a real system
  sleep(2);

  Request *fac2 = (Request *)malloc(sizeof(Request));
  *fac2 = createRequest(8, 12, 13, 12, createUser(342567, "faculty2@tcnj.edu", 2));

  Request *stud1 = (Request *)malloc(sizeof(Request));
  *stud1 = createRequest(8, 12, 13, 12, createUser(232456, "student1@tcnj.edu", 1));

  sleep(2);

  Request *stud2 = (Request *)malloc(sizeof(Request));
  *stud2 = createRequest(8, 12, 13, 12, createUser(425678, "student2@tcnj.edu", 1));

  enqueueRequest(&reqQueue, *fac1);
  enqueueRequest(&reqQueue, *fac2);
  enqueueRequest(&reqQueue, *stud1);
  enqueueRequest(&reqQueue, *stud2);

  Reservation *adminRes = (Reservation *)malloc(sizeof(Reservation));
  *adminRes = createReservation(413, 8, 12, 13, createUser(852365, "admin@tcnj.edu", 0));

  //Begin servicing requests concurrently
  pthread_create(&t1, NULL, startRequest, fac1);
  pthread_create(&t2, NULL, startRequest, fac2);
  pthread_create(&t3, NULL, startRequest, stud1);
  pthread_create(&t4, NULL, startRequest, stud2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);

  pthread_create(&t5, NULL, processAdminReservation, adminRes);
  pthread_join(t5, NULL);

  fac1 = NULL;
  free(fac1);

  fac2 = NULL;
  free(fac2);

  stud1 = NULL;
  free(stud1);

  stud2 = NULL;
  free(stud2);

  adminRes = NULL;
  free(adminRes);*/

  return 0;
}
