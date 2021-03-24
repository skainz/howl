#include <curses.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "mypanel.h"
#include <sys/stat.h>


#define BUFLEN  PATH_MAX
static wchar_t WBUF[BUFLEN];

//extern int mvwaddnwstr (WINDOW *, int, int, const wchar_t *, int);

struct termios orig_termios;


void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}


void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}




int  ls(Row **rowsp,char* path)
{
  DIR *dp;
  struct dirent *ep;
  Row *rows;
  int n=0; // NUmber of files in current dir
  int i; //index pointer
     struct stat statbuf; 
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
  chdir(path);
  while ((ep = readdir(dp)))
    {
      if (strcmp(".",ep->d_name)==0) continue;
      rows[i].name=malloc(strlen(ep->d_name)+2);
      strcpy(rows[i].name,ep->d_name);
      //      printf("%ld\n",sizeof(struct stat));
      rows[i].statbuf=malloc(sizeof(struct stat));
      char* fullpath=path;
      //      strcat(fullpath,path);
      int statrv=stat(rows[i].name,rows[i].statbuf);
      if (statrv == -1) { printf("error stating %s, dir %s,fullpath %s\n",ep->d_name,path,fullpath);exit (1);}
       if (S_ISDIR(rows[i].statbuf->st_mode)) {
            strcat(rows[i].name, "/");
       }

      i++;
      //     printf("%d %s\n",i,ep->d_name);
    }

  closedir(dp);
  *rowsp=rows;
  return i;
}

    

int b,h;
//int files;
//int cursor=0;
MyPanel links,rechts;
  int ch;

int currentlines,currentcols;
void quit(void)
{
  delwin(links.window);
    delwin(rechts.window);
  endwin();
  //    disableRawMode();

}

void draw(MyPanel * panel, int start)
{
  
  int j;
  char SBUF[PATH_MAX];
  
  //get dimensions of left window
  int h,b;
  getmaxyx(panel->window, h, b);
  //  printf("fenster ist %d hoch\n",h);
  int page_height=h-2;
  //  int start=2;
  int lines=panel->numfiles;

  if (page_height<panel->numfiles) {lines=page_height;}
  //  printf("%d zeilen hoch\n",page_height);
  //  printf("drawing %d lines\n",lines);
  //exit(1);
  for (j=0;j<lines; j++)
    {
      if (  S_ISDIR(panel->zeilen[j+start].statbuf->st_mode))
	{
	  wattron(panel->window,A_BOLD);
	} else
	{
	  wattroff(panel->window,A_BOLD);
	}


	    if (panel->zeilen[j+start].marked)
	{
	  wattron(panel->window,A_STANDOUT);
	  //	  wattron(panel->window,COLOR_PAIR(4));
	} else
	{
	  //  wattroff(panel->window,A_STANDOUT);
	
      
      if (j==panel->cursor)
	{
      wattrset(panel->window,COLOR_PAIR(1));

      
	} else
	{
	  //    wattrset(panel->window,COLOR_PAIR(2));
	  
	}
      // wattron(panel->window,COLOR_PAIR(4));
	}
      
      //   wattron(COLOR_PAIR(4));
      snprintf(SBUF,b," %s",panel->zeilen[j+start].name);
      //,panel->zeilen[j+start].marked);
      int f;
      int z=strlen(SBUF);
      for (f=0;f<b;f++) {strncat(SBUF," ",PATH_MAX);}
      //      strncat(SBUF," xxxxx",PATH_MAX);
      mbstowcs(WBUF,SBUF/*panel->zeilen[j+start].name*/,PATH_MAX);
    
  //   mbstowcs(WBUF,panel->zeilen[j+start].name,PATH_MAX);
      mvwaddnwstr(panel->window,(j+1),(2),(WBUF),b-4);

      //      mvwaddstr(win1,j+1,2+strlen(zeilen[j+start].name)+1,"xxxx");
	    //      mvwhline(win1, j+1, 2+strlen(zeilen[j+start].name), 'x', 5);
      //      wchgat(win1,j+1,10,COLOR_PAIR(1));
      //  whline(win1,"-",20-strlen(WBUF));
      //      wattrset(panel->window,COLOR_PAIR(4));
      //      mvwchgat(panel->window,j+1,30,1,A_REVERSE,COLOR_PAIR(4),0);
      wclrtoeol(panel->window);
      wattrset(panel->window,COLOR_PAIR(2));
      //      wchgat(panel->window,);
      //      mvwaddnstr(win1, j + 1, 2, zeilen[j].name,10);
    }
  //   mvwaddstr(win1, 3, 3, "Hallo win");
  // mvwaddstr(win2, 7, 3, "Diese Zeichenkette wird nicht angezeigt!"); 
  // da ausserhalb des win-Anzeigebereichs
  box(panel->window,0,0);
  //  box(win2,0,0);
  //    mvwchgat(win1,0, 0, 1, A_REVERSE, 1, NULL);	
  //    files=files*files;
  snprintf(SBUF,b," %d Dateien ",panel->numfiles);
  mvwaddstr(panel->window,h-1,b-strlen(SBUF)-1,SBUF);
  wattron(panel->window,COLOR_PAIR(5));
  wattron(panel->window,A_BOLD);
  snprintf(SBUF,b,"ph: %d, files %d, z:%d, cursor: %d Dateien ",page_height,panel->numfiles,start,panel->cursor);
  mvwaddstr(panel->window,h-1,b-strlen(SBUF)-1,SBUF);
  wattroff(panel->window,COLOR_PAIR(2));
  
  wattroff(panel->window,A_BOLD);
  snprintf(SBUF,b,"%d %s",ch,panel->cwd);
  mvwaddstr(panel->window,0,1,SBUF);

}


int main(void)
{


  MyPanel *currentp,*other,*b;
  links.cwd=".";
  links.numfiles=ls(&links.zeilen,links.cwd/*"/usr/include"*/);
    rechts.cwd="/usr/share/doc";
  rechts.numfiles=ls(&rechts.zeilen,rechts.cwd);

  _mypanel_sort_dirs_top(&links);
  _mypanel_sort_dirs_top(&rechts);

  // _mypanel_sort_dirs_top(&links);
  
  //  exit(0);

  // ende debug
  //  links.zeilen[0].marked=1;
    currentp=&links;
  other=&rechts;
    //  printf("anzahl: %d\n",links.numfiles);exit(0);

  setlocale(LC_ALL, "");

  /*printf("\n%d dateien\n",files);
    exit(0);
  */
  initscr();
  //  enableRawMode();
  atexit(quit);
  clear();
  noecho();
  curs_set(0);
      cbreak();
    //  nl();
    //    nodelay(stdscr,TRUE);
    keypad(stdscr, TRUE);
    //raw();
  start_color();
  init_pair(1, COLOR_BLUE, COLOR_GREEN);
  init_pair(2, COLOR_WHITE, COLOR_RED);
  init_pair(3, COLOR_RED, COLOR_MAGENTA);

  init_pair(4, COLOR_WHITE, COLOR_CYAN);
  init_pair(5,COLOR_WHITE,COLOR_BLUE);
  
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

  /*char c;

  int ismeta=0;
  
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
    {
      if (c==27)
	{
	  //Meta key pressed
	  ismeta=1;
	}
    printf("%d\n",c);
    }

  */

  
  int page_height;

  getmaxyx(stdscr,currentlines,currentcols);
  
  while((ch=getch()) != KEY_F(10))
    {
      getmaxyx((currentp->window), h, b);
      page_height=h-2;
      currentp->page_height=page_height;

      int nl,nc;
      getmaxyx(stdscr,nl,nc);
      if (!(currentlines==nl) || (!(currentcols==nc))) // Check if window was resized
	{
	  currentlines=nl;
	  currentcols=nc;
	  wclear(currentp->window);
	  wclear(other->window);
	  wresize(currentp->window,nl,nc/2);
	  wresize(other->window,nl,nc/2);
	  mvwin(other->window,0,nc/2);
	  mvwin(currentp->window,0,0);
	  draw(currentp,currentp->toprow);
	  draw(other,other->toprow);
	  
	}

      switch (ch)
	{
	case 331: //INSERT
	  mypanel_mark_toggle(currentp);
	  mypanel_nav_down(currentp);
	  //	  exit(0);
	  break;
	case 27:
	  exit(0);
	  break;
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
