#ifndef RESERVATION_H
#define RESERVATION_H

#include "user.h"
#include <time.h>

//Global variable for the file name of the database
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

//A room struct stores information about a room when data is queried from the database
typedef struct {
  int roomNum;
  int seating;
  int hasPurpose;
} Room;

//A TimeInterval struct stores a startTime and endTime, and will be used to check for overlap among time intervals
typedef struct {
  int startTime;
  int endTime;
} TimeInterval;

//Static variable for an array of Room structs to be used in the selectRoomCallback and findReservation functions
static Room *compatibleRooms;
//A static variable for the number of elements in the compatibleRooms array
int compatibleRoomsSize;

int compareRooms(const void *r1, const void *r2);

//A callback function to get the results of a SELECT query on the Room table
static int selectRoomCallback(void *NotUsed, int argc, char **argv, char **azColName);

//Constructors for Requests and Reservations
Request createRequest(int day, int startTime, int endTime, int seatsNeeded, User user);
Reservation createReservation(int roomNum, int day, int startTime, int endTime, User user);

/*
  Finds a room and makes a reservation for the user
  This is the entry point of a user thread
*/
void processRequest(Request request);

/*
  Makes a reservation for an admin user
*/
void processAdminReservation(Reservation reservation);

//Takes a request and returns a reservation for the user
Reservation findReservation(Request request);

//Adds a reservation to the database
void makeReservation();

//A callback function to get the results of a SELECT query on the Reservation table and store TimeIntervals
static int selectResTimeCallback(void *NotUsed, int argc, char **argv, char **azColName);

//Static variable for holding the TimeInterval structs found from a query on the Reservation table
static TimeInterval *resIntervals;
int resIntervalsSize;

#endif
