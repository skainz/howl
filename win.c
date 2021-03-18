#include <curses.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

#include "mypanel.h"

//WINDOW /* *win1,*/ *win2;

#define BUFLEN  PATH_MAX
static wchar_t WBUF[BUFLEN];

extern NCURSES_EXPORT(int) mvwaddnwstr (WINDOW *, int, int, const wchar_t *, int);





int  ls(Row **rowsp,char* path)
{
  DIR *dp;
  struct dirent *ep;
  Row *rows;
  int n=0; // NUmber of files in current dir
  int i; //index pointer
  
  if (!(dp=opendir(path)))
    {
      return -1; // unable to open dir, for whatever reason
    }

  while (readdir(dp))
    {
      n++;
    }
  rows=malloc(n*sizeof(Row));
  
  rewinddir(dp);
  i=0;
  while ((ep = readdir(dp)))
    {
      rows[i].name=malloc(strlen(ep->d_name)+2);

      strcpy(rows[i].name,ep->d_name);
      i++;
      //     printf("%d %s\n",i,ep->d_name);
    }

  closedir(dp);
  *rowsp=rows;
  return n;
}

    

int b,h;
//int files;
//int cursor=0;
MyPanel links,rechts;
 
void quit(void)
{
  delwin(links.window);
  //  delwin(win2);
  endwin();
}

void draw(MyPanel * panel, int start)
{
  
  int j;
  
  //get dimensions of left window
  int h,b;
  getmaxyx(panel->window, h, b);
  int page_height=h-2;
  //  int start=2;
  int lines=panel->numfiles;

  if (page_height<panel->numfiles) {lines=page_height;}

  //  printf("drawing %d lines\n",lines);
  //exit(1);
  for (j=0;j<lines; j++)
    {
      if (j==panel->cursor)
	{
      wattrset(panel->window,COLOR_PAIR(1));

	} else
	{
      wattrset(panel->window,COLOR_PAIR(2));
	  
	}

      mbstowcs(WBUF,panel->zeilen[j+start].name,PATH_MAX);
            mvwaddnwstr(panel->window,j+1,2,WBUF,40);

      //      mvwaddstr(win1,j+1,2+strlen(zeilen[j+start].name)+1,"xxxx");
	    //      mvwhline(win1, j+1, 2+strlen(zeilen[j+start].name), 'x', 5);
      //      wchgat(win1,j+1,10,COLOR_PAIR(1));
      //  whline(win1,"-",20-strlen(WBUF));
      wclrtoeol(panel->window);
      wattrset(panel->window,COLOR_PAIR(2));
      
      //      mvwaddnstr(win1, j + 1, 2, zeilen[j].name,10);
    }
  //   mvwaddstr(win1, 3, 3, "Hallo win");
  // mvwaddstr(win2, 7, 3, "Diese Zeichenkette wird nicht angezeigt!"); 
  // da ausserhalb des win-Anzeigebereichs
  box(panel->window,0,0);
  //  box(win2,0,0);
  //    mvwchgat(win1,0, 0, 1, A_REVERSE, 1, NULL);	
  char SBUF[128];
  //    files=files*files;
  snprintf(SBUF,b," %d Dateien ",panel->numfiles);
  mvwaddstr(panel->window,h-1,b-strlen(SBUF)-1,SBUF);

  snprintf(SBUF,b,"ph: %d, files %d, z:%d, cursor: %d Dateien ",page_height,panel->numfiles,start,panel->cursor);
  mvwaddstr(panel->window,h-1,b-strlen(SBUF)-1,SBUF);

  
}


int main(void)
{


  MyPanel *currentp,*other,*b;
  links.cwd="/usr/include";
  links.numfiles=ls(&links.zeilen,links.cwd/*"/usr/include"*/);
  rechts.numfiles=ls(&rechts.zeilen,"/usr/share/doc");

  currentp=&links;
  other=&rechts;
    //  printf("anzahl: %d\n",links.numfiles);exit(0);

  setlocale(LC_ALL, "");

  /*printf("\n%d dateien\n",files);
    exit(0);
  */
  initscr();
  atexit(quit);
  clear();
  noecho();
  curs_set(0);
    cbreak();
    //  nl();
  keypad(stdscr, TRUE);

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_GREEN);
  init_pair(2, COLOR_WHITE, COLOR_RED);
  init_pair(3, COLOR_RED, COLOR_MAGENTA);

  links.window = newwin(LINES, COLS/2, 0, 0);
  rechts.window = newwin(LINES,COLS/2,0,COLS/2);
  //  bkgd(COLOR_PAIR(1));
    wbkgd(links.window, COLOR_PAIR(2));
  wbkgd(rechts.window, COLOR_PAIR(1));

  draw(&links,0);
  draw(&rechts,0);
  draw(currentp,0);
  //  mvaddstr(5,5, "Hallo stdscr");

  
  refresh();
  wrefresh(links.window);
  wrefresh(rechts.window);
  //  int z=0;
  
  int ch;
  int page_height;
  while((ch=getch()) != KEY_F(10))
    {
      //  printf("%d\n",ch);exit(0);
      //      printf("KC: %d\n",ch);
	
      getmaxyx(stdscr, h, b);
      page_height=h-2;
      currentp->page_height=page_height;
      switch (ch)
	{
	case 9:
	  b=currentp;
	  currentp=other;
	  other=b;
	  //	  exit(0);
	  break;
	  
	case KEY_NPAGE:
	  mypanel_nav_npage(currentp);
	  break;

	case KEY_HOME:
	  mypanel_nav_home(currentp);
	  break;

	case KEY_END:
	  mypanel_nav_end(currentp);
	  break;

	case KEY_PPAGE:
	  mypanel_nav_ppage(currentp);
	  break;

	case KEY_UP: 
	  mypanel_nav_up(currentp);
	  break;

	case KEY_DOWN:
	  mypanel_nav_down(currentp);
	  break;
	}
   
      /*    wresize(win1,h,b/2);
      wresize(win2,h,b/2);
      mvwin(win2,0,b/2);
    
      wclear(win1);
      wclear(win2);
      */
      draw(currentp,currentp->toprow);
    
      refresh();
      wrefresh(links.window);
      wrefresh(rechts.window);

      //printf("%d %d ",b,h);
      //  z++;
    
    } 

  return(0);
}
