#include "user.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

char dbFilename[] = "ReservationSystem.sqlite";

User createUser(int userID, char *email, int type)
{
  User user;
  user.userID = userID;
  strcpy(user.email, email);
  user.type = type;
  return user;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void registerUser(User user)
{
  sqlite3 *db;
  int err;

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

  //Create a buffer for a string of up to 130 characters for the SQL statement
  char sql[130];
  sprintf(sql, "INSERT INTO User (PawsID, Email, Type) VALUES (%d, '%s', %d)", user.userID, user.email, user.type);
  printf("%s\n", sql);

  char *errMsg;

  err = sqlite3_exec(db, sql, callback, 0, &errMsg);
  if (err != SQLITE_OK)
  {
    printf("SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
  }
  else
    printf("You have been registered to the reservation system\n");

  sqlite3_close(db);
}