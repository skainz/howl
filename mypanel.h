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
int mypanel_mark_offn(MyPanel *panel);
int mypanel_mark_toggle(MyPanel *panel);


//Sorting

int _mypanel_cmpfunc_name_asc(const void *a,const void *b);
int _mypanel_cmpfunc_name_desc(const void *a,const void *b);
int _mypanel_cmpfunc_dirs_top(const void *a,const void *b);

void _mypanel_sort_dirs_top();


// file system functions

int _mypanel_cd(MyPanel *panel, char*path);

