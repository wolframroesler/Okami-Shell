/********************************************************************

	     Mikro-Shell: Startprogramm fuer Okami-Shell



		  von Wolfram Rîsler 1.11.89

		Usenet: wr@bara.oche.de
		Mausnetz: Wolfram Roesler @ AC2

			~~~ FREEWARE ~~~


  énderungen:
  
18.02.90 Optional öbergabe der Konfigurationsdatei
01.03.90 Internes Kommando: echo
31.03.90 öbergabe mehrerer Konfigurationsdateien
	 internes Kommando: incl
10.05.90 LÑnge des Para-Strings in Pexec korrekt
	 Zeilenumbruch mit \ am Ende einer Zeile
28.12.90 Kommandozeile mit -c und Kommandos
	 internes Kommando: ver
05.03.91 interne Kommandos exit und wait
19.06.91 1.4: Steuersequenzen in echo
19.10.91 1.5: Anpassung an Ansi-C (PureC), Option -v fÅr Versionsnummer
28.01.92 1.6: Kommando "exec", MiNT-UnterstÅtzung
20.08.92      wait-Bug korrigiert
30.08.92 1.7: neues Kommando "cd"

*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <tos.h>

/* Das folgende auf 1 setzen, wenn Mint unterstÅtzt werden soll:
** (der einzige Grund, es nicht auf 1 zu setzen, ist daû man
** kein mintbind.h hat)
*/
#define USEMINT 1

#if USEMINT
#include <mintbind.h>
#endif

#define strlast(S)	(*(S+strlen(S)-1))	/* String: letztes char.*/

/* Strings mit Programmname und Versionsnummer				*/

#define TPName		"Okami Microshell"
#define TPVersion	"1.7"

/* Weitere globale Variablen und defines */
 
const char KONFFILE[]=	"msh.inf";	/* Default bei argc==1		*/
#define CMDLEN		(3*80)
#define ARGV0		"msh"		/* eigentlich: argv[0]		*/
#define EXITCODE	(-999)		/* Script beenden		*/

int MsgFlag=1;				/* Flag fÅr Meldungen		*/
int MintThere=0;			/* Flag: Mint ist installiert	*/

/* Prototypen */
int	msh	(char FName[]);
int	DoCom	(char *Para);
void	Xputs	(register char *s);
void	DoExec	(char *Cmd);
int	DoWait	(char *Para);

/************************************************************************/

int main(int argc,char *argv[])
{
  register int i;
  short ExFlag=0;			/* 0: Script, 1: Kommando	*/
  int Erg;
  int DoFlag=0;

#if USEMINT
  /* Herausfinden, ob Mint intalliert ist. Das geht durch Durchsuchen
  ** des Cookiejars nach dem MiNT-Eintrag.
  */
  {
    register struct C {long tag,val;} *c;
    long s;

    s = Super(0L);
    c = *(struct C**)0x5a0;
    Super((void*)s);

    if (c)
      for(;c->tag;c++)
        if (c->tag == 'MiNT')
        {
          MintThere = 1;
          break;
        }
  }
#endif
  
  if (argc>1)
  {
    if (argc==2 && !strcmp(argv[1],"-v"))
    {
      DoCom("ver");
      return 0;
    }
    for (i=1;i<argc;i++)
    {
      if (argv[i][0]=='\0')	/* ja, das _kommt_ vor */
        continue;

      if (!strcmp(argv[i],"-c"))
      {
	ExFlag=1;
	continue;
      }
      if (ExFlag)
	Erg=DoCom(argv[i]);
      else 
	Erg=msh(argv[i]);
      DoFlag=1;
    }
  }

  if (!DoFlag)
    Erg=msh(KONFFILE);

  return Erg;
}

/************************************************************************

	msh: ein MSH-Script ausfÅhren

************************************************************************/
int msh(char FName[])
{
  FILE *FPtr;				/* File-Ptr. Konf.datei 	*/
  char	St[CMDLEN+1];			/* eingelesene Zeile		*/
  char	Para2[CMDLEN+1];		/* Hilfsstring			*/
  register char *Para;
  char c;
  int Erg;
  
  while (isspace(*FName)) FName++;

  if ((FPtr=fopen(FName,"r"))==NULL)
  {
    fputs(ARGV0,stdout);
    fputs(": cannot open ",stdout);
    puts(FName);
     return -1;
  }
 
  while (fgets(St,3*80,FPtr)!=NULL)
  {
    Para=St;

    while (isspace(*Para)) Para++;	/* fÅhrende Leerzeichen	      */

    if (*Para=='\0' || *Para=='\n' || *Para=='#')
      continue;

    for (;;)				/* endende Leerzeichen		*/
    {
      c=strlast(Para);
      if (isspace(c))
	strlast(Para)='\0';
      else
	break;
    }
	
    while(strlast(Para)=='\\')		/* \ am Ende: Zeilenumbruch	*/
    {
      strlast(Para)='\0';
      fgets(Para2,80,FPtr);
      while (Para2[strlen(Para2)-1]=='\n')
	Para2[strlen(Para2)-1]='\0';
      strcat(Para,Para2);
    }

    /* Kommando ausfÅhren */

    Erg=DoCom(St);
    if (Erg==EXITCODE)
      break;	/* Script beenden */
  }

  fclose(FPtr);
  return Erg;
}

/************************************************************************

	DoCom: ein MSH-Kommando ausfÅhren

Return: EXITCODE: aktuelles Script beenden
	sonst: Returncode
************************************************************************/
int DoCom(char *Para)
{
  char	*Com;
  char Para2[CMDLEN+1];			/* Hilfsstring			*/
  int Ret=0;				/* RÅckgabewert			*/

  if (Para[0]=='-')			/* -: Meldung abschalten	*/
  {
    MsgFlag=0;
    return 0;
  }
  if (Para[0]=='+')			/* +: Meldung einschalten	*/
  {
    MsgFlag=1;
    return 0;
  }

  for (Com=Para;*Para && !isspace(*Para);Para++);
  if (*Para!='\0')
    *Para++='\0';

  if (!strcmp(Com,"cd"))			/* internes Kommando: cd	*/
  {
    char *Dir = Para;
    if (Dir[1]==':')
    {
      Dsetdrv(toupper(Dir[0])-'A');
      Dir+=2;
    }
    if (Dsetpath(Dir)!=0)
      printf("cannot cd to %s\n",Para);
  }
   
  else if (!strcmp(Com,"echo"))		/* internes Kommando: echo	*/
    Xputs(Para);

  else if (!strcmp(Com,"exit"))		/* internes Kommando: exit	*/
    return EXITCODE;
      
  else if (!strcmp(Com,"incl"))		/* internes Kommando: incl	*/
    msh(Para);

  else if (!strcmp(Com,"exec"))		/* internes Kommando: exec	*/
    DoExec(Para);

  else if (!strcmp(Com,"ver"))		/* internes Kommando: ver	*/
  {
    puts(TPName " " TPVersion);
    puts("compiled " __DATE__ " " __TIME__);
  }

  else if (!strcmp(Com,"wait"))		/* internes Kommando: wait	*/
    Ret = DoWait(Para);
    
  else				/* Programm starten */
  {
    /* Kommentarmodus: Kommando ausgeben */
    if (MsgFlag)
    {
      fputs(Com,stdout);
      fputc(' ',stdout);
      puts(Para);
    }
    
    Para2[0]=(char)strlen(Para);
    strcpy(Para2+1,Para);
    if ((Ret=(int)Pexec(0,Com,Para2,0L))<0)
    if (MsgFlag)
    {
      fputs(Com,stdout);
      puts(": not found");
    }
  }

  return Ret;
}


/************************************************************************

	Xputs: wie puts, aber interpretiert Escape-Sequenzen mit \ und ^

************************************************************************/
void Xputs(register char *s)
{
  for(;*s;s++)
  {
    switch(*s)
    {
      case '\\':
        s++;
        switch(*s)
	{
	  case '\0':
	    return;
	  case 'n':
	    fputc('\n',stdout);
	    break;
	  case 't':
	    fputc('\t',stdout);
	    break;
	  case 'b':
	    fputc('\b',stdout);
	    break;
	  case 'c':
	    break;
	  default:
	    fputc(*s,stdout);
	}
	break;
      case '^':
        s++;
	switch(*s)
	{
	  case '\0':
	    return;
	  default:
	    fputc(*s-'A'+1,stdout);
	}
        break;
      default:
        fputc(*s,stdout);
    }
  }

  if (!(s[-1]=='c' && s[-2]=='\\'))
    fputc('\n',stdout);
}

/************************************************************************

	DoExec: das interne Kommando exec
	Pexec und dann exit. Hat natÅrlich nur Sinn mit MiNT.

*************************************************************************/
void DoExec(char *Cmd)
{
  char *Arg;
  char S[130];
  int Mode = MintThere ? 200 : 0;

  while(isspace(*Cmd)) Cmd++;

  if (MsgFlag)
    puts(Cmd);

  for(Arg=Cmd;*Arg && !isspace(*Arg);Arg++);
  if (*Arg)
    *Arg++='\0';

  strncpy(S+1,Arg,128);
  S[0] = (char)strlen(Arg);

  if (Pexec(Mode,Cmd,S,0L)<0)
  {
    fputs(Cmd,stdout);
    puts(": not found");
  }
  else
    exit(0);
  return;
}

/************************************************************************

	DoWait: das interne Kommando wait

	Syntax: wait ((ttmmmjj|*) HHMMSS|key)

Return: EXITCODE: User hat abgebrochen, sonst: Zeit ist erreicht

*************************************************************************/
int DoWait(char *Para)
{
  void scan(char **Ptr,int *adr);
  time_t waitend;
  struct tm Date;

  while(isspace(*Para)) Para++;

  /* Worauf warten? */
  if (!strcmp(Para,"key"))	/* Tastendruck */
  {
    if ((Cconin()&0xff)==0x1b)
      return EXITCODE;
    else
      return 0;
  }

  /* ab hier: auf Datum und Uhrzeit warten */

  /* Mit aktuellem Tag initialisieren */
  {
    time_t t = time(NULL);
    Date = *localtime(&t);
  }

  /* Datum beachten? */
  if (*Para=='*') /* nein */
    while(isspace(*++Para));
  else
  {
    scan(&Para,&Date.tm_mday);
    scan(&Para,&Date.tm_mon);
    Date.tm_mon--;
    scan(&Para,&Date.tm_year);
    Para++; /* Leerzeichen */
  } 

  /* Uhrzeit scannen */
  scan(&Para,&Date.tm_hour);
  scan(&Para,&Date.tm_min);
  scan(&Para,&Date.tm_sec);
  if (*Para) Para++;

  /* Umformen in long-Format */
  waitend = mktime(&Date);
  
  if (MsgFlag)
  {
    fputs("Waiting",stdout);
    if (*Para)
    {
      fputs(" for ",stdout);
      fputs(Para,stdout);
    }
    puts("... press ESC to abort");
  }

  /* Warteschleife... */
  for(;;)
  {
#if 1
    printf("Jetzt: %lu, warte auf %lu: %s",time(NULL),waitend,ctime(&waitend));
#endif
    if (waitend==time(NULL))
      return 0;
  
    /* ESC gedrÅckt? */
    if (Cconis())
      if ((Cnecin()&0xff) == 0x1b)
        return EXITCODE;
  }
}

/* entspricht sscanf(*Ptr,"%02d",adr);(*Ptr)+=2 */
void scan(char **Ptr,int *adr)
{
  *adr = 10*((*(*Ptr)++)-'0');
  *adr +=    (*(*Ptr)++)-'0';
}
