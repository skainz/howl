#include "mypanel.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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


int  ls(Row **rowsp,char* path)
{
  DIR *dp;
  struct dirent *ep;
  Row *rows;
  int n=0; // Number of files in current dir
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
  chdir(path);
  while ((ep = readdir(dp)))
    {
      if (strcmp(".",ep->d_name)==0) continue;
      rows[i].name=malloc(strlen(ep->d_name)+2);
      strcpy(rows[i].name,ep->d_name);
      rows[i].statbuf=malloc(sizeof(struct stat));
      rows[i].marked=0;
      char* fullpath=path;
      int statrv=stat(rows[i].name,rows[i].statbuf);
      if (statrv == -1) { printf("error stating %s, dir %s,fullpath %s\n",ep->d_name,path,fullpath);exit (1);}
       if (S_ISDIR(rows[i].statbuf->st_mode)) {
            strcat(rows[i].name, "/");
       }

      i++;
    }

  closedir(dp);
  *rowsp=rows;
  return i;
}


int _mypanel_cd(MyPanel *panel,char* path)
{
  panel->cwd=path;
  int rv=chdir(panel->cwd);
  if (!(rv==0)) // chdir() failed, return 0;
    {
      return rv;
    }
  
  panel->numfiles=ls(&(panel->zeilen),panel->cwd);
  panel->cursor=0;
  panel->toprow=0;
  return 0;
}

