#include "types.h"
#include <curses.h>
typedef struct MyPanel {

  int cursor;
  int numfiles;
  int toprow;
  Row* zeilen;
  int page_height;
  WINDOW* window;
  char* cwd;

} MyPanel;


//Keyboard navigation functions
void mypanel_nav_up(MyPanel *panel);
void mypanel_nav_down(MyPanel *panel);
void mypanel_nav_ppage(MyPanel *panel);
void mypanel_nav_npage(MyPanel *panel);
void mypanel_nav_home(MyPanel *panel);
void mypanel_nav_end(MyPanel *panel);


//Highlighting items

int mypanel_mark_on(MyPanel *panel);
