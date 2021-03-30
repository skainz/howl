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


void _mypanel_free(MyPanel *panel)
{
  int lp;
  for (lp=0;lp<panel->numfiles;lp++)
    {
      free(panel->zeilen[lp].statbuf);
      free(panel->zeilen[lp].name);
      
    }
  free(panel->zeilen);
  
}



    

int b,h,ch;
MyPanel links,rechts;
int currentlines,currentcols;
  char SBUF[PATH_MAX];

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
  
  //get dimensions of left window
  int h,b;
  getmaxyx(panel->window, h, b);

  int page_height=h-2;

  int lines=panel->numfiles;

  if (page_height<panel->numfiles) {lines=page_height;}

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
      

      snprintf(SBUF,b," %s",panel->zeilen[j+start].name);

      int f;
      for (f=0;f<b;f++) {strncat(SBUF," ",PATH_MAX-1);}

      mbstowcs(WBUF,SBUF,PATH_MAX);
    
        mvwaddnwstr(panel->window,(j+1),(2),(WBUF),b-4);

        wclrtoeol(panel->window);
      wattrset(panel->window,COLOR_PAIR(2));
  
    }

  box(panel->window,0,0);
  
  
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
  // links.cwd="/home/skainz";
  // links.numfiles=ls(&links.zeilen,links.cwd/*"/usr/include"*/);

  //rechts.cwd="/usr/share/doc";
  //rechts.numfiles=ls(&rechts.zeilen,rechts.cwd);



  _mypanel_cd(&links,"/home/skainz");
  _mypanel_cd (&rechts,"/usr/include");
  
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


  
  int page_height;
  char newdir[PATH_MAX];
	
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


      	  int selected=currentp->toprow+currentp->cursor;

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

	case 10:

	  // todo:
	  // zeilen freigeben
	  // meta infos freigeben
	  // ins verzeichnis wechseln
	  // dir listing holen
	  // cursor reset
	  // toprow reset
	  //  sortieren
	  // panel refresh
	  
	  /*chdir(links.cwd);

	  getcwd(cwd, sizeof(cwd));

	  links.cwd=cwd;
	  	  strcat(links.cwd,"/");
	  strcat(links.cwd,currentp->zeilen[selected].name);
	  */
	  //	  printf("CD: %s \n",links.cwd);
	  //	  	  endwin();
	  //	  printf ("CHDIR: %s\n",currentp->zeilen[selected].name);
	  //	  sleep(5);
	  //  snprintf(SBUF,b,"CHDIR %s\n",currentp->zeilen[selected].name);
	    //	    mvwaddstr(currentp->window,h-5,b-strlen(SBUF)-1,SBUF);

	  if (strcmp(currentp->zeilen[selected].name,"../")==0)
	    {
	      //	      if (strlen(currentp->prev_dir)) {
		//	printf("Back to %s\n",currentp->prev_dir);sleep(2);
	      //}
	      //strip last part of dir
	      char topdir[PATH_MAX];
	      strcpy(topdir,currentp->cwd);
	      char* ptr=strrchr(topdir,'/');
	      	      *ptr='\0';
	      //	      int stripsize=strlen(ptr);
	      printf("nd: %s\n",topdir);//currentp->cwd);
	      
	      //    sleep(2);
	      //printf("dirp\n");exit(0);
	      _mypanel_free(currentp);
	      _mypanel_cd(currentp,topdir);
	    } else
	    {
	      strcpy(newdir,currentp->cwd);
	      strcat(newdir,"/");
	      strcpy(currentp->prev_dir,currentp->zeilen[selected].name);;
	      strncat(newdir,currentp->zeilen[selected].name,strlen(currentp->zeilen[selected].name));
	      
	      if (newdir[strlen(newdir)-1]=='/') //strip trailing /
		{
		  newdir[strlen(newdir)-1]='\0';
		}
	      
	      _mypanel_free(currentp);
	      _mypanel_cd(currentp,newdir);
	    }
	  
	  /*  links.numfiles=ls(&links.zeilen,links.cwd);
	   links.cursor=0;
	   links.toprow=0;
	  */
	   wclear(currentp->window);
	  _mypanel_sort_dirs_top(currentp);

	  // set cursor on directory entry we came from
	  int lastpos=_mypanel_find(currentp,currentp->prev_dir);
	  if (lastpos>-1)
	    {
	      currentp->cursor=lastpos;
	    }


	  break;
	}

      

      draw(currentp,currentp->toprow);

      refresh();
      wrefresh(links.window);
      wrefresh(rechts.window);

    } 

  return(0);
}
