#include "user.h"

//Used to specify a range of time on a certain day
typedef struct {
  //The day number of the month, which can range from 1 to 31
  int day;

  //startTime and endTime represent hours in a 24-hour clock
  int startTime;
  int endTime;
} TimeRange;

//A Request represents the user's input into the system
typedef struct {
  int day;
  int seatsNeeded;
  User user;
  TimeRange times;
} Request;

/*
  A Reservation represents the actual room and time range that a user is trying to reserve a room for
  A Request will become a Reservation after a room is found for a user
  Admins will input a Reservation into the system, since they will know what room they need at what time already
*/
typedef struct {
  int roomNum;
  TimeRange times;
  User user;
} Reservation;

//Finds a room and creates a reservation from it for the user
void processRequest();

//Takes a request and returns a reservation for the user
void findRoom();

//Adds a reservation to the database
void makeReservation();
