#ifndef RESERVATION_H
#define RESERVATION_H

#include "user.h"
#include <time.h>
#include <pthread.h>

//Global variable for the file name of the database
extern char dbFilename[25];

//Global mutex locks for reservation and request operations
extern pthread_mutex_t findResLock, makeResLock, dbLock;

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
static int compatibleRoomsSize;

int compareRooms(const void *r1, const void *r2);

//A callback function to get the results of a SELECT query on the Room table
static int selectRoomCallback(void *NotUsed, int argc, char **argv, char **azColName);

//Constructors for Requests and Reservations
Request createRequest(int day, int startTime, int endTime, int seatsNeeded, User user);
Reservation createReservation(int roomNum, int day, int startTime, int endTime, User user);

//Takes a request and returns a reservation for the user - if mustReschedule is set to 1, that means that the request was formerly a deleted reservation and must be rescheduled - adminRes is a pointer to a Reservation that the returned reservation must not conflict with
Reservation findReservation(Request request, int mustReschedule, Reservation *adminRes);

//Adds a reservation to the database
void makeReservation(Reservation reservation);

//Adds an admin's reservation to the database and attempts to reschedule and overridden ones
void makeAdminReservation(Reservation reservation);

//A static array variable for storing all of the Reservations returned by a SELECT statement in the makeAdminReservation function
static Reservation *possibleReservations;
//A static variable for the number of elements in the possibleReservations array
static int possibleReservationsSize;

//Used to set currentResUser from the results of a query on the User table
static int selectUserCallback(void *NotUsed, int argc, char **argv, char **azColName);

//A static User variable to store the user's data from a SELECT statement on the User table
static User currentResUser;
//A static int variable to store a number of seats in a room returned by a SELECT statment
int currentRoomSeating;

//A callback function for the makeReservation INSERT statement just in case something goes wrong
static int makeReservationCallback(void *NotUsed, int argc, char **argv, char **azColName);

//A callback function to get the results of a SELECT query on the Reservation table and store TimeIntervals
static int selectResTimeCallback(void *NotUsed, int argc, char **argv, char **azColName);

//A callback to set the currentRoomSeating variable from a SELECT statment on the Room table
static int selectRoomNumCallback(void *NotUsed, int argc, char **argv, char **azColName);

//Static variable for holding the TimeInterval structs found from a query on the Reservation table
static TimeInterval *resIntervals;
static int resIntervalsSize;

//Cancels a user's reservation
void cancelReservation(Reservation reservation);
//A static int variable for that indicates whether a canceled reservation previously existed or not
static int didCancelRes;

//Callback that is called if a reservation was deleted
static int cancelResCallback(void *NotUsed, int argc, char **argv, char **azColName);

//Enqueues a request on the request queue and uses mutex locks for synchronization then creates a new thread for dequeuing and servicing a request
void *startRequest(void *arg);

//Dequeues the next request in the request queue and finds a reservation for it
void* processNextRequest();

//Dequeues the next reservation in the reservation queue and makes the reservation
void* processNextReservation();

//Makes a reservation for an admin user
void* processAdminReservation(void *arg);

#endif
