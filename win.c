#include <ncursesw/curses.h>
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

  int page_height=h-2-2;

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
	
      
	  if (j==panel->cursor && panel->focus)
	    {
	      wattrset(panel->window,COLOR_PAIR(6));
     
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

  //display current path, highlight if active
  if (panel->focus)
    {  
    wattron(panel->window,COLOR_PAIR(7));
    }
  
  snprintf(SBUF,b,"%d %s",ch,panel->cwd);
  mvwaddstr(panel->window,0,1,SBUF);
  wattroff(panel->window,COLOR_PAIR(7));

  // lower box, show current selected cursor entry
    mvwhline(panel->window,h-3,1,0,b-2);
    mvwaddch(panel->window,h-3,0,ACS_LTEE);
    mvwaddch(panel->window,h-3,b-1,ACS_RTEE);

    SBUF[0]=0;

    int selected=panel->toprow+panel->cursor;
    strcpy(SBUF,panel->zeilen[selected].name);
    strcat(SBUF," ");
    snprintf(SBUF,b," %s %d",panel->zeilen[selected].name,panel->zeilen[selected].statbuf->st_size);
    b=20;
    for (int f=0;f<b;f++) {strncat(SBUF," ",b-1);}

    
    mvwaddstr(panel->window,h-2,1,SBUF);

	// #define WACS_LTEE	WACS_SSSB

    
}

void setTitle(char* title)
{
  //  printf("\033]0;%s\a",title);
  printf("\033]2;%s\007", title);
}

void setClip(char* title)
{
  //  printf("\033]0;%s\a",title);
  printf("\033]52;c;%s\x07", title);
}


int main(void)
{
    setTitle("!lolz!");
    setClip("!lolz!");

  // fprintf (stdout, "\33]0;%s\7", "lolr");
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
    links.focus=true;
    rechts.focus=false;
  other=&rechts;
    //  printf("anzahl: %d\n",links.numfiles);exit(0);

  setlocale(LC_ALL, "");

  /*printf("\n%d dateien\n",files);
    exit(0);
  */
  initscr();
  raw();
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
  init_pair(2, COLOR_WHITE, COLOR_BLUE);
  init_pair(3, COLOR_RED, COLOR_MAGENTA);

  init_pair(4, COLOR_WHITE, COLOR_CYAN);
  init_pair(5,COLOR_WHITE,COLOR_BLUE);

  init_pair(6,COLOR_BLACK,COLOR_CYAN); //CURSOR
  init_pair(7,COLOR_BLACK,COLOR_WHITE);
  links.window = newwin(LINES-1, COLS/2, 0, 0);
  rechts.window = newwin(LINES-1,COLS/2,0,COLS/2);
  //  bkgd(COLOR_PAIR(1));
    wbkgd(links.window, COLOR_PAIR(2));
  wbkgd(rechts.window, COLOR_PAIR(2));

  draw(&links,0);
  draw(&rechts,0);
  draw(currentp,0);
  //  mvaddstr(5,5, "Hallo stdscr");

  
  refresh();
  wrefresh(links.window);
  wrefresh(rechts.window);


  
  int page_height,fpos;
  char newdir[PATH_MAX];
	
  getmaxyx(stdscr,currentlines,currentcols);
  
  while((ch=getch()) != KEY_F(10))
    {
      printf("%d\n",ch);//sleep(1);
      getmaxyx((currentp->window), h, b);
      page_height=h-2-2;
      currentp->page_height=page_height;

      int nl,nc;
      getmaxyx(stdscr,nl,nc);
      if (!(currentlines==nl) || (!(currentcols==nc))) // Check if window was resized
	{
	  currentlines=nl;
	  currentcols=nc;
	  wclear(currentp->window);
	  wclear(other->window);
	  wresize(currentp->window,nl-1,nc/2);
	  wresize(other->window,nl-1,nc/2);
	  mvwin(other->window,0,nc/2);
	  mvwin(currentp->window,0,0);
	  draw(currentp,currentp->toprow);
	  draw(other,other->toprow);
	  
	}


      	  int selected=currentp->toprow+currentp->cursor;
	  bool refresh_all=false;
      switch (ch)
	{
	case 331: //INSERT
	  mypanel_mark_toggle(currentp);
	  mypanel_nav_down(currentp);
	  //	  exit(0);
	  break;
	case 9:
	  	  currentp->focus=false;
		  other->focus=true;
		  refresh_all=true;
	  // TAB,focus other window
	  // TODO: Mark new window active/current window inactive
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

	case 19:
	  fpos=_mypanel_find(currentp,"apt.gpg");

	  if (fpos >-1)
	    {
	      currentp->cursor=fpos;
	      mypanel_scroll_center(currentp,fpos);
	    }
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

	  // check, ob current entry ein directory ist

	  if (!(S_ISDIR(currentp->zeilen[selected].statbuf->st_mode)))
	    {
	      
	      //	       printf("is not dir!\n");sleep(1);
	      char cmdline[PATH_MAX];
	      cmdline[0]=0;
	      chdir(currentp->cwd);
	      strcat(cmdline,"xdg-open ");
	      //	      strcat(cmdline,currentp->cwd);
	      //	      strcat(cmdline,"/");
	      strcat(cmdline,"\"");
	      strcat(cmdline,currentp->zeilen[selected].name);
	      strcat(cmdline,"\"");
	      strcat(cmdline," &");
	      
	      
	      //	      	      printf("EXECUTING %s\n",cmdline);sleep(2);
		      	      system(cmdline);
	      // 	      system()
	      break;
	      
	    }
	  
	  // going one dir up
	  if (strcmp(currentp->zeilen[selected].name,"../")==0)
	    {
	      //	      if (strlen(currentp->prev_dir)) {
		//	printf("Back to %s\n",currentp->prev_dir);sleep(2);
	      //}
	      //strip last part of dir
	      char topdir[PATH_MAX];
	      char savedir[PATH_MAX];
	      strcpy(savedir,currentp->cwd);
	       strcpy(topdir,currentp->cwd);
	      char* ptr=strrchr(topdir,'/');
	      	      *ptr='\0';
	      //	      int stripsize=strlen(ptr);
	      //printf("nd: %s\n",topdir);//currentp->cwd);
	      
	      //    sleep(2);
	      //printf("dirp\n");exit(0);
		      if (strlen(topdir)==0)
			{
			  topdir[0]='/'; topdir[1]=0;
			}

	      _mypanel_free(currentp);
	      int retval=_mypanel_cd(currentp,topdir);

	      if (retval==-1) //chdir failed, go back to last dir
		{
		  //		  printf("error on cd, going to %s\n",savedir);sleep(2);
		  int retval=_mypanel_cd(currentp,savedir);
		  //todo: if this fails, report error
		  
		}

	    } else
	    {

	      char savedir[PATH_MAX];
	      strcpy(savedir,currentp->cwd);
	      
	      strcpy(newdir,currentp->cwd);
	      if (!(strcmp(newdir,"/")==0))
		{
	      strcat(newdir,"/");
		}
	      strcpy(currentp->prev_dir,currentp->zeilen[selected].name);;
	      strncat(newdir,currentp->zeilen[selected].name,strlen(currentp->zeilen[selected].name));
	      
	      if (newdir[strlen(newdir)-1]=='/') //strip trailing / on new path
		{
		  newdir[strlen(newdir)-1]='\0';
		}
	      
	      _mypanel_free(currentp); //free previous entry

	      int retval=_mypanel_cd(currentp,newdir);
	      if (retval==-1)
		{
		  int retval=_mypanel_cd(currentp,savedir);
		  //todo: if this fails, report error
		}
	    }
	  
	   wclear(currentp->window);
	  _mypanel_sort_dirs_top(currentp);

	  // set cursor on directory entry we came from
	  int lastpos=_mypanel_find(currentp,currentp->prev_dir);

	  if (lastpos>-1)
	    {
	      
	      currentp->cursor=lastpos;
	      if (currentp->cursor > currentp->page_height)
		{
		  mypanel_scroll_center(currentp,lastpos);
		}
	    }


	  break;
	}

      

      draw(currentp,currentp->toprow);
      if (refresh_all)
	
            draw(other,other->toprow);// -currentp->toprow);

      //       def_prog_mode();
      // refresh();
      
      // endwin();
      //fprintf (stdout, "\33]1;%s\7", "lod");
      //      sleep(2);
      // fflush(stdout);
      
refresh();
      
      wrefresh(links.window);
      wrefresh(rechts.window);
      //      	  setTitle("lol");
      
      
    } 

  return(0);
}
