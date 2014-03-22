typedef struct {
  int day;
  int startTime;
  int endTime;
} TimeRange;

typedef struct {
  int roomNum;
  TimeRange times;
} Reservation;
