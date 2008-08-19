/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id: video.c,v 1.46 2006-07-11 07:09:30 serpilliere Exp $
 *
 */

#include "vars.h"
#include "utils.h"

#include "page.h"
#include "video.h"


//unsigned int need_
unsigned int rasta_mode=0;
unsigned int fOOl_mode = 0;
unsigned int fOOl_mode_color_shift=0;

unsigned int cursor_X=0;
unsigned int cursor_Y=0;

unsigned int cursor_old_color=0;
unsigned int cursor_color='0';


#define COLORSET1 0x12
#define COLORSET2 0x50

char COLOR=COLORSET1;


int insline=40;


unsigned int data_win_y;
unsigned int data_win_dim;

unsigned int code_win_y;
unsigned int code_win_dim;

unsigned int buff_win_y;
unsigned int buff_win_dim;

unsigned int cmd_win_y;
unsigned int cmd_win_dim;

unsigned int screen_offset_x;
unsigned int screen_offset_y;

unsigned int screen_bpp;

unsigned int code_dim_line[10];


unsigned int font_width;
unsigned int font_height;

unsigned int font_x;
unsigned int font_y;
unsigned int font_bpp;

unsigned int screen_adresse ;

unsigned char dump_screen_data[dump_screen_size];

char  screen_line0[80];
char  screen_line1[80];
char  screen_line2[80];
char  screen_line3[80];
char  screen_line4[80];
char  screen_line5[80];
char  screen_line6[80];
char  screen_line7[80];
char  screen_line8[80];
char  screen_line9[80];

char  screen_line10[80];
char  screen_line11[80];
char  screen_line12[80];
char  screen_line13[80];
char  screen_line14[80];
char  screen_line15[80];
char  screen_line16[80];
char  screen_line17[80];
char  screen_line18[80];
char  screen_line19[80];

char  screen_line20[80];
char  screen_line21[80];
char  screen_line22[80];
char  screen_line23[80];
char  screen_line24[80];
char  screen_line25[80];
char  screen_line26[80];
char  screen_line27[80];
char  screen_line28[80];
char  screen_line29[80];

char  screen_line30[80];
char  screen_line31[80];
char  screen_line32[80];
char  screen_line33[80];
char  screen_line34[80];
char  screen_line35[80];
char  screen_line36[80];
char  screen_line37[80];
char  screen_line38[80];
char  screen_line39[80];

char *table[]={&screen_line0[0],&screen_line1[0],&screen_line2[0],&screen_line3[0],&screen_line4[0],
               &screen_line5[0],&screen_line6[0],&screen_line7[0],&screen_line8[0],&screen_line9[0],
               &screen_line10[0],&screen_line11[0],&screen_line12[0],&screen_line13[0],&screen_line14[0],
               &screen_line15[0],&screen_line16[0],&screen_line17[0],&screen_line18[0],&screen_line19[0],
               &screen_line20[0],&screen_line21[0],&screen_line22[0],&screen_line23[0],&screen_line24[0],
               &screen_line25[0],&screen_line26[0],&screen_line27[0],&screen_line28[0],&screen_line29[0],
	       &screen_line30[0],&screen_line31[0],&screen_line32[0],&screen_line33[0],&screen_line34[0],
               &screen_line35[0],&screen_line36[0],&screen_line37[0],&screen_line38[0],&screen_line39[0],
};


char  screen_color0[80];
char  screen_color1[80];
char  screen_color2[80];
char  screen_color3[80];
char  screen_color4[80];
char  screen_color5[80];
char  screen_color6[80];
char  screen_color7[80];
char  screen_color8[80];
char  screen_color9[80];
char screen_color10[80];
char screen_color11[80];
char screen_color12[80];
char screen_color13[80];
char screen_color14[80];
char screen_color15[80];
char screen_color16[80];
char screen_color17[80];
char screen_color18[80];
char screen_color19[80];
char screen_color20[80];
char screen_color21[80];
char screen_color22[80];
char screen_color23[80];
char screen_color24[80];
char screen_color25[80];
char screen_color26[80];
char screen_color27[80];
char screen_color28[80];
char screen_color29[80];
char screen_color30[80];
char screen_color31[80];
char screen_color32[80];
char screen_color33[80];
char screen_color34[80];
char screen_color35[80];
char screen_color36[80];
char screen_color37[80];
char screen_color38[80];
char screen_color39[80];

char *table_color[]={
  &screen_color0[0],&screen_color1[0],&screen_color2[0],&screen_color3[0],&screen_color4[0],
  &screen_color5[0],&screen_color6[0],&screen_color7[0],&screen_color8[0],&screen_color9[0],
  &screen_color10[0],&screen_color11[0],&screen_color12[0],&screen_color13[0],&screen_color14[0],
  &screen_color15[0],&screen_color16[0],&screen_color17[0],&screen_color18[0],&screen_color19[0],
  &screen_color20[0],&screen_color21[0],&screen_color22[0],&screen_color23[0],&screen_color24[0],
  &screen_color25[0],&screen_color26[0],&screen_color27[0],&screen_color28[0],&screen_color29[0],
  &screen_color30[0],&screen_color31[0],&screen_color32[0],&screen_color33[0],&screen_color34[0],
  &screen_color35[0],&screen_color36[0],&screen_color37[0],&screen_color38[0],&screen_color39[0],
};


int accessline[SCREEN_LINE];

int CommandLength=0;
char Command[256];

int test()
{
  //	Out_Debug_String("(_.-POUET-._)!\n\r");



  return 0;
}





void prepare_screen(void)
{
  unsigned int i, ii;

  for (i=0;i<cmd_win_y+cmd_win_dim;i++)
    for (ii=0;ii<80;ii++)
    {
      table[i][ii]=' ';
      table_color[i][ii]=' ';
    }

  for (i=0;i<cmd_win_y/*+cmd_win_dim*/;i++)
    {
      table[i][0] = '³';   table_color[i][0] = '1';
      table[i][79] = '³';      table_color[i][79] = '1';
    }

  for (i=0;i<80;i++)
    {
      table[0][i] = 'Ä';
      table_color[0][i] = '1';

      table[data_win_y-1][i] = 'Ä';
      table_color[data_win_y-1][i] = '1';

      table[code_win_y-1][i] = 'Ä';
      table_color[code_win_y-1][i] = '1';

      table[buff_win_y-1][i] = 'Ä';
      table_color[buff_win_y-1][i] = '1';

      table[cmd_win_y-1][i] = 'Ä';
      table_color[cmd_win_y-1][i] = '1';

    }


  table[0][0] = 'Ú';
  table[0][79] = '¿';


  table[data_win_y-1][0] = 'Ã';
  table[data_win_y-1][79] = '´';
  
  table[code_win_y-1][0] = 'Ã';
  table[code_win_y-1][79] = '´';
  
  table[buff_win_y-1][0] = 'Ã';
  table[buff_win_y-1][79] = '´';
  
  table[cmd_win_y-1][0] = 'À';//'Ã';
  table[cmd_win_y-1][79] = 'Ù';//'´';


  rr0d_memcpy(&table[1][1],  "EAX=", 4);
  rr0d_memcpy(&table[1][14], "EBX=", 4);
  rr0d_memcpy(&table[1][27], "ECX=", 4);
  rr0d_memcpy(&table[1][40], "EDX=", 4);
  rr0d_memcpy(&table[1][53], "EDI=", 4);

  rr0d_memcpy(&table[2][1],  "ESI=", 4);
  rr0d_memcpy(&table[2][14], "EBP=", 4);
  rr0d_memcpy(&table[2][27], "ESP=", 4);
  rr0d_memcpy(&table[2][40], "EIP=", 4);

  rr0d_memcpy(&table[3][1],  "CS =", 4);
  rr0d_memcpy(&table[3][14], "DS =", 4);
  rr0d_memcpy(&table[3][27], "ES =", 4);
  rr0d_memcpy(&table[3][40], "SS =", 4);
  rr0d_memcpy(&table[3][53], "FS =", 4);
  rr0d_memcpy(&table[3][66], "GS =", 4);



  for (i=1;i<14;i++)
    {
      for (ii=0;ii<data_win_dim;ii++)
	table_color[data_win_y+ii][i] = '2';
      for (ii=0;ii<code_win_dim;ii++)
	table_color[code_win_y+ii][i] = '2';

      if (i==4)
	i++;
    }


  for (i=0;i<6;i++)
    {
      for (ii=1;ii<4;ii++)
	{
	  table_color[1][13*i+ii] = '8';
	  table_color[2][13*i+ii] = '8';
	  table_color[3][13*i+ii] = '8';
	}

    }


  

}



/*
 *------------------------------------------------------------------
 *- bool LoadBMP(char*, GLfloat, GLfloat) --------------------------
 *------------------------------------------------------------------
 *- Description: This function loads a Windows Bitmap into the	   -
 *-		 TEXTURE class object that it represents.          -
 *------------------------------------------------------------------
 *- Big thanks to NeHe for this one				   -
 *------------------------------------------------------------------
 */
int LoadBMP(char* file_data, unsigned int* width, unsigned int* height, unsigned int *font_bpp, unsigned char *data)
{

  *width = 2048;
  *height = 12;
  *font_bpp = 8;



  //	Out_Debug_String("Loaded  correctly \n\r");
	
  return 1;
}

/*affiche un caractere a l'ecran a la position    */
/*x y (unité: le caractere)                       */
/*si ca déborde, ben ca débordera.                */
/*les couleur sont gérées la aussi                */

#ifdef VIDEO_FB
void print_char(unsigned char a, unsigned int x, unsigned int y)
{

  unsigned int x_pix, y_pix, x1, y1;
  int char_font_x, char_screen_x;
  int font_next_line = 256 * font_x;
  unsigned int color;
  unsigned int byte_per_pixel = font_bpp>>3;
  int tmp_decal_font_x=0;
  int tmp_decal_screen_x=0;
  int tmp_font_y_m_y=0;
  //unsigned char debug[80];


  x1 = x - screen_offset_x;
  y1 = y - screen_offset_y;

  //__asm{int 3};
//  font_x = 8;
/*
Sprintf(debug, "%.8X %.8X %.8X\n\r", font_x, x1, y1);
Out_Debug_String(debug);
*/


  char_font_x = font_x * (int)a;
  char_screen_x = font_x * x + font_y * y * SCREEN_MAX_X;

	//__asm{int 3};
//	Sprintf(debug, "->%X %X \n\r", font_x, font_y);
//  Out_Debug_String(debug);
	
  for (x_pix = 0; x_pix < font_x; x_pix++)
    {
      tmp_decal_font_x = char_font_x + x_pix;
      tmp_decal_screen_x = char_screen_x + x_pix;
      for (y_pix = 0; y_pix < font_y; y_pix++)
	{
	  tmp_font_y_m_y = (font_y-y_pix)*SCREEN_MAX_X;
	  color =  (font_data[(tmp_decal_font_x + y_pix*font_next_line)*byte_per_pixel+0]>>3);


//	    Sprintf(debug, "mode %.8X %.8X \n\r", rasta_mode, fOOl_mode);
//  Out_Debug_String(debug);


	  if (rasta_mode)
	    {
	      /*Rasta Mode*/
	      switch(color)
		{
		case 0:
		  break;

		default:
		  switch (x%3)
		    {
		    case 0: color = 0x0780; break;
		    case 1: color = 0xf800; break;
		    case 2: color = 0xffe0; break;
		    }
					
		  break;

		}
	    }
	  if (fOOl_mode)
	    {
	      /*f001 Mode*/ 
	      /* 0 blk, 1 darkbl, 2 darkgr, 3 cyan, 4 red, 5 purpur, 6 orange, 7 lgray, 8 dgray, 
		 9 lblue, 10 lgrn, 11 lblu, 12 lred, 13 lping, 14 yell, 15 white */
	      
	      switch ( ((x+fOOl_mode_color_shift)>>2)%8)
		{
		case 0: color = 0x1; break;
		case 1: color = 0x9; break;
		case 2: color = 0x3; break;
		case 3: color = 0xb; break;
		case 4: color = 0xf; break;
		case 5: color = 0xb; break;
		case 6: color = 0x3; break;
		case 7: color = 0x9; break;
		}
	      
	      
	    }

	  if (!rasta_mode && !fOOl_mode)
	    
	    {
//	    Sprintf(debug, "normal mode %.8X %.8X %.8X %X\n\r", a, x1, y1, color);
//Out_Debug_String(debug);

	      /*Normal mode*/
	      if ((x1<80) && (y1<40))
		{
		  if (!color)
		    {
		    if (x1 == cursor_X && y1 == cursor_Y) 
		      color = 0xc618;
		    }
		  else
		    {
		      switch(table_color[y1][x1])
			{
			case 0:
			  break;
			case ' ': color = 0xc618; break;
			case '1': color = 0x0780; break;
			case '2': color = 0x9CD3; break;
			case '8': color = 0x44b0; break;
			default:
			  break;
			  
			}
		    }
		}
	      
	    }
	  /*
	  #ifdef BYTE_PER_PIXEL_HALF
	  *((char*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	  #endif


	  #ifdef BYTE_PER_PIXEL_ONE
	  *((char*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	  #endif
	  #ifdef BYTE_PER_PIXEL_TWO
	  *((short*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	  #endif
	  #ifdef BYTE_PER_PIXEL_THREE
	  *((char*)screen_adresse+0+2*(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	  #endif
	  */
	  //__asm{int 3};
	  #ifdef BYTE_PER_PIXEL_FOUR
	  #ifdef HW_FB
	  poked1(screen_adresse+4*(tmp_decal_screen_x + (tmp_font_y_m_y)) , color);
	  #else
	  *((int*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	  #endif
	  #endif

	  /*
	  switch(screen_bpp)
	    {
	    case 1:
	      *((char*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	      break;
	    case 2:
	      *((short*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	      break;
	    case 3:
	      *((char*)screen_adresse+0+2*(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	      *((char*)screen_adresse+1+2*(tmp_decal_screen_x + (tmp_font_y_m_y)))=(color>>8)&0xff;
	      break;
	    case 4:
	      *((int*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	      break;

	    default:
	      *((short*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	      break;
	  
	    }
	  */	


	}
    }
	
}
#endif

#ifdef VIDEO_AA
#define TEXT_MODE_ADDRESS     0xC00B8000

unsigned char videobaselow;
unsigned char videobasehigh;

void read_mem_base(void)
   {
#ifdef ATT_ASM
   __asm("\n\t"
      "movw $0x3d4,%dx\n\t"
      "movb $0x0c,%al\n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d5,%dx\n\t"
      "inb  %dx,%al\n\t"
      "movb %al,(videobasehigh);  \n\t"
      "movw $0x3d4,%dx\n\t"
      "movb $0x0d,%al\n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d5,%dx\n\t"
      "inb  %dx,%al\n\t"
      "movb %al,(videobaselow);\n\t"
      );
#endif
   };
   
void restore_mem_base(void) 
   {
#ifdef ATT_ASM
   __asm("\n\t"
      "movw $0x3d4,%dx\n\t"
      "movb $0x0c,%al\n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d5,%dx\n\t"
      "movb (videobasehigh),%al  \n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d4,%dx\n\t"
      "movb $0x0d,%al\n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d5,%dx\n\t"
      "movb (videobaselow),%al  \n\t"
      "outb %al,%dx\n\t"
      );
#endif
   };

void reset_mem_to_0(void)   
   {
#ifdef ATT_ASM     
   __asm("\n\t"
      "movw $0x3d4,%dx\n\t"
      "movw $0x000c,%ax\n\t"
      "outw %ax,%dx\n\t"
      "movw $0x000d,%ax\n\t"
      "outw %ax,%dx\n\t"
      );
#endif
     
   };

void Start_Display(void)
{

  //printk("read...");
  read_mem_base();
  //printk("mem to 0...");
  reset_mem_to_0();
};

void End_Display(void)
{

  //printk("mem base...");
  restore_mem_base();	  
};




void print_char(unsigned char a, unsigned int x, unsigned int y)
{
  char *ptr=(char *) TEXT_MODE_ADDRESS/*+80*10*2*/;
  int color=0;

  if ((y>25)||(x>80))
    return; 

  //Start_Display();

  ptr=ptr+(2*80*y);     
  ptr=ptr+(x*2);

  *ptr=a;
  ptr++;

  if (rasta_mode)
    {
      /*Rasta Mode*/
      switch (x%3)
	{
	case 0: color = 0x4; break;
	case 1: color = 0xE; break;
	case 2: color = 0xA; break;
	}
      
      
    }

  if (fOOl_mode)
    {
      /*f001 Mode*/ 
      /* 0 blk, 1 darkbl, 2 darkgr, 3 cyan, 4 red, 5 purpur, 6 orange, 7 lgray, 8 dgray, 
	 9 lblue, 10 lgrn, 11 lblu, 12 lred, 13 lping, 14 yell, 15 white */
      
      switch ( ((x+fOOl_mode_color_shift)>>2)%8)
 	{
 	case 0: color = 0x1; break;
 	case 1: color = 0x9; break;
 	case 2: color = 0x3; break;
 	case 3: color = 0xb; break;
 	case 4: color = 0xf; break;
 	case 5: color = 0xb; break;
 	case 6: color = 0x3; break;
 	case 7: color = 0x9; break;
 	}

  
    }
  if (!rasta_mode && !fOOl_mode)
    {
      /*Normal mode*/
      if ((x<80) && (y<40) )
	{

	  switch(table_color[y][x])
	    {
	    case 0:
	      break;
	    case ' ': color = 0x7; break;
	    case '0': color = 0xc7; break;
	    case '1': color = 0x3; break;
	    case '2': color = 0x8; break;
	    case '8': color = 0x2; break;
	    default:
	      break;
	      
	    }
	
	}
    }
  
  
  
  *ptr=(char)color;
  

}

/*
void Clear_VHAL(void)
 {
 int x,y;
 for (x=0;x<27;x++)
 for (y=0;y<80;y++)
   print_char('A', x, y);
  // PutChar(x,y,' ',COLORSET1);
 };
*/
#endif /* VIDEO_AA */


/*affiche une chaine de char a la pos x y         */
/*                                                */
/*                                                */

void Put_String(char* string, unsigned int x, unsigned int y)
{
  unsigned int i;

  for (i=0;i<rr0d_strlen(string);i++)
    {
      /*
      if (x+i>=80)
      break;*/

      print_char(string[i], x+i,y);
    }



}
/*surcouche englobant le décalage de l'ecran      */
/*                                                */

void PutString(unsigned long y,unsigned long x,char *ptr,char attrib)
{
#ifdef VIDEO_FB
  Put_String(ptr, x+screen_offset_x, y+screen_offset_y);
#endif
#ifdef VIDEO_AA
  Put_String(ptr, x, y);
#endif

}

/*dump les pixel de l'ecran pour plus tard quand  */
/*voudra remettre les pixel ecrasés               */
void dump_screen(void)
{
  /*	int i;*/
  /*
    for (i=0;i<SCREEN_MAX_X*SCREEN_MAX_Y*BYTE_PER_PIXEL/4;i++)
    *(((unsigned int*)screen_data)+i)=((unsigned int*)screen_adresse)[i];
    */
#ifdef VIDEO_AA 
  int i;

  unsigned int *ptr=(unsigned int *) TEXT_MODE_ADDRESS;
  for (i=0;i<dump_screen_size/sizeof(unsigned int);i++)
    {
      dump_screen_data[i] = ptr[i];
    }
#endif


}

/*l'inverse                                       */
/*                                                */

void restore_screen(int do_work)
{
  /*	int i;*/
  /*
    for (i=0;i<SCREEN_MAX_X*SCREEN_MAX_Y*BYTE_PER_PIXEL/4;i++)
    ((unsigned int*)screen_adresse)[i]=*(((unsigned int*)screen_data)+i);
  */
#ifdef VIDEO_AA 

  int i;
  unsigned int *ptr=(unsigned int *) TEXT_MODE_ADDRESS;
  
  if (!do_work)
    return;
  for (i=0;i<dump_screen_size/sizeof(unsigned int);i++)
    {
       ptr[i] = dump_screen_data[i];
    }
#endif
}


void put_XY_char(int x, int y, unsigned char a)
{
  table[y][x]=a;
}

               

void displaymessage(char *ptr) /* Display a message */
{
  int x=1;

  accessline[cmd_win_y+1]=1;
  while (*ptr!=0)
    {
      accessline[x]=1;
      /* screen_line24[x]=*ptr; */
      table[cmd_win_y+1][x]=*ptr;

      x++;
      ptr++;
    };

}

void WriteVCon(unsigned int x,unsigned y,char *ptr)
{
  if ((x<SCREEN_LINE) && (y<80) && ((rr0d_strlen(ptr)+y)<80)) 
    {
      strcpy(&table[x][y], ptr);     
      accessline[x]=1;
    };
}
 
void delline(int screen_line)     /* Clear a buffer line */
{
  int x;
  char *ptr;
  accessline[screen_line]=1;
  ptr=&table[screen_line][0];
  for (x=0;x<80;x++)
    {
      *ptr=' ';
      ptr++;
    };
}

void ClearAW()             /* Clear A window */
{
  unsigned int x;
  for (x=buff_win_y;x<=buff_win_y + buff_win_dim;x++)
    delline(x);
}

void ClearDW()             /* Clear dump window */
{
  unsigned int x;
  for (x=data_win_y;x<=data_win_y + data_win_dim;x++)
    delline(x);
}


void ClearCW()             /* Clear dump window */
{
  unsigned int x;
  for (x=code_win_y;x<=code_win_y + code_win_dim;x++)
    delline(x);
}

void cls()                 /* Clear Screen */
{
  insline=40;
  ClearAW();
  ClearDW();
}


void DisplayText(int x,int y,char *texto) /* Display text in a x,y position in Console */
{
  char *buf=(char *) &table[y][x];
  char *tmp=texto;
  accessline[y]=1;
  while (*tmp!=0) 
    {
      *buf=*tmp;
      buf++;tmp++;
      x++;
      if (x>78)
	break;
    };
}



void print_AW(char* ptr, int ligne)
{
  DisplayText(1,buff_win_y+ligne,ptr);
}

void print_DW(char* ptr, int ligne)
{
  DisplayText(1,data_win_y+ligne,ptr);
}

void print_CW(char* ptr, int ligne)
{
  DisplayText(1,code_win_y+ligne,ptr);
}


/*rafraichi la fenetre de log                     */
/*                                                */
void Refresh_AW()
{
  unsigned int i;
  for (i=buff_win_y;i<=buff_win_y + buff_win_dim;i++)
    accessline[i]=1;
}

/*rafraichi la fenetre de data                    */
/*                                                */
void Refresh_DW()
{
  unsigned int i;
  for (i=data_win_y;i<data_win_y + data_win_dim;i++)
    accessline[i]=1;
}

/*rafraichi la fenetre de code                    */
/*                                                */
void Refresh_CW()
{
  unsigned int i;
  for (i=code_win_y;i<code_win_y + code_win_dim;i++)
    accessline[i]=1;
}

/*rafraichi tout                                  */
/*                                                */
void Refresh_ALL()
{
  unsigned int i;
  for (i=0;i<SCREEN_LINE;i++)
    accessline[i]=1;
}


void cursor_top_right(void)
{
  rr0d_outb(0xC, 0x3d4); 
  rr0d_outb(0x0, 0x3d5); 
  rr0d_outb(0xd, 0x3d4); 
  rr0d_outb(0x0, 0x3d5); 

}
/*end test :)*/

/*re affiche les lignes qui ont besoin d'etre     */
/*rafraichies                                     */

void Refresh_Display()       /* look for a modified lines and rewrite them */
{
  
  unsigned long screen_line;
 
  screen_line=0;
  

#ifdef VIDEO_AA
cursor_top_right();
  Start_Display();
#endif /* VIDEO_AA */

  for(screen_line=0;screen_line<SCREEN_LINE;screen_line++)
    {
      if (accessline[screen_line]){
	if (insline!=screen_line) PutString(screen_line,0,&table[screen_line][0],COLOR);
	else     PutString(screen_line,0,&table[screen_line][0],COLORSET2);
      };
      accessline[screen_line]=0;	
    };

#ifdef VIDEO_AA
    End_Display();
#endif /* VIDEO_AA */

}

/* ajoute un caractere a la commande courante */
void InsertCommandKey(char car) /* A new key is pressed and is added to the command */
{
  accessline[cmd_win_y]=1;
  Command[CommandLength]=car;

  table[cmd_win_y][1+CommandLength]=car;
  CommandLength++;
  Command[CommandLength]=0x0;
}

/*efface la commande courante                     */
/*                                                */
void ClearCommand()                  /*  Clear Command line */
{
  delline(cmd_win_y);   
  CommandLength=0;
}

/*efface un car de la commande courante           */
/*(bacjspace)                                     */
void DeleteCommandKey()
{
  accessline[cmd_win_y]=1;
  if (CommandLength) {

    table[cmd_win_y][CommandLength]=' ';

		 
    CommandLength--;
    Command[CommandLength]=0x0;
  };      
}

void deblin_change_color(char *object,char *val)
{
}

void deblin_console_off()
{

} 

 
void deblin_console_on()
{
  int x;
  for (x=0;x<SCREEN_LINE;x++) accessline[x]=1; /* We want to modify all lines */

  Refresh_Display();
}




void test_scr()
{

  int i;


  for (i=0;i<28;i++)
    Put_String(&table[i][0], 60, 70+i);


}



void update_cursor(unsigned int x, unsigned int y)
{
  if (y<data_win_y)
    y = data_win_y;

  if (x<15)
    x = 15;


  if (y>data_win_y + data_win_dim-1)
    y = (unsigned int )(data_win_y + data_win_dim-1);

  if (x>80-1)
    x = 80-1;



  if ((x<80) && (y<40) )
    {
      accessline[y]=1;
      accessline[cursor_Y]=1;
      table_color[cursor_Y][cursor_X] = (char)cursor_old_color;

      if (x<63)
      if (!((x+1)%3))
	x = x>cursor_X?x+1:x-1;



      cursor_old_color = table_color[y][x];
      table_color[y][x] = (char)cursor_color;



      cursor_X = x;
      cursor_Y = y;

    }


  Refresh_Display();

}

void erase_cursor()
{

  if ((cursor_X<80) && (cursor_Y<40) )
    {
      accessline[cursor_Y]=1;
      table_color[cursor_Y][cursor_X] = (char)cursor_old_color;
    }


  Refresh_Display();

}

