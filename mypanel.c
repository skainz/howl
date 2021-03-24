#include "mypanel.h"
#include <sys/stat.h>


void mypanel_nav_up(MyPanel *panel)
{
  if (panel->cursor==0)
    {
      if (panel->toprow>0)
	panel->toprow--;
      
    } else {
    panel->cursor--;
  }
  
}


void mypanel_nav_down(MyPanel *panel)
{
  if (panel->cursor+1==panel->numfiles)
    {
      return;
    }
  if (panel->cursor<(panel->page_height-1))
    {
      panel->cursor++;
      return;
    }
  
  if (panel->cursor==(panel->page_height-1))
    {
      if ((panel->toprow+panel->page_height-1)<panel->numfiles-1)
	{
	  panel->toprow++;
	  return;
	}
      
    }
  
}


void mypanel_nav_ppage(MyPanel *panel)
{
  if (panel->toprow<panel->page_height)
    {
      panel->cursor=0;
      panel->toprow=0;
    }
  else
    {
      panel->toprow=panel->toprow-panel->page_height;
    }
  
}


void mypanel_nav_npage(MyPanel *panel)
{
  if ((panel->toprow==0) && (panel->toprow+panel->page_height>(panel->numfiles-1)))
    {
      panel->cursor=panel->numfiles-1;
      panel->toprow=0;
      return;
    }
	  
  panel->toprow=panel->toprow+panel->page_height;
	  
  if ((panel->numfiles-panel->toprow)<panel->page_height)
    {
      panel->toprow=panel->numfiles-panel->page_height;
      panel->cursor=panel->page_height-1;
	      
      return;
    }
}	  


void  mypanel_nav_home(MyPanel *panel)
{
  panel->toprow=0;
  panel->cursor=0;
}


void mypanel_nav_end(MyPanel *panel)
{
  if ((panel->toprow==0) && (panel->toprow+panel->page_height>(panel->numfiles-1)))
    {
      panel->cursor=panel->numfiles-1;
      panel->toprow=0;
      return;
    }
	    
  panel->cursor=panel->page_height-1;
  panel->toprow=panel->numfiles-panel->page_height;
  return;
}


int mypanel_mark_on(MyPanel *panel)
{
  panel->zeilen[panel->toprow+panel->cursor].marked=1;
  return 0;
}

int mypanel_mark_off(MyPanel *panel)
{
  panel->zeilen[panel->toprow+panel->cursor].marked=0;
  return 0;
}

int mypanel_mark_toggle(MyPanel *panel)
{
  if (panel->zeilen[panel->toprow+panel->cursor].marked)
    {
  panel->zeilen[panel->toprow+panel->cursor].marked=0;
    }else
    {
  panel->zeilen[panel->toprow+panel->cursor].marked=1;

    }
  return 0;
}



int _mypanel_cmpfunc_name_asc(const void *a,const void *b)
{

  struct Row * a1 =(struct Row *)a;
  struct Row * b1 =(struct Row *)b;
  
  return strcmp(a1->name,b1->name);  
}

int _mypanel_cmpfunc_name_desc(const void *a,const void *b)
{

  struct Row * a1 =(struct Row *)a;
  struct Row * b1 =(struct Row *)b;
  
  return strcmp(b1->name,a1->name);  
}

int _mypanel_cmpfunc_dirs_top(const void *a,const void *b)
{
  struct Row * a1 =(struct Row *)a;
  struct Row * b1 =(struct Row *)b;

  struct stat * sba1=a1->statbuf;
  struct stat * sbb1=b1->statbuf;
  
  return (S_ISDIR(sba1->st_mode)<S_ISDIR(sbb1->st_mode));

}


int _mypanel_sort_dirs_top(MyPanel *panel)
{
  
    qsort (panel->zeilen,panel->numfiles,sizeof(Row),_mypanel_cmpfunc_dirs_top);
  // get number of directory entries
  int z=0;
  while ((z<(panel->numfiles)-1) && S_ISDIR(panel->zeilen[z].statbuf->st_mode)) z++;

  // find entry point for lsit of files
  Row* files=panel->zeilen;
  files=files+z;

  //sort directories 
  qsort(panel->zeilen,z,sizeof(Row),_mypanel_cmpfunc_name_asc);
  // sort filenames
 
  qsort(files,panel->numfiles-z-1,sizeof(Row),_mypanel_cmpfunc_name_asc);

}
