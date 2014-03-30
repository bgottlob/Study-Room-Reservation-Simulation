#include "user.h"
#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "reservation.h"

User createUser(int userID, char *email, int type)
{
  User user;
  user.userID = userID;
  strcpy(user.email, email);
  user.type = type;
  return user;
}

static int registerUserCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

static int userFoundCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
  userFound = 1;
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
      pthread_exit(NULL);
    }
  }
  else
  {
    //If the file doesn't exist, exit
    printf("The database could not be found: %s does not exist\n", dbFilename);
    pthread_exit(NULL);
  }

  char sql[150];
  char *errMsg;

  sprintf(sql, "SELECT PawsID FROM User WHERE PawsID = %d AND Email = '%s' AND Type = %d", user.userID, user.email, user.type);

  pthread_mutex_lock(&dbLock);
  userFound = 0;
  err = sqlite3_exec(db, sql, userFoundCallback, 0, &errMsg);
  if (err != SQLITE_OK)
  {
    printf("SQL error: %s, exiting\n", errMsg);
    sqlite3_free(errMsg);

    pthread_mutex_unlock(&dbLock);
    pthread_exit(NULL);
  }

  //If userFound == 0, the user was not found in the database and that user must now be added
  if (userFound == 0)
  {
    //Reset userFound for the next query
    userFound = 1;
    pthread_mutex_unlock(&dbLock);

    sprintf(sql, "INSERT INTO User (PawsID, Email, Type) VALUES (%d, '%s', %d)", user.userID, user.email, user.type);

    pthread_mutex_lock(&dbLock);
    err = sqlite3_exec(db, sql, registerUserCallback, 0, &errMsg);
    pthread_mutex_unlock(&dbLock);

    if (err != SQLITE_OK)
    {
      printf("SQL error: %s, exiting\n", errMsg);
      sqlite3_free(errMsg);
      pthread_exit(NULL);
    }
    else
    {
      printf("%s has been registered to the reservation system\n", user.email);

      char fileName[154];
      sprintf(fileName, "%s.txt", user.email);

      FILE *file;
      file = fopen(fileName, "a+");
      if (file == NULL)
        printf("Error opening %s\n", fileName);
      else
        fprintf(file, "%s has been registered to the reservation system\n", user.email);

      fclose(file);
    }
  }
  else
  {
    pthread_mutex_unlock(&dbLock);
  }

  sqlite3_close(db);
}
