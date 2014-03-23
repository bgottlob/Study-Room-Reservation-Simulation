#include "reservation.h"

Request createRequest(int day, int startTime, int endTime, User user)
{
  Request request;

  request.day = day;
  request.startTime = startTime;
  request.endTime = endTime;
  request.user = user;

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
