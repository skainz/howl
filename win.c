#include <curses.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

#include "mypanel.h"

WINDOW *win1, *win2;

#define BUFLEN  PATH_MAX
static wchar_t WBUF[BUFLEN];

extern NCURSES_EXPORT(int) mvwaddnwstr (WINDOW *, int, int, const wchar_t *, int);





int  ls(Row **rowsp)
{
  DIR *dp;
  struct dirent *ep;
  Row *rows;
  int n=0; // NUmber of files in current dir
  int i; //index pointer
  
  if (!(dp=opendir("/tmp/f")))
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
  MyPanel links;
 
void quit(void)
{
  delwin(win1);
  delwin(win2);
  endwin();
}

void draw(int start)
{
  
  int j;
  
  //get dimensions of left window
  int h,b;
  getmaxyx(win1, h, b);
  int page_height=h-2;
  //  int start=2;
  int lines=links.numfiles;

  if (page_height<links.numfiles) {lines=page_height;}

  //  printf("drawing %d lines\n",lines);
  //exit(1);
  for (j=0;j<lines; j++)
    {
      if (j==links.cursor)
	{
      wattrset(win1,COLOR_PAIR(1));

	} else
	{
      wattrset(win1,COLOR_PAIR(2));
	  
	}

      mbstowcs(WBUF,links.zeilen[j+start].name,PATH_MAX);
            mvwaddnwstr(win1,j+1,2,WBUF,40);

      //      mvwaddstr(win1,j+1,2+strlen(zeilen[j+start].name)+1,"xxxx");
	    //      mvwhline(win1, j+1, 2+strlen(zeilen[j+start].name), 'x', 5);
      //      wchgat(win1,j+1,10,COLOR_PAIR(1));
      //  whline(win1,"-",20-strlen(WBUF));
      wclrtoeol(win1);
      wattrset(win1,COLOR_PAIR(2));
      
      //      mvwaddnstr(win1, j + 1, 2, zeilen[j].name,10);
    }
  //   mvwaddstr(win1, 3, 3, "Hallo win");
  // mvwaddstr(win2, 7, 3, "Diese Zeichenkette wird nicht angezeigt!"); 
  // da ausserhalb des win-Anzeigebereichs
  box(win1,0,0);
  box(win2,0,0);
  //    mvwchgat(win1,0, 0, 1, A_REVERSE, 1, NULL);	
  char SBUF[128];
  //    files=files*files;
  snprintf(SBUF,b," %d Dateien ",links.numfiles);
  mvwaddstr(win1,h-1,b-strlen(SBUF)-1,SBUF);

  snprintf(SBUF,b,"ph: %d, files %d, z:%d, cursor: %d Dateien ",page_height,links.numfiles,start,links.cursor);
  mvwaddstr(win1,h-1,b-strlen(SBUF)-1,SBUF);

  
}


int main(void)
{


    links.numfiles=ls(&links.zeilen);
  
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
  nl();
  keypad(stdscr, TRUE);

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_GREEN);
  init_pair(2, COLOR_WHITE, COLOR_RED);
  init_pair(3, COLOR_RED, COLOR_MAGENTA);

  win1 = newwin(LINES, COLS/2, 0, 0);
  win2 = newwin(LINES,COLS/2,0,COLS/2);
  //  bkgd(COLOR_PAIR(1));
    wbkgd(win1, COLOR_PAIR(2));
  wbkgd(win2, COLOR_PAIR(1));
  
  draw(0);
  //  mvaddstr(5,5, "Hallo stdscr");

  
  refresh();
  wrefresh(win1);
  wrefresh(win2);
  //  int z=0;
  
  int ch;
  int page_height;
  while((ch=getch()) != KEY_F(10))
    {
      getmaxyx(stdscr, h, b);
      page_height=h-2;
      links.page_height=page_height;
      switch (ch)
	{
	case KEY_NPAGE:
	  mypanel_nav_npage(&links);
	  break;

	case KEY_HOME:
	  mypanel_nav_home(&links);
	  break;

	case KEY_END:

	  if ((links.toprow==0) && (links.toprow+page_height>(links.numfiles-1)))
	    {
	      links.cursor=links.numfiles-1;
	      links.toprow=0;
	      break;
	    }
	    
	  links.cursor=page_height-1;
	  links.toprow=links.numfiles-page_height;
	  break;
	  
	case KEY_PPAGE:

	  if (links.toprow<page_height)
	    {
	      links.cursor=0;
	      links.toprow=0;
	    }
	  else
	    {
	      links.toprow=links.toprow-page_height;
	    }
	  break;

	case KEY_UP: 
	  if (links.cursor==0)
	    {
	      if (links.toprow>0)
	      links.toprow--;
	      
	    } else {
	  links.cursor--;
	  }
	  break;

	case KEY_DOWN:
	  if (links.cursor+1==links.numfiles)
	    {
	      break;
	    }
	  if (links.cursor<(page_height-1))
	    {
	      links.cursor++;
	      break;
	    }
      
	  if (links.cursor==(page_height-1))
	    {
	      if ((links.toprow+page_height-1)<links.numfiles-1)
		{
		  links.toprow++;
		  break;
		}

	    }
	  break;
	}
   
      /*    wresize(win1,h,b/2);
      wresize(win2,h,b/2);
      mvwin(win2,0,b/2);
    
      wclear(win1);
      wclear(win2);
      */
      draw(links.toprow);
    
      refresh();
      wrefresh(win1);
      //    wrefresh(win2);

      //printf("%d %d ",b,h);
      //  z++;
    
    } 

  return(0);
}
