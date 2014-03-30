#ifndef USER_H
#define USER_H

typedef struct {
  int userID;
  char email[150];
  int type;
} User;

/*
  An enumerated type for the user type
  admin = 0
  student = 1
  faculty = 2
*/
enum { admin, student, faculty };

//This variable will be used in the registerUserCallback to determine whether a user was found for a certain SELECT query on the user table
int userFound;

static int registerUserCallback(void *NotUsed, int argc, char **argv, char **azColName);

//This will be called if a user is found for certain SELECT query on the user table
static int userFoundCallback(void *NotUsed, int argc, char **argv, char **azColName);

//Adds the user to the database
void registerUser(User user);

User createUser(int userID, char *email, int type);

#endif
