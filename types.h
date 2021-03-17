#include <sys/types.h>
#include <dirent.h>
#include <curses.h>


typedef struct Row {
  char* name;
  off_t size;
  mode_t mode;
  int islink;
  int marked;
} Row;
