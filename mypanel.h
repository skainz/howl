#include "types.h"

typedef struct MyPanel {

  int cursor;
  int numfiles;
  int toprow;
  Row* zeilen;
  int page_height;
} MyPanel;


void  mypanel_nav_home(MyPanel *panel);
void  mypanel_nav_npage(MyPanel *panel);
