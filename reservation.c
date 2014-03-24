#include "reservation.h"
#include <string.h>

//All global variables are going to be initialized here
char dbFilename[25] = "ReservationSystem.sqlite";

Request createRequest(int day, int startTime, int endTime, User user)
{
  Request request;

  request.day = day;
  request.startTime = startTime;
  request.endTime = endTime;
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

void processRequest()
{

}
