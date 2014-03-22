typedef struct {
  User userData;
  User *nextUser;
} UserNode;

typedef struct {
  UserNode head;
  int size;
} UserQueue;

void enqueue(User addUser);
User dequeue();
