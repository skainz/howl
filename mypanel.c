#include "mypanel.h"


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
