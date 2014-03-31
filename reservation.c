#include "reservation.h"
#include <string.h>
#include "sqlite3.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "queue.h"

#define MAX_ROOMS 26
#define MAX_RESERVATIONS 24

//Some global variables are going to be initialized here
char dbFilename[25] = "ReservationSystem.sqlite";
pthread_mutex_t findResLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t makeResLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dbLock = PTHREAD_MUTEX_INITIALIZER;

Request createRequest(int day, int startTime, int endTime, int seatsNeeded, User user)
{
  Request request;

  request.day = day;
  request.startTime = startTime;
  request.endTime = endTime;
  request.seatsNeeded = seatsNeeded;
  request.user = user;
  request.timeCreated = time(NULL);

  return request;
}

Reservation createReservation(int roomNum, int day, int startTime, int endTime, User user)
{
  Reservation reservation;

  reservation.roomNum = roomNum;
  reservation.day = day;
  reservation.startTime = startTime;
  reservation.endTime = endTime;
  reservation.user = user;

  return reservation;
}

static int selectRoomCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
  //This method is called once for each row that is returned by the SELECT query

  Room room;

  int i;
  //Iterating through the results of the SELECT query
  for(i = 0; i < argc; i++){
    if (strcmp(azColName[i], "RoomNumber") == 0)
    {
      //atoi converts the resulting string into an int
      room.roomNum = atoi(argv[i]);
    }
    else if (strcmp(azColName[i], "Seating") == 0)
    {
      room.seating = atoi(argv[i]);
    }
    else if (strcmp(azColName[i], "Purpose") == 0)
    {
      if (argv[i])
        room.hasPurpose = 1;
      else
        room.hasPurpose = 0;
    }
  }

  compatibleRooms[compatibleRoomsSize] = room;
  compatibleRoomsSize++;

  return 0;
}

static int selectResTimeCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
  //This method is called once for each row that is returned by the SELECT query

  TimeInterval interval;

  int i;
  //Iterating through the results of the SELECT query
  for(i = 0; i < argc; i++){
    if (strcmp(azColName[i], "StartTime") == 0)
      interval.startTime = atoi(argv[i]);
    else if (strcmp(azColName[i], "EndTime") == 0)
      interval.endTime = atoi(argv[i]);

    //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }

  resIntervals[resIntervalsSize] = interval;
  resIntervalsSize++;

  //printf("\n");
  return 0;
}

//-1 = r1 before r2, 0 = r1==r2, 1 = r1 after r2
int compareRooms(const void *r1, const void *r2)
{
  /*
    The sorting is first based off of the amount of seating: the rooms with less seats appear first in the array so that the smallest possible rooms for the request will try to be found first

    The sorting is then based off of hasPurpose.
    Rooms that have a hasPurpose value of 0 will appear first
    If a room has a special purpose, it is less likely to be booked by an administrator

    return 0 -> Both objects are equivalent
    return -1 –> r1 will be placed before r2
    return 1 –> r1 will be placed after r2
  */

  Room *room1 = (Room *)r1;
  Room *room2 = (Room *)r2;
  if (room1->seating == room2->seating)
  {
    if (room1->hasPurpose == room2->hasPurpose)
    {
      if (room1->roomNum == room2->roomNum)
        return 0;
      else if (room1->roomNum < room2->roomNum)
        return -1;
      else
        return 1;
    }
    else if (room1->hasPurpose < room2->hasPurpose)
      return -1;
    else
      return 1;
  }
  else if (room1->seating < room2->seating)
    return -1;
  else
    return 1;
}

Reservation findReservation(Request request, int mustReschedule, Reservation *adminRes)
{
  //The reservation struct that will be returned
  Reservation reservation;

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

      pthread_mutex_unlock(&reqQueueLock);
      pthread_mutex_unlock(&findResLock);
      pthread_mutex_unlock(&makeResLock);
      pthread_exit(NULL);
    }
  }
  else
  {
    //If the file doesn't exist, exit
    printf("The database could not be found: %s does not exist, exiting\n", dbFilename);

    pthread_mutex_unlock(&reqQueueLock);
    pthread_mutex_unlock(&findResLock);
    pthread_mutex_unlock(&makeResLock);
    pthread_exit(NULL);
  }

  //Create SQL statement
  char sql[150];
  sprintf(sql, "SELECT RoomNumber,Seating,Purpose FROM Room WHERE Seating >= %d", request.seatsNeeded);

  compatibleRooms = malloc(sizeof(Room) * MAX_ROOMS);
  compatibleRoomsSize = 0;

  pthread_mutex_lock(&dbLock);
  err = sqlite3_exec(db, sql, selectRoomCallback, (void*)data, &errMsg);
  pthread_mutex_unlock(&dbLock);

  if( err != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);

    //If there is an SQLite error, exit the thread
    pthread_mutex_unlock(&reqQueueLock);
    pthread_mutex_unlock(&findResLock);
    pthread_mutex_unlock(&makeResLock);
    pthread_exit(NULL);
  }

  //Sorts the array of rooms based on the criteria laid out by the compareRooms function
  qsort(compatibleRooms, compatibleRoomsSize, sizeof(Room), compareRooms);

  //If the a valid reservation can be made from a request, this will be set to 1
  int foundRes = 0;
  int roomIndex = 0;

  //Loop through each of the compatible rooms
  while (foundRes == 0 && roomIndex < compatibleRoomsSize)
  {
    Room currentRoom = compatibleRooms[roomIndex];

    resIntervals = malloc(sizeof(TimeInterval) * MAX_RESERVATIONS);
    resIntervalsSize = 0;

    sprintf(sql, "SELECT StartTime,EndTime FROM Reservation WHERE RoomNumber = %d AND Day = %d", currentRoom.roomNum, request.day);

    pthread_mutex_lock(&dbLock);
    err = sqlite3_exec(db, sql, selectResTimeCallback, (void*)data, &errMsg);
    pthread_mutex_unlock(&dbLock);

    if( err != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", errMsg);
      sqlite3_free(errMsg);

      //If there is an SQLite error, exit the thread
      pthread_mutex_unlock(&reqQueueLock);
      pthread_mutex_unlock(&findResLock);
      pthread_mutex_unlock(&makeResLock);
      pthread_exit(NULL);
    }

    int i;
    for (i = 0; i < resIntervalsSize; i++)
    {
      TimeInterval interval = resIntervals[i];
    }

    //This will be set to 1 if there any existing reservations overlap with what the user is trying to request
    int foundOverlap = 0;
    int timeIndex;
    for (timeIndex = 0; timeIndex < resIntervalsSize && foundOverlap == 0; timeIndex++)
    {
      TimeInterval interval = resIntervals[timeIndex];

      if (request.startTime <= interval.endTime && interval.startTime <= request.endTime)
        foundOverlap = 1;
    }

    //A boolean value that determines if the proposed rescheduling time conflicts with the admin's reservation time (if applicable)
    if (adminRes != NULL && mustReschedule == 1)
    {
      if (currentRoom.roomNum == adminRes->roomNum && request.day == adminRes->day)
      {
        //Checks if the times of the two reservations conflict
        if (request.startTime <= adminRes->endTime && adminRes->startTime <= request.endTime)
          foundOverlap = 1;
      }
    }

    //If no overlapping time periods have been found, the request is valid and the proper room has been found
    if (foundOverlap == 0)
    {
      //Initialize the reservation object to be returned
      reservation.roomNum = currentRoom.roomNum;
      reservation.day = request.day;
      reservation.startTime = request.startTime;
      reservation.endTime = request.endTime;
      reservation.user = request.user;

      pthread_mutex_lock(&resQueueLock);

      //If this reservation is already in the resQueue, it can't be made and a different reservation should be found - if the return value of the searchForRes function is 0, a matching reservation has not been found in the queue; if the return value is one, foundRes will stay at 0 and another reservation may be found
      if (searchForRes(&resQueue, reservation) == 0)
        foundRes = 1;
      pthread_mutex_unlock(&resQueueLock);
    }

    free(resIntervals);
    resIntervals = NULL;

    roomIndex++;
  }

  //Free the memory taken up by the array and have it point to NULL so its contents are released
  free(compatibleRooms);
  compatibleRooms = NULL;

  sqlite3_close(db);

  if (foundRes == 0 && mustReschedule == 0)
  {
    printf("No room for %s was found, exiting\n\n", request.user.email);
    pthread_mutex_unlock(&reqQueueLock);
    pthread_mutex_unlock(&findResLock);
    pthread_mutex_unlock(&makeResLock);
    pthread_exit(NULL);
  }
  else if (foundRes == 0 && mustReschedule == 1)
  {
    printf("%s: Your reservation was overridden by an administrator and could not be rescheduled\n\n", request.user.email);

    //Write to output file
    char fileName[154];
    sprintf(fileName, "%s.txt", request.user.email);

    FILE *file;
    file = fopen(fileName, "a+");
    if (file == NULL)
      printf("Error opening %s\n", fileName);
    else
      fprintf(file, "%s: Your reservation was overridden by an administrator and could not be rescheduled\n\n", request.user.email);

    fclose(file);

    Reservation res;
    res.day = -1;
    return res;
  }
  else if (foundRes == 1 && mustReschedule == 0) //This reservation will definitely be added to the database
  {
    //printf("%s: Your reservation is going to be scheduled as follows:\nRoomNum: %d\nDay: %d\nStartTime: %d\nEndTime: %d\n\n", reservation.user.email, reservation.roomNum, reservation.day, reservation.startTime, reservation.endTime);

    printf("%s: Your reservation is going to be scheduled\n\n", reservation.user.email);
  }

  return reservation;
}

void makeReservation(Reservation reservation)
{
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

      pthread_mutex_unlock(&resQueueLock);
      pthread_mutex_unlock(&makeResLock);
      pthread_exit(NULL);
    }
  }
  else
  {
    //If the file doesn't exist, exit
    printf("The database could not be found: %s does not exist, exiting\n", dbFilename);

    pthread_mutex_unlock(&resQueueLock);
    pthread_mutex_unlock(&makeResLock);
    pthread_exit(NULL);
  }

  //Create SQL INSERT statement
  char sql[150];
  sprintf(sql, "INSERT INTO Reservation (RoomNumber,Day,StartTime,EndTime,UserID) VALUES (%d,%d,%d,%d,%d)", reservation.roomNum, reservation.day, reservation.startTime, reservation.endTime, reservation.user.userID);

  pthread_mutex_lock(&dbLock);
  err = sqlite3_exec(db, sql, makeReservationCallback, (void*)data, &errMsg);
  pthread_mutex_unlock(&dbLock);

  if( err != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);

    pthread_mutex_unlock(&resQueueLock);
    pthread_mutex_unlock(&makeResLock);
    pthread_exit(NULL);
  }else
  {
    printf("%s: Your reservation has been scheduled as follows:\nRoomNum: %d\nDay: %d\nStartTime: %d\nEndTime: %d\n\n", reservation.user.email, reservation.roomNum, reservation.day, reservation.startTime, reservation.endTime);

    //Write to output file
    char fileName[154];
    sprintf(fileName, "%s.txt", reservation.user.email);

    FILE *file;
    file = fopen(fileName, "a+");
    if (file == NULL)
      printf("Error opening %s\n", fileName);
    else
      fprintf(file, "%s: Your reservation has been scheduled as follows:\nRoomNum: %d\nDay: %d\nStartTime: %d\nEndTime: %d\n\n", reservation.user.email, reservation.roomNum, reservation.day, reservation.startTime, reservation.endTime);

    fclose(file);
  }
}

static int makeReservationCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

static int selectResCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
  //This method is called once for each row that is returned by the SELECT query

  Reservation reservation;

  int i;
  //Iterating through the results of the SELECT query
  for(i = 0; i < argc; i++){
    if (strcmp(azColName[i], "StartTime") == 0)
      reservation.startTime = atoi(argv[i]);
    else if (strcmp(azColName[i], "EndTime") == 0)
      reservation.endTime = atoi(argv[i]);
    else if (strcmp(azColName[i], "UserID") == 0)
      reservation.user.userID = atoi(argv[i]);
    else if (strcmp(azColName[i], "RoomNumber") == 0)
      reservation.roomNum = atoi(argv[i]);
    else if (strcmp(azColName[i], "Day") == 0)
      reservation.day = atoi(argv[i]);
  }

  possibleReservations[possibleReservationsSize] = reservation;
  possibleReservationsSize++;

  return 0;
}

void makeAdminReservation(Reservation reservation)
{
  //Getting a list of reservations for the room and day the admin wants that are already in the database
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

      pthread_mutex_unlock(&makeResLock);
      pthread_exit(NULL);
    }
  }
  else
  {
    //If the file doesn't exist, exit
    printf("The database could not be found: %s does not exist, exiting\n", dbFilename);

    pthread_mutex_unlock(&makeResLock);
    pthread_exit(NULL);
  }

  //Create SQL statement
  char sql[150];
  sprintf(sql, "SELECT UserID,StartTime,EndTime,RoomNumber,Day FROM Reservation WHERE RoomNumber = %d AND Day = %d", reservation.roomNum, reservation.day);

  possibleReservations = malloc(sizeof(Reservation) * MAX_RESERVATIONS);
  possibleReservationsSize = 0;

  pthread_mutex_lock(&dbLock);
  err = sqlite3_exec(db, sql, selectResCallback, (void*)data, &errMsg);
  pthread_mutex_unlock(&dbLock);

  if( err != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);

    //If there is an SQLite error, exit the thread
    pthread_mutex_unlock(&makeResLock);
    pthread_exit(NULL);
  }

  int index;
  for (index = 0; index < possibleReservationsSize; index++)
  {
    Reservation currentRes = possibleReservations[index];

    if (currentRes.startTime <= reservation.endTime && reservation.startTime <= currentRes.endTime)
    {
      //currentRes must be deleted from the database and rescheduled if possible
      sprintf(sql, "DELETE FROM Reservation WHERE RoomNumber = %d AND Day = %d AND StartTime = %d AND EndTime = %d", currentRes.roomNum, currentRes.day, currentRes.startTime, currentRes.endTime);

      //Run the deletion from the database
      pthread_mutex_lock(&dbLock);
      err = sqlite3_exec(db, sql, selectResCallback, (void*)data, &errMsg);
      pthread_mutex_unlock(&dbLock);

      if( err != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);

        //If there is an SQLite error, exit the thread
        pthread_mutex_unlock(&makeResLock);
        pthread_exit(NULL);
      }

      currentResUser.userID = currentRes.user.userID;

      //Attempt to reschedule currentRes since it has just been deleted
      //First, find the user whose reservation has been overridden in the User table to complete his/her data
      sprintf(sql, "SELECT Email,Type FROM User WHERE PawsID = %d", currentResUser.userID);

      //Run the deletion from the database
      pthread_mutex_lock(&dbLock);
      err = sqlite3_exec(db, sql, selectUserCallback, (void*)data, &errMsg);
      pthread_mutex_unlock(&dbLock);

      if( err != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);

        //If there is an SQLite error, exit the thread
        pthread_mutex_unlock(&makeResLock);
        pthread_exit(NULL);
      }

      //Next, find the number of seats that were in the room from currentRes
      sprintf(sql, "SELECT Seating FROM Room WHERE RoomNumber = %d", currentRes.roomNum);

      //Run the deletion from the database
      pthread_mutex_lock(&dbLock);
      err = sqlite3_exec(db, sql, selectRoomNumCallback, (void*)data, &errMsg);
      pthread_mutex_unlock(&dbLock);

      if( err != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);

        //If there is an SQLite error, exit the thread
        pthread_mutex_unlock(&makeResLock);
        pthread_exit(NULL);
      }

      //The user and room seating data is complete, so a new request can now be created
      Request newReq = createRequest(currentRes.day, currentRes.startTime, currentRes.endTime, currentRoomSeating, currentResUser);
      Reservation newRes = findReservation(newReq, 1, &reservation);

      int search = searchForRes(&resQueue, newRes);

      //If the day of the new reservation was set to -1, a valid reservation was not found so newRes should not be added to the database
      //Also, in order for a reschedule to occur, the new reservaiton must not already be in the resQueue
      if (newRes.day != -1 && search == 0)
      {
        printf("%s: Your reservation is to be rescheduled\n", newRes.user.email);

        //Write to output file
        char fileName[154];
        sprintf(fileName, "%s.txt", newRes.user.email);

        FILE *file;
        file = fopen(fileName, "a+");
        if (file == NULL)
          printf("Error opening %s\n", fileName);
        else
          fprintf(file, "%s: Your reservation is to be rescheduled\n", newRes.user.email);

        fclose(file);

        makeReservation(newRes);
      }

    }
  }

  //Make the admin's original reservation
  makeReservation(reservation);
}

static int selectUserCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
  //This method is called once for each row that is returned by the SELECT query

  int i;
  //Iterating through the results of the SELECT query
  for(i = 0; i < argc; i++){
    if (strcmp(azColName[i], "Email") == 0)
      strcpy(currentResUser.email, argv[i]);
    else if (strcmp(azColName[i], "Type") == 0)
      currentResUser.type = atoi(argv[i]);
  }

  return 0;
}

static int selectRoomNumCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
  //This method is called once for each row that is returned by the SELECT query

  int i;
  //Iterating through the results of the SELECT query
  for(i = 0; i < argc; i++){
    if (strcmp(azColName[i], "Seating") == 0)
      currentRoomSeating = atoi(argv[i]);
  }

  return 0;
}

void cancelReservation(Reservation reservation)
{
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
      pthread_exit(NULL);
    }
  }
  else
  {
    //If the file doesn't exist, exit
    printf("The database could not be found: %s does not exist, exiting\n", dbFilename);
    pthread_exit(NULL);
  }

  didCancelRes = 0;

  //Create SQL statement
  char sql[150];
  sprintf(sql, "SELECT ID FROM Reservation WHERE RoomNumber = %d AND Day = %d AND StartTime = %d AND EndTime = %d AND UserID = %d", reservation.roomNum, reservation.day, reservation.startTime, reservation.endTime, reservation.user.userID);

  //Check to see if the reservation actually exists
  pthread_mutex_lock(&dbLock);
  err = sqlite3_exec(db, sql, cancelResCallback, (void*)data, &errMsg);
  pthread_mutex_unlock(&dbLock);

  if( err != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);

    pthread_exit(NULL);
  }

  if (didCancelRes == 1)
  {
    char sql[150];
    sprintf(sql, "DELETE FROM Reservation WHERE RoomNumber = %d AND Day = %d AND StartTime = %d AND EndTime = %d AND UserID = %d", reservation.roomNum, reservation.day, reservation.startTime, reservation.endTime, reservation.user.userID);

    //Delete the reservation
    pthread_mutex_lock(&dbLock);
    err = sqlite3_exec(db, sql, cancelResCallback, (void*)data, &errMsg);
    pthread_mutex_unlock(&dbLock);
    if( err != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", errMsg);
      sqlite3_free(errMsg);

      pthread_exit(NULL);
    }
    else
    {
      printf("%s: Your reservation in Room %d from %d to %d on %d has been cancelled\n", reservation.user.email, reservation.roomNum, reservation.startTime, reservation.endTime, reservation.day);

      //Write to output file
      char fileName[154];
      sprintf(fileName, "%s.txt", reservation.user.email);

      FILE *file;
      file = fopen(fileName, "a+");
      if (file == NULL)
        printf("Error opening %s\n", fileName);
      else
        fprintf(file, "%s: Your reservation in Room %d from %d to %d on %d has been cancelled\n", reservation.user.email, reservation.roomNum, reservation.startTime, reservation.endTime, reservation.day);

      fclose(file);
    }
  }
  else
  {
    printf("%s: Your reservation did not exist, so it could not be cancelled\n", reservation.user.email);

    //Write to output file
    char fileName[154];
    sprintf(fileName, "%s.txt", reservation.user.email);

    FILE *file;
    file = fopen(fileName, "a+");
    if (file == NULL)
      printf("Error opening %s\n", fileName);
    else
      fprintf(file, "%s: Your reservation did not exist, so it could not be cancelled\n", reservation.user.email);

    fclose(file);
  }
}

static int cancelResCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
  didCancelRes = 1;
  return 0;
}

void* startRequest(void *arg)
{
  //Convert the void pointer to a pointer to a Request
  Request *ptr = (Request *)arg;

  registerUser(ptr->user);

  pthread_t nextThread;
  pthread_create(&nextThread, NULL, processNextRequest, NULL);
  pthread_join(nextThread, NULL);

  return NULL;
}

void* processNextRequest()
{
  pthread_mutex_lock(&findResLock);

  pthread_mutex_lock(&reqQueueLock);

  Request request = dequeueRequest(&reqQueue);

  pthread_mutex_unlock(&reqQueueLock);

  Reservation reservation = findReservation(request, 0, NULL);

  pthread_mutex_lock(&resQueueLock);

  enqueueReservation(&resQueue, reservation);

  pthread_mutex_unlock(&resQueueLock);

  pthread_mutex_unlock(&findResLock);

  pthread_t nextThread;
  pthread_create(&nextThread, NULL, processNextReservation, NULL);
  pthread_join(nextThread, NULL);

  return NULL;
}

void* processNextReservation()
{

  pthread_mutex_lock(&makeResLock);

  pthread_mutex_lock(&findResLock);

  pthread_mutex_lock(&resQueueLock);

  Reservation reservation = dequeueReservation(&resQueue);

  pthread_mutex_unlock(&resQueueLock);

  pthread_mutex_unlock(&findResLock);

  //Gives findReservation operations the opportunity to run here to demonstrate concurrency
  sleep(2);

  //findResLock is locked in this thread because if a reservation has been dequeued but is not yet in the database, the findReservation method will not detect it and allow for the same room to be reserved at conflicting times

  pthread_mutex_lock(&findResLock);

    if (reservation.user.type == 0)
      makeAdminReservation(reservation);
    else
      makeReservation(reservation);

  pthread_mutex_unlock(&findResLock);

  pthread_mutex_unlock(&makeResLock);

  return NULL;
}

void* processAdminReservation(void *arg)
{
  Reservation *ptr = (Reservation *)arg;

  registerUser(ptr->user);

  //If a findReservation operation is going on while an administrator reservation is being serviced, the findReservation op may not see that an admin's reservation is going to be created, which could lead to a case where the same room is reserved at the same time
  pthread_mutex_lock(&findResLock);
  pthread_mutex_lock(&resQueueLock);
  enqueueReservation(&resQueue, *ptr);
  pthread_mutex_unlock(&resQueueLock);
  pthread_mutex_unlock(&findResLock);

  //Create another thread to process the next reservation so that the admin's reservation can be made
  pthread_t nextThread;
  pthread_create(&nextThread, NULL, processNextReservation, NULL);
  pthread_join(nextThread, NULL);

  return NULL;
}
