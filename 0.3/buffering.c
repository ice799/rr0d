/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id: buffering.c,v 1.22 2006-06-29 08:37:30 serpilliere Exp $
 *
 */

#include "vars.h"
#include "buffering.h"
#include "utils.h"
#include "video.h"

#define BUFFERMAXLINES 80



int infobuffer; /* =DESACTIVADO; */
int bufferindex=0;
int insertbufferindex=0;

char buff_cmd[BUFFERMAXCMD][80];
int insertcmdindex;
int cmdindex;


char buffering[BUFFERMAXLINES][80];       /* 200 lines and 80 chars */


/*nettoie le buffer de log                        */
/*                                                */
/*                                                */
void cleaningbuffer(void)
{
  int x,y=0;
  char *ptr;
  for (x=0;x<BUFFERMAXLINES;x++)
    {
      ptr=&buffering[x][0];
      for (y=0;y<80;y++)
	{*ptr=' ';ptr++;};
           
    }
}


/*insere une commande dans l historique           */
/*                                                */
void insert_cmd(char *ptr)
{
  insertcmdindex = insertcmdindex % BUFFERMAXCMD;
  strcpy(&buff_cmd[insertcmdindex][0], ptr ); 
  insertcmdindex = (insertcmdindex+1) % BUFFERMAXCMD;
  cmdindex =insertcmdindex;
}



void recall_oldcmd(int decal)
{
  unsigned int len,i;

  cmdindex = (cmdindex + decal + BUFFERMAXCMD) % BUFFERMAXCMD;	
  cmdindex = cmdindex % BUFFERMAXCMD;

  ClearCommand();

  len = rr0d_strlen(&buff_cmd[cmdindex][0]);
  len = len>50?50:len;

  for (i=0;i<len;i++)
    InsertCommandKey(buff_cmd[cmdindex][i]);

}


/*insere une ligne dans la file du buffer de log  */
/*                                                */
/*                                                */
void insertinbuffer(char *ptr)
{
  unsigned int i;
  char buf[]="                                                                                ";


  if (insertbufferindex>=BUFFERMAXLINES) 
    {
      for (i=0;i<BUFFERMAXLINES-1;i++)
	  rr0d_memcpy(&buffering[i][0], &buffering[i+1][0], 78);

      insertbufferindex-=1;
     
      for (;i<BUFFERMAXLINES;i++)
	  rr0d_memcpy(&buffering[i][0],&buf[0], 78);
     
    }

  strcpy(&buffering[insertbufferindex][0], ptr); 
  insertbufferindex++;
  
  bufferindex = (unsigned int)insertbufferindex<buff_win_dim?0 : insertbufferindex-buff_win_dim;
}


/*affiche le bufer de log dans la fenetre de log  */
/*                                                */
/*                                                */
void displaybuffer()
{
  unsigned int x;
  //  for (x=buff_win_y; x < buff_win_y + buff_win_dim;x++)
    //rr0d_memcpy(&table[x][1], &buffering[bufferindex+x-buff_win_y][0], 78);
  for (x=0; x < buff_win_dim;x++)
    print_AW("                                                                              ", x);


  for (x=0; x < buff_win_dim;x++)
    print_AW(&buffering[bufferindex+x][0], x);

}

/*descend d'une ligne dans le buffer de log       */
/*                                                */
/*                                                */
void bufferdown(void) 
{
  Refresh_AW();
  if ((bufferindex+buff_win_dim)<BUFFERMAXLINES) bufferindex++;

}

/*remonte d'une ligne dans le buffer de log       */
/*                                                */
/*                                                */
void bufferup(void)
{
  Refresh_AW();
  if (bufferindex>0) bufferindex--;

}

/*descend de 10 ligne dans le buffer de log       */
/*                                                */
/*                                                */
void bufferbigdown(void) 
{
  int i;
  for (i=0;i<10;i++) 
    bufferdown();
}

/*remonte de 10 ligne dans le buffer de log       */
/*                                                */
/*                                                */
void bufferbigup(void)
{
  int i;
  for (i=0;i<10;i++) 
    bufferup();
}


void startbuffer(void)
{
  ClearAW();
  cleaningbuffer();

  bufferindex=0;
  insertbufferindex=0;
}

void endbuffer(void)
{

}
 
void restorebuffer(void)
{

}


/*insere une ligne dans la file du buffer de log  */
/*                                                */
/*                                                */
void log_line_in_buffer(char *ptr)
{
  insertinbuffer(ptr);
  displaybuffer();
  Refresh_Display();  
}


