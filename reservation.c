#include "reservation.h"
#include <string.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_ROOMS 26
#define MAX_RESERVATIONS 16

//All global variables are going to be initialized here
char dbFilename[25] = "ReservationSystem.sqlite";

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

    //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }

  compatibleRooms[compatibleRoomsSize] = room;
  compatibleRoomsSize++;

  //printf("\n");

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

Reservation findReservation(Request request)
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
      printf("The database could not be opened\n");
      exit(0);
    }
  }
  else
  {
    //If the file doesn't exist, exit
    printf("The database could not be found: %s does not exist\n", dbFilename);
    exit(0);
  }

  //Create SQL statement
  char sql[150];
  sprintf(sql, "SELECT RoomNumber,Seating,Purpose FROM Room WHERE Seating >= %d", request.seatsNeeded);
  printf("%s\n", sql);

  compatibleRooms = malloc(sizeof(Room) * MAX_ROOMS);
  compatibleRoomsSize = 0;

  //Execute SQL statement
  err = sqlite3_exec(db, sql, selectRoomCallback, (void*)data, &errMsg);
  if( err != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
  }else{
    printf("Operation done successfully\n");
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

    err = sqlite3_exec(db, sql, selectResTimeCallback, (void*)data, &errMsg);
    if( err != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", errMsg);
      sqlite3_free(errMsg);
      exit(0);
    }

    printf("Room #: %d\n", currentRoom.roomNum);

    for (int i = 0; i < resIntervalsSize; i++)
    {
      TimeInterval interval = resIntervals[i];
      printf("StartTime: %d\nEndTime: %d\n", interval.startTime, interval.endTime);
    }

    //This will be set to 1 if there any existing reservations overlap with what the user is trying to request
    int foundOverlap = 0;
    for (int timeIndex = 0; timeIndex < resIntervalsSize && foundOverlap == 0; timeIndex++)
    {
      TimeInterval interval = resIntervals[timeIndex];

      if (request.startTime <= interval.endTime && interval.startTime <= request.endTime)
        foundOverlap = 1;
    }

    //If no overlapping time periods have been found, the request is valid and the proper room has been found
    if (foundOverlap == 0)
    {
      foundRes = 1;

      //Initialize the reservation object to be returned
      reservation.roomNum = currentRoom.roomNum;
      reservation.day = request.day;
      reservation.startTime = request.startTime;
      reservation.endTime = request.endTime;
      reservation.user = request.user;
    }

    free(resIntervals);
    resIntervals = NULL;

    roomIndex++;
  }

  //Free the memory taken up by the array and have it point to NULL so its contents are released
  free(compatibleRooms);
  compatibleRooms = NULL;

  sqlite3_close(db);

  if (foundRes == 0)
  {
    printf("No room for the specified times was found, exiting\n");
    exit(0);
  }
  /*else
    printf("Your request can be completed\nYour reservation is as follows:\nRoomNum: %d\nDay: %d\nStartTime: %d\nEndTime: %d\n", reservation.roomNum, reservation.day, reservation.startTime, reservation.endTime);*/

  return reservation;
}
