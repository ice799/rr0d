/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id: command.c,v 1.31 2006-06-30 12:25:35 serpilliere Exp $
 *
 */


#include "var_globale.h"
#include "command.h"
#include "utils.h"
#include "import_symb.h"

/* without that gcc complains on FreeBSD */


char cmd_result[80];			/* buffer for entered command */
unsigned int adresse_ret_1;
unsigned int adresse_ret_2;


unsigned int edit_mode=0;

#define STATE_ADD			1000000

#define STATE_NO_COMMAND	1
#define STATE_COMMAND		2
#define STATE_HEXA		3
#define STATE_RASTA		4
#define STATE_UNRASTA		5
#define STATE_INVALID		1337
#define STATE_END		666

#define CMD_VER				1000000
#define CMD_E_DATA			1000001
#define CMD_TEST			1000002
#define CMD_E_CODE			1000003
#define CMD_RASTA			1000004
#define CMD_UNRASTA			1000005
#define CMD_SET				1000006

#define CMD_PARSEREG		        1000100
#define CMD_HELP			1000007
#define CMD_BPX				1000008
#define CMD_BC				1000009
#define CMD_BL				1000010
#define CMD_IDT				1000011
#define CMD_BPM				1000012
#define CMD_LOAD_SYMB			1000013
#define CMD_PAGE_INFO			1000014
#define CMD_SYMB			1000015
#define CMD_PRINT			1000016
#define CMD_EDITMEM			1000017
#define CMD_INTHOOK			1000018
#define CMD_RDTSCHOOK			1000019
#define CMD_F001			1000020
#define CMD_UNF001			1000021

#define CMD_E_DATA_END   		1100001
#define CMD_E_CODE_END			1100003
#define CMD_BPX_END			1100008
#define CMD_BC_END			1100009
#define CMD_SET_END			1100006
#define CMD_BPM_END			1100010
#define CMD_PAGE_INFO_END		1100014

#define CMD_PRINT_END   		1100016

#define CMD_INTHOOK_END			1100018


#define CMD_LOAD_SYMB_BIS		1100024
#define CMD_LOAD_SYMB_END		1100025

int nbr_arg=0;
#define MAX_CMD 20

/*table des commande possibles                    */

char *table_command[]=
			{
			"VER",
			"D",
			"DROIDS",
			"U",
			"RASTA",
			"UNRASTA",
			"R",
			"HELP",
			"BPX",
			"BC",
			"BL",
			"IDT",
			"BPM",
			"LOAD",
			"PAGE",
			"SYMB",
			"PRINT",
			"E",
			"INTHOOK",
			"RDTSCHOOK",
 			"f001",
 			"unf001",
			0
			};

/*tables des réponses aux commandes possibles     */

char *command_res[]=
			{
			"RasTa Ring0 DebUgger v0.3 zeta ",
			"D adresse, man                 ",
			"RuLEZ                          ",
			"U adresse, man                 ",
			"Rasta mode On (sweet, man)     ",
			"Rasta mode Off(keep rasta, man)",
			"SET data, man                  ",
			"need some help man?            ",
			"soft break, man                ", 
			"del break, man                 ",
			"bp green list, man             ",
			"idt                            ",
			"hard break point, man          ",
			"Load Symbol man                ",
			"Info on pages man              ",
			"Info on symbols, man           ",
			"Print reg                      ",
			"Edit the memory man            ",
			"ON/OFF hook interrupt N        ",
			"ON/OFF hook rdtsc              ",
 			"°±²Û²±° 0xf001 ON  °±²Û²±°     ",
 			"°±²Û²±° 0xf001 OFF °±²Û²±°     ",

			};





/*table des registres possibles en argument       */

char *table_reg[]=
			{
			"EAX",
			"EBX",
			"ECX",
			"EDX",
			"ESI",
			"EDI",
			"EIP",
			"ESP",
			"EBP",
			"DR0",
			"DR1",
			"DR2",
			"DR3",
			"DR6",
			"DR7",
			0
			};

/*compare 2 chaine de char                        */

/* int cmp_string(char *text,char *buf) */
/* { */
/* 	if (! (text && buf) ) */
/* 		return 0; */
/* 	/\* */
/* 	if ( (*text) != (*buf) ) */
/* 		return 0; */
/* 	*\/ */
	
/* 	while (( (*text) && (*buf)) &&  */
/* 	       ( ((*text) == (*buf))|| */
/* 		 ((*text | 0x20) == (*buf | 0x20)) )) */
/* 	{ */
/* 		text++; */
/* 		buf++; */
/* 	} */
	

/* 	return ( (*text == *buf) || (*text == *buf)); */

/* } */

/*renvoie le num de commande entrée               */

int chose_command(char* command)
{
	int i=0;


	while( table_command[i] != 0 )
	{
		if (!rr0d_stricmp(command, table_command[i]))
		{
			rr0d_strcpy(cmd_result, command_res[i]);
			//	printf("pouet 1");
			return i+STATE_ADD;
		}

		i++;

	}

	//			printf("BAD 1");

	rr0d_strcpy(cmd_result, "parse: invalid cmd, man");
	return 0;
}

/*renvoie le num du registre entré                */

int chose_reg(char* reg)
{
	int i=0;

	while(table_reg[i] != 0)
	{
	  if (!rr0d_strnicmp(reg, table_reg[i], 3))
		{
			return i;
		}
	  i++;

	}
	return -1;
}


unsigned int chose_symb(char* symb)
{
  int i;
  
  i = 0;
  while(tab_import_symb[i].address && (i<MAX_SYMBOL))
    {

      if (!rr0d_strnicmp(symb, tab_import_symb[i].name, rr0d_strlen(tab_import_symb[i].name)))
	{
	  return i;
	}      
      
      i++;
    }
  return -1;
}



unsigned int adresse_tmp =0;
unsigned int next_state = 0;

unsigned int num_reg;

/*automate reconnaissant le langage de rr0d       */
/*parse_ret est l'entier représentant la commande */
/*parsée                                          */
/*adresse_ret1 est l'argument 1 de la commande    */
/*parsée (peut ne pas etre utilisé)               */
/*adresse_ret2 est l'argument 2 de la commande    */
/*parsée (peut ne pas etre utilisé)               */

int parse_command(char* command)
{
	char c;
	int i=0;
	int state = STATE_NO_COMMAND;
	int ret;
	int num_s;
	int rr;
	int fini=0;
	int parse_ret=0;

	char* start_cmd = (char*)0;

	char* start_reg = (char*)0;


	fini=0;
	while(!fini)
	{

		c = command[i];
		switch(state)
		{
			

			case STATE_NO_COMMAND:
				switch(c)
				{
				case ' ':
				case  0:
					i++;
					break;

				default:
					start_cmd = &command[i];
					i++;
					state = STATE_COMMAND;
					break;
				}
				break;


			case STATE_COMMAND:
				switch(c)
				{
				case  0:
				case ' ':
				  command[i]=0;
				
					i++;
					ret = chose_command(start_cmd);
					if (!ret)
					{
						state = STATE_INVALID;
						break;
					}

					state = ret;

					break;

				default:
					i++;
					break;
				}
				break;



			case CMD_E_DATA:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					state = STATE_HEXA;
					next_state = CMD_E_DATA_END;
					adresse_tmp=0;
					break;

				}
				break;

			case CMD_E_CODE:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					state = STATE_HEXA;
					next_state = CMD_E_CODE_END;
					adresse_tmp=0;
					break;

				}
				break;


			case STATE_HEXA:
			  if ((rr = chose_reg(&command[i]))!=-1)
			    {
			      switch(rr)
				{
				case 0:
				  adresse_tmp = reg_stack->reg_EAX;
				  break;
				case 1:
				  adresse_tmp = reg_stack->reg_EBX;
				  break;
				case 2:
				  adresse_tmp = reg_stack->reg_ECX;
				  break;
				case 3:
				  adresse_tmp = reg_stack->reg_EDX;
				  break;
				case 4:
				  adresse_tmp = reg_stack->reg_ESI;
				  break;
				case 5:
				  adresse_tmp = reg_stack->reg_EDI;
				  break;
				case 6:
				  adresse_tmp = reg_stack->reg_EIP;
				  break;
				case 7:
				  adresse_tmp = reg_stack->reg_ESP;
				  break;
				case 8:
				  adresse_tmp = reg_stack->reg_EBP;
				  break;

				default:
				  adresse_tmp = 0xDEADFACE;
				  break;
				}

			      i+=3;


			      
			    }
			  else 
			  if ((num_s = chose_symb(&command[i]))!=-1)
			    {
			      adresse_tmp = tab_import_symb[num_s].address;
			      i+=rr0d_strlen(tab_import_symb[num_s].name);
			      break;
			      
			    }else
			      {
				if ((c >= 'A') && (c <= 'F') )
				{
					c = c - 'A' + 10;
					adresse_tmp= adresse_tmp<<4;
					adresse_tmp+=c;
					i++;
					break;
				}
				if ((c >= 'a') && (c <= 'f') )
				{
					c = c - 'a' + 10;
					adresse_tmp= adresse_tmp<<4;
					adresse_tmp+=c;
					i++;
					break;
				}

				if ((c >= '0') && (c <= '9') )
				{
					c = c - '0';
					adresse_tmp= adresse_tmp<<4;
					adresse_tmp+=c;
					i++;
					break;
				}
			    }

			  state = next_state;
			  break;
			
			
			case CMD_E_DATA_END:
				adresse_ret_1 = adresse_tmp;
				state = STATE_END;
				parse_ret = 2;
				break;

			case CMD_E_CODE_END:
				adresse_ret_2 = adresse_tmp;
				state = STATE_END;
				parse_ret = 3;
				break;

			case CMD_RASTA:
				fini = 1;
				parse_ret = 4;
				break;

			case CMD_UNRASTA:
				fini = 1;
				parse_ret = 5;
				break;

			case CMD_SET:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					start_reg = &command[i];
					state = CMD_PARSEREG;
					next_state = STATE_HEXA;
					adresse_tmp=0;
					parse_ret = 6;
					break;

				}
				

				break;

			case CMD_PARSEREG:
				switch(c)
				{
				case ' ':
					command[i]=0;
					if ( (num_reg=chose_reg(start_reg))==-1)
					{
						state = STATE_INVALID;
						break;
					}

					adresse_ret_2 = num_reg;

					i++;
					state = next_state;
					next_state = CMD_SET_END;
					
					break;

					/*
				case  0:
					state = STATE_INVALID;
					break;
					*/

				default:
					i++;
					break;

				}
				break;



			case CMD_SET_END:
				adresse_ret_1 = adresse_tmp;
				fini =1;
				break;

			case STATE_INVALID:
				fini = 1;
				parse_ret = 0;
				break;


			case STATE_END:
				fini = 1;
				break;

			case CMD_VER:
			case CMD_TEST:
				fini = 1;
				parse_ret = 1;
				break;

			

			case CMD_HELP:
				fini = 1;
				parse_ret = 8;
				break;

			case CMD_BPX:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					state = STATE_HEXA;
					next_state = CMD_BPX_END;
					adresse_tmp=0;
					break;

				}
				break;

			case CMD_BPM:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					state = STATE_HEXA;
					next_state = CMD_BPM_END;
					adresse_tmp=0;
					break;

				}
				break;

			case CMD_PAGE_INFO:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					state = STATE_HEXA;
					next_state = CMD_PAGE_INFO_END;
					adresse_tmp=0;
					break;

				}
				break;

			case CMD_BC:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					state = STATE_HEXA;
					next_state = CMD_BC_END;
					adresse_tmp=0;
					break;

				}
					break;


			case CMD_LOAD_SYMB:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					state = STATE_HEXA;
					next_state = CMD_LOAD_SYMB_BIS;
					adresse_tmp=0;
					break;

				}
				break;
			case CMD_LOAD_SYMB_BIS:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					state = STATE_HEXA;
					adresse_ret_1 = adresse_tmp;
					next_state = CMD_LOAD_SYMB_END;
					adresse_tmp=0;
					break;

				}
				break;


			case CMD_INTHOOK:
				switch(c)
				{
				case ' ':
					i++;
					break;

				case  0:
					state = STATE_INVALID;
					break;

				default:
					state = STATE_HEXA;
					next_state = CMD_INTHOOK_END;
					adresse_tmp=0;
					break;

				}
				break;



			case CMD_BPX_END:
				adresse_ret_1 = adresse_tmp;
				state = STATE_END;
				parse_ret = 9;
				break;
		

			case CMD_BC_END:
				adresse_ret_1 = adresse_tmp;
				state = STATE_END;
				parse_ret = 10;
				break;

			case CMD_BL:
				fini = 1;
				parse_ret = 11;
				break;

			case CMD_IDT:
				fini = 1;
				parse_ret = 12;
				break;

			case CMD_BPM_END:
				adresse_ret_1 = adresse_tmp;
				state = STATE_END;
				parse_ret = 13;
				break;



			case CMD_LOAD_SYMB_END:
				adresse_ret_2 = adresse_tmp;
				state = STATE_END;
				parse_ret = 14;
				break;

			case CMD_PAGE_INFO_END:
				adresse_ret_1 = adresse_tmp;
				state = STATE_END;
				parse_ret = 15;
				break;

			case CMD_SYMB:
				fini = 1;
				parse_ret = 16;
				break;


			case CMD_PRINT:
			  fini = 1;
			  //				command[i]=0;
			  if ( (num_reg=chose_reg(&command[i]))==-1)
			    {
			      state = STATE_INVALID;
			      break;
			    }
			  
			  parse_ret = 0xff;
			  adresse_ret_2 = num_reg;
			  
			  break;

			case CMD_EDITMEM:
				fini = 1;
				parse_ret = 17;
				edit_mode = !edit_mode;
				break;


			case CMD_INTHOOK_END:
				adresse_ret_1 = adresse_tmp;
				state = STATE_END;
				parse_ret = 18;
				break;

			case CMD_RDTSCHOOK:
				fini = 1;
				parse_ret = 19;
				break;

 			case CMD_F001:
			        fini = 1;
 				parse_ret = 20; /* f001 mode on*/
				break;
 			case CMD_UNF001:
 				fini = 1;
 				parse_ret = 21; /* f001 mode off */
 				break;

			  

			default:
				fini = 1;
				parse_ret = 0;
				break;

			



		}
	}


	return parse_ret;


}
