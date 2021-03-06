/*************************************************************************

	Aufruf von Accessories von der Okami-Shell aus

		von Wolfram R�sler

**************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <aes.h>

#define RSCFILE	"gem.rsc"

int main(void)
{
  OBJECT *Tree;
  int buf[8];
  char *Cmd;

  appl_init();

  /* Resourcefile laden. */
  if (rsrc_load(RSCFILE)==0)
  {
    form_alert(1,"[3][gem:|Resource file `" RSCFILE "' not found][  Pity  ]");
    appl_exit();
    return 1;
  }

  rsrc_gaddr(0,0,&Tree);

  for(;;)
  {
    menu_bar(Tree,1);

    /* Warten auf Men�klick... */
    evnt_mesag(buf);

    if (buf[0]==10)
      menu_tnormal(Tree,buf[3],1);

    menu_bar(Tree,0);

    if (buf[0]==10)
    {
      /* Kommando aus dem Objektbaum holen. */
      Cmd = Tree[buf[4]].ob_spec.free_string;
      while(isspace(*Cmd)) Cmd++;

      /* Test auf Abbruchbedingung. */
      if (strncmp(Cmd,"exit",4)==0)
        break;

      graf_mouse(256,0);
      system(Cmd);
      graf_mouse(257,0);
    }
  }
  
  rsrc_free();
  appl_exit();

  return 0;
}