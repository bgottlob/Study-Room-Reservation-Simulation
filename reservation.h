#ifndef RESERVATION_H
#define RESERVATION_H

#include "user.h"
#include <time.h>

//All global variables are going to be defined here
extern char dbFilename[25];

//A Request represents the user's input into the system
typedef struct {
  int day;
  int startTime;
  int endTime;
  int seatsNeeded;
  User user;
  time_t timeCreated;
} Request;

/*
  A Reservation represents the actual room and time range that a user is trying to reserve a room for
  A Request will become a Reservation after a room is found for a user
  Admins will input a Reservation into the system, since they will know what room they need at what time already
*/
typedef struct {
  int roomNum;
  int day;
  int startTime;
  int endTime;
  User user;
} Reservation;

//Constructors for Requests and Reservations
Request createRequest(int day, int startTime, int endTime, User user);
Reservation createReservation(int roomNum, int day, int startTime, int endTime, User user);

/*
  Finds a room and makes a reservation for the user
  This is the entry point of a user thread
*/
void processRequest(Request request);

//Takes a request and returns a reservation for the user
Reservation findRoom(Request request);

//Adds a reservation to the database
void makeReservation();

#endif
