#include "mypanel.h"



void  mypanel_nav_home(MyPanel *panel)
{
  panel->toprow=0;
  panel->cursor=0;
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
