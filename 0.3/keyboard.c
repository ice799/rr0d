/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id: keyboard.c,v 1.85 2006-07-12 13:03:55 serpilliere Exp $
 *
 */

#include "vars.h"

#include "nasm.h"
#include "disasm.h"


#include "keyboard.h"
#include "buffering.h"
#include "utils.h"
#include "video.h"
#include "var_globale.h"
#include "command.h"

#include "page.h"
#include "breakpoint.h"
#include "idt.h"

#include "import_symb.h"

void resetkbd(void);
void dummy_asm(void);

unsigned int ctrl_key=0;
unsigned int shift_key=0;
unsigned int alt_key =0;

extern int CommandLength;
extern char Command[256];

extern unsigned int rasta_mode;
extern unsigned int fOOl_mode;
extern unsigned int fOOl_mode_color_shift;
extern unsigned int first_asm_line;
extern unsigned int update_disasm;
extern unsigned int force_disasm;

//extern char *table_command[];
extern char *table_command[];

extern long back_disasm (unsigned int , int ); 
unsigned char key_state;


extern void *old_int[MAX_INT];
extern void *tmp_int[MAX_INT];
extern unsigned long addrnewint[MAX_INT];


extern int is_int_hooked[MAX_INT];


int hook_rdtsc = 0;


// commands
#define READ_CONTROLLER		0x20
#define WRITE_CONTROLLER	0x60

// command bytes
#define SET_LEDS			0xED
#define KEY_RESET			0xFF

// responses from keyboard
#define KEY_ACK				0xFA	// ack
#define KEY_AGAIN			0xFE	// send again

// 8042 ports
// when you read from port 64, this is called STATUS_BYTE
// when you write to port 64, this is called COMMAND_BYTE
// read and write on port 64 is called DATA_BYTE 
unsigned char KEYBOARD_PORT_60 = (unsigned char)0x60;
unsigned char KEYBOARD_PORT_64 = (unsigned char)0x64;

// status register bits
#define IBUFFER_FULL		0x02
#define OBUFFER_FULL		0x01

// flags for keyboard LEDS
#define SCROLL_LOCK_BIT		(0x01 << 0)
#define NUMLOCK_BIT			(0x01 << 1)
#define CAPS_LOCK_BIT		(0x01 << 2)

unsigned int WaitForKeyboard(void)
{
  int i = 100;	// number of times to loop
  unsigned char mychar;
	
  do
    {
      mychar = rr0d_inb( KEYBOARD_PORT_64 );
      if(!(mychar & IBUFFER_FULL)) break;	// if the flag is clear, we go ahead
    }
  while (i--);

  if(i) return 1;
  return 0;
}

// call WaitForKeyboard before calling this function
void DrainOutputBuffer(void)
{
  int i = 100;	// number of times to loop
  unsigned char c;

  do
    {
      c = rr0d_inb(KEYBOARD_PORT_64);
      if(!(c & OBUFFER_FULL)) break;	// if the flag is clear, we go ahead	
      c = rr0d_inb(KEYBOARD_PORT_60);
    }
  while (i--);
}

// write a byte to the data port at 0x60
unsigned int SendKeyboardCommand(  unsigned char theCommand )
{
  if(1 == WaitForKeyboard())
    {
      DrainOutputBuffer();
      rr0d_outb( theCommand, KEYBOARD_PORT_60 );
    }
  else
    {
      return 0;
    }	
  // TODO: wait for ACK or RESEND from keyboard	
  return 1;
}

unsigned int must_read = 1;
volatile unsigned char key_code_loc;

unsigned char print_keystroke(void)
{

  unsigned char state;
  unsigned char key_c;

/*   if (must_read) */
/*     { */
/*       must_read = 0; */
/*       c = rr0d_inb(KEYBOARD_PORT_60); */
	
/*       if (fired) */
/* 	must_read = 1; */
/*       else */
/* 	{ */
/* 	  /\* */
/* 	  rr0d_outb(0xD2, KEYBOARD_PORT_64);  */
/* 	  WaitForKeyboard(); */
/* 	  rr0d_outb(c, KEYBOARD_PORT_60);  */
/* 	  *\/ */
/* 	} */

/*       handle_scancode(c); */
/*     } */
/*   else */
/*     must_read = 1; */

  if (myWaitForKeyboard(&state))
    {
      key_state = state;
      if (!(key_state&0x20))
	{
	  //if (1)
	  key_c = rr0d_inb(KEYBOARD_PORT_60);

	  handle_scancode(key_c);	  
	  
	  if (!fired)
	    {
	      myWaitForKeyboard(&state);
	      rr0d_outb(0xAD, KEYBOARD_PORT_64);
	      
	      myWaitForKeyboard(&state);
	      rr0d_outb(0xD2, KEYBOARD_PORT_64);
	      
	      myWaitForKeyboard(&state);
	      rr0d_outb(key_c, KEYBOARD_PORT_60);
	      
	      myWaitForKeyboard(&state);
	      rr0d_outb(0xAE, KEYBOARD_PORT_64);
	    }

	  return key_c;


	}
    }

  return 0;
}

char debug__[80];
char mouse_on;
char mouse;
char mouse1;
char mouse2;
char mouse3;

volatile unsigned int tmpo=0xdeadbeef;
unsigned int myWaitForKeyboard(unsigned char* state)
{
  int i = 0x100;	// number of times to loop
  unsigned char mychar;
	
  do
    {
      mychar = rr0d_inb( KEYBOARD_PORT_64 );
      for (tmpo=0;tmpo<0x10;tmpo++) tmpo = tmpo;

      if((mychar & 1)) break;	// if the flag is clear, we go ahead
    }
  while (i--);

  if(i>=0) 
    {
      *state = mychar;
      return 1;
    }
  key_code_loc = 0;
  return 0;
}





unsigned int led = 1;
unsigned int led_wait=0;
char led_tab[] = {2, 4, 1};
//void  print_keystroke(void)
unsigned int read_scancode(void)
{
  unsigned char state;
  unsigned char m1, m2, m3;
  unsigned char signe_X, signe_Y, of_X, of_Y, b1, b2, b3;
  int pX, pY;

  /*test led*/
  /*  
  led_wait++;
  if (led_wait>200)
    {
      led_wait = 0;
      led++;
      led = led%3;
      SendKeyboardCommand(0xED);
      SendKeyboardCommand(led_tab[led]);

      fOOl_mode_color_shift = (fOOl_mode_color_shift+1)%64;

    }
*/
  if (myWaitForKeyboard(&state))
    {
      key_state = state;
      if (!(key_state&0x20))
	{
	  //if (1)
	  key_code_loc = rr0d_inb(KEYBOARD_PORT_60);

	  /*
	  if (!fired)    
	    {
	      myWaitForKeyboard(&state);
	      rr0d_outb(0xAD, KEYBOARD_PORT_64);
	      
	      myWaitForKeyboard(&state);
	      rr0d_outb(0xD2, KEYBOARD_PORT_64);
	      
	      myWaitForKeyboard(&state);
	      rr0d_outb(key_code_loc, KEYBOARD_PORT_60);
	      
	      myWaitForKeyboard(&state);
	      rr0d_outb(0xAE, KEYBOARD_PORT_64);
	      
	      
	    };  
	  */

	}
      
      
      else
	{
	  myWaitForKeyboard(&state);
	  m1 = rr0d_inb(KEYBOARD_PORT_60);
	  myWaitForKeyboard(&state);
	  m2 = rr0d_inb(KEYBOARD_PORT_60);
	  myWaitForKeyboard(&state);
	  m3 = rr0d_inb(KEYBOARD_PORT_60);

	  b1 = m1&1;
	  b2 = m1&2;
	  b3 = m1&4;

	  signe_X = m1 & 16;
	  signe_Y = m1 & 32;

	  of_X = m1 & 64;
	  of_Y = m1 & 128;

	  m2 = signe_X?((~m2)):m2;
	  m3 = signe_Y?((~m3)):m3;

	  if (of_X)
	    m2 = 0;
	  if (of_Y)
	    m3 = 0;

	  pX = m2;
	  pY = m3;

	  if (pX>0)
	    pX=pX/3 +1;
	  if (pY>0)
	    pY=pY/3 +1;

	  
	  pX = signe_X?-pX:pX;
	  pY = signe_Y?pY:-pY;

	  Sprintf(debug__, "%d %d", pX, pY);
	  insertinbuffer(debug__);


	  update_cursor(cursor_X+pX, cursor_Y+pY);
	}
    }

  return key_code_loc;



}




/*transfome les données du clavier brut en donnees*/
/*exploitable (scancode)                          */
/*                                                */
char scan_to_ascii(int scancode)
{
  int down=scancode & 0x80;
  /* AZERTY TAB */  
#define AZERTY
#ifdef AZERTY
  static char scantable_MAJ[]=	{
    0,  0, '1','2','3','4','5','6','7','8','9','0','?','­', 0,  
    0, 'A','Z','E','R','T','Y','U','I','O','P','¨','£', -1,   
    0, 'Q','S','D','F','G','H','J','K','L','M','%','µ', 0,  
    0, 'W','X','C','V','B','N','?','.','/','§', 0,  0,   
    0, ' ', 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, '7','8','9', 0, '4','5','6','+','1','2','3',  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
  };

  static char scantable_N_MAJ[]=	{
    0, '²','&','é','"','\'','(','-','è','_','ç','à',')','=', 0,  
    0, 'a','z','e','r','t','y','u','i','o','p','^','$', -1,   
    0, 'q','s','d','f','g','h','j','k','l','m','ù','*', 0,  
    0, 'w','x','c','v','b','n',',',';',':','!', 0,  0,   
    0, ' ', 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, '7','8','9', 0, '4','5','6','+','1','2','3',  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
  };

#else
/* ====== quertzy ======*/
  static char scantable_MAJ[]=	{
   0,  0, '!','"','§','$','%','&','/','(',')','=','?','`', 0,  
    0, 'Q','W','E','R','T','Y','U','I','O','P','Ü','*', -1,   
    0, 'A','S','D','F','G','H','J','K','L','Ö','Ä','\'', 0,  
    '>', 'Y','X','C','V','B','N','M',';',':','_', 0,  0,   
    0, ' ', 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
   };

  static char scantable_N_MAJ[]=	{
    0,  0, '1','2','3','4','5','6','7','8','9','0','?','­', 0,  
    0, 'q','w','e','r','t','y','u','i','o','p','ü','+', -1,   
    0, 'a','s','d','f','g','h','j','k','l','ö','ä','#', 0,  
   '<','y','x','c','v','b','n','m',',','.','-', 0,  0,   
    0, ' ', 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
  };
#endif




  switch(scancode & 0x7f)
    {

    case 42:
    case 54:
      shift_key = !down;
      break;

    case 29:
      ctrl_key = !down;
      break;

    case 56:
      alt_key = !down;
      break;
      /*
	case 58:
	shift_key = !shift_key;
	break;
      */
    }

  switch(scancode)
    {
    case 14: return KBDEL;
    case 88: return KBF12;
    case 87: return KBF11;
    case 72: return KBCURSORUP;
    case 80: return KBCURSORDOWN;
    case 56: return KBLEFTALT;
    case 54: return KBRIGHTSHIFT;
    case 42: return KBLEFTSHIFT;
    case 73: return KBPAGEUP;
    case 81: return KBPAGEDOWN;
    case 75: return KBCURSORLEFT;
    case 77: return KBCURSORRIGHT;
    case 1: return KBESC;

    }; 

  if (scancode<=0x80)
    return shift_key?scantable_MAJ[scancode]:scantable_N_MAJ[scancode];    
	
  return KBUP;
}




/*essaye de remonter le listing asm*/
/*de facon un peu pourie (brute    */
/*force...)                        */
long back_disasm (unsigned int cur_eip, int min_back_line)
{
  char tmp[100];
  int back_dec = (min_back_line<10)?10*3:min_back_line*3;
  unsigned int tmp_eip;
  int num_instr=0;
  int ret;
  int guard=15;


  unsigned int testing_eip;
  insn my_ins;

  cur_eip=translate_logic_to_linear(current_reg_stack.reg_CS, cur_eip  );

  if (!(page_exist(cur_eip)))
    return 0;

  while((!page_exist(cur_eip-back_dec))&& (back_dec>0))
    back_dec--;

  if (back_dec==0)
    return 0;


  testing_eip= cur_eip - back_dec;

  do
    {
      if (!(page_exist(testing_eip-1)))
	{	  
	  break;
	}
      testing_eip--;
      tmp_eip = testing_eip;
      num_instr=0;
      do
	{
	  ret = disasm((unsigned char*)tmp_eip, tmp,32,0,0, &my_ins);
	  tmp_eip+=ret;
	  num_instr++;
  
	}while((ret)&&(tmp_eip<cur_eip));
      
      guard--;
    }while ((tmp_eip != cur_eip)&&(guard>=0));

  while(num_instr>min_back_line)
    {
      testing_eip+=disasm((unsigned char*)testing_eip, tmp,32,0,0, &my_ins);
      num_instr--;
    }


  return cur_eip-testing_eip;

}



void dump_data(unsigned int selector, unsigned char * ptr);



#define CTRL_KEY	0x1d
#define F_KEY		0x21


/*execute les commandes en fonction des touche    */
/*saisies: raccourci, scroll, ...                 */
/*                                                */
extern Reg_struct *reg_stack;
extern void print_reg(Reg_struct *regstack);

void handle_scancode(unsigned char scancode)
{
  char car;
  unsigned int i;
  int len;
  int ret;
  int val;
  unsigned int local_data_ptr;
  unsigned int local_data_byte;

  if ((scancode & 0x7f ) == CTRL_KEY)
    {
      if (scancode & 0x80)
	ctrl_key = 0;
      else
	ctrl_key = 1;
    }

		
  if (ctrl_key)
    if ((scancode & 0x7f ) == F_KEY)
      {
	if (!(scancode & 0x80))
	  {

	    ctrl_key = 0;
	    fired = !fired;
	    return;
	  }
      }

  car=scan_to_ascii((int) scancode);

  switch(car)
    {
    case KBENTER:              /* Exec a commandSi se pulsó enter borra linea 
				  y indice igual a 0 */
      if (fired)
	{  
	  
	  if (CommandLength==0)
	    break;
	  
	  insertinbuffer(Command);

				 

                   
	  //delline(cmd_win_y);delline(cmd_win_y+1);delline(cmd_win_y+2);

	  insert_cmd(Command);
	  ret =  parse_command(Command);

	  /*
	    Sprintf(debug__, "cmd: %d", ret);
	    insertinbuffer(debug__);
	  */

	  if (ret>=1)
	    {
	      insertinbuffer(cmd_result);

	      switch(ret)
		{
		case 1:
		  break;

		case 2:
		  data_ptr = adresse_ret_1;
		  break;

		case 3:
		  code_ptr = adresse_ret_2;
		  force_disasm = 1;
		  update_disasm = 1;
		  break;

		case 4:
		  rasta_mode = 1;
		  Refresh_ALL();
		  break;

		case 5:
		  rasta_mode = 0;
		  Refresh_ALL();
		  break;

		case 6:
	
		  switch(adresse_ret_2)
		    {
		    case 0:
		      reg_stack->reg_EAX = adresse_ret_1;
		      break;
		    case 1:
		      reg_stack->reg_EBX = adresse_ret_1;
		      break;
		    case 2:
		      reg_stack->reg_ECX = adresse_ret_1;
		      break;
		    case 3:
		      reg_stack->reg_EDX = adresse_ret_1;
		      break;
		    case 4:
		      reg_stack->reg_ESI = adresse_ret_1;
		      break;
		    case 5:
		      reg_stack->reg_EDI = adresse_ret_1;
		      break;
		    case 6:
		      reg_stack->reg_EIP = adresse_ret_1;
		      break;
		    case 7:
		      reg_stack->reg_ESP = adresse_ret_1;
		      break;
		    case 8:
		      reg_stack->reg_EBP = adresse_ret_1;
		      break;

		    default:
		      break;



		    }
		  /* reg_stack-> */
		  Sprintf(debug__, "%X <= cr3", basetp());
		  insertinbuffer(debug__);
		  break;

		case 8:
		  insertinbuffer("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
		  insertinbuffer("³yeaa man, this is the help command: to use   ³");
		  insertinbuffer("³this rasta soft man, push some keys and enjoy³");
		  insertinbuffer("³the different actions it will produce man    ³");
		  insertinbuffer("³°±²Û²±° °±²Û²±°    peace man, rasta them all.³");
		  insertinbuffer("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");

		  insertinbuffer(" ");
		  insertinbuffer("Commands:");

		  i = 0;
		  while( table_command[i++] ) {
			insertinbuffer(table_command[i-1]);
		  }

	          insertinbuffer("   use up/down keys to scr0ll");



		  break;

		case 9:
		  insert_bp(current_reg_stack.reg_CS, adresse_ret_1, BP_CLASSIC);
		  break;

		case 10:
		  if (erase_breakpoint(adresse_ret_1)==-1)
		    {
		      insertinbuffer("not a green break point, man");
		      break;

		    }
		  break;

		case 11:
		  display_breakpoints();
		  break;

		case 12:
		  visualise_idt((unsigned long *) idt_[0]);
		  break;

		case 13:

		  //insert_bp(current_reg_stack.reg_CS, adresse_ret_1);
		  set_hw_bp(translate_logic_to_linear(current_reg_stack.reg_CS, adresse_ret_1));
		  /*printf_disasm(code_ptr);*/
		  break;


		case 0xff:
		  switch(adresse_ret_2)
		    {
		    case 0:
		      val = reg_stack->reg_EAX;
		      break;
		    case 1:
		      val = reg_stack->reg_EBX;
		      break;
		    case 2:
		      val = reg_stack->reg_ECX;
		      break;
		    case 3:
		      val = reg_stack->reg_EDX;
		      break;
		    case 4:
		      val = reg_stack->reg_ESI;
		      break;
		    case 5:
		      val = reg_stack->reg_EDI;
		      break;
		    case 6:
		      val = reg_stack->reg_EIP;
		      break;
		    case 7:
		      val = reg_stack->reg_ESP;
		      break;
		    case 8:
		      val = reg_stack->reg_EBP;
		      break;


		    case 9:
		      val = get_dr0_val();
		      break;
		    case 10:
		      val = get_dr1_val();
		      break;
		    case 11:
		      val = get_dr2_val();
		      break;
		    case 12:
		      val = get_dr3_val();
		      break;
		    case 13:
		      val = get_dr6_val();
		      break;
		    case 14:
		      val = get_dr7_val();
		      break;



		    default:
		      val = 0xDEADFACE;
		      break;

		    }

		  Sprintf(debug__, "%.8X", val);
		  insertinbuffer(debug__);

		  break;

		case 14:
		  insertinbuffer("Loading symbol man... asking ring 3 :)");
		  if (page_exist(adresse_ret_1))
		    ret = load_symbol((Symbol*) adresse_ret_1, adresse_ret_2);
		  else
		    insertinbuffer("bad load adress..");
		  //#endif
		  Sprintf(debug__,"number of symbol loaded: %d", ret);
		  insertinbuffer(debug__);
		  if (ret>=MAX_SYMBOL)
		    {
		      insertinbuffer("YuP! Not enought static memory for all those symbols man!");
		    }

		  break;

		case 15:

		  display_page(adresse_ret_1);
		  get_page_phys(adresse_ret_1);


		  break;


		case 16:
		  insertinbuffer("------");
		  list_symbol();
		  insertinbuffer("------");
		  break;

		case 17:
		  if (cursor_X == 0 && cursor_Y == 0)
		    {
		      cursor_X = 15;
		      cursor_Y = data_win_y;
		    }

		  cursor_old_color = table_color[cursor_Y][cursor_X];
		  update_cursor(cursor_X, cursor_Y);
		  insertinbuffer("MeM Edit mode");
		  break;


		case 18:

		  Sprintf(debug__, "inthook N=%d ", adresse_ret_1);
		  insertinbuffer(debug__);


		  if (adresse_ret_1 >=MAX_INT)
		    {
		      insertinbuffer("too big int, man");
		      break;
		    }

		  if (is_int_hooked[adresse_ret_1])
		    {
		      tmp_int[adresse_ret_1] =(void *) modifie_idt( (unsigned int)old_int[adresse_ret_1],(unsigned int*)idt_[0]+2*adresse_ret_1);
		    }
		  else
		    {
		      if (addrnewint[adresse_ret_1]==(unsigned long)NULL)
			{
			  Sprintf(debug__, "humm, rr0d doesn't know how to handle int %d yet, man", adresse_ret_1);
			  insertinbuffer(debug__);
			  break;


			}
		      old_int[adresse_ret_1] =(void *) modifie_idt( (unsigned int)addrnewint[adresse_ret_1],(unsigned int*)idt_[0]+2*adresse_ret_1);

		    }

		  is_int_hooked[adresse_ret_1]= is_int_hooked[adresse_ret_1]?0:1;

		  break;

		case 19:
		  /*
		  Sprintf(debug__, "AV cr4: %.8X",get_cr4_val()); 
		  Out_Debug_String(debug__);
		  log_line_in_buffer(debug__);
		  */
		  
		  /*TEST RDTSC disable*/
		  if (hook_rdtsc)
		    set_cr4_val(get_cr4_val()&(~0x4));
		  else
		    set_cr4_val(get_cr4_val()|(0x4));

		  hook_rdtsc = hook_rdtsc?0:1;
		  
		  Sprintf(debug__, "AP cr4: %.8X",get_cr4_val()); 
		  Out_Debug_String(debug__);
		  log_line_in_buffer(debug__);
		  


		  break;

		case 20:
		  fOOl_mode = 1;
		  Refresh_ALL();
		  break;

		case 21:
		  fOOl_mode = 0;
		  Refresh_ALL();
		  break;



		default:

		  Sprintf(debug__, "%.8X", adresse_ret_1);
		  insertinbuffer(debug__);
						 
		  break;

		}
					
	    }
	  else
	    {
	      insertinbuffer("BaD Command man");

	    }
				  

	  ClearCommand();
	  delline(cmd_win_y);

	  displaybuffer();
	  Refresh_AW();
				  
	};
      break;

    case KBDEL: if (fired) DeleteCommandKey();  /* Delete the last character pressed */
      break;
     
    case KBPAGEUP:/*  if ((infobuffer==ACTIVADO) && (fired))
		      bufferup();
		      else if ((disasmindex!=0) && (fired))
		      {retrocededesasm();
		      };*/
      if (fired)
	{
	  if (edit_mode)
	    {
	      update_cursor(cursor_X, cursor_Y-10);
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(!shift_key))
	    {
	      bufferbigup();
	      displaybuffer();
	    }

	  if (((alt_key)&&(!ctrl_key)&&(!shift_key)))
	    {
	      data_ptr-=0x100;

	    }


	  if ((!alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      /*
		len=0;
		for (i=0;i<code_win_dim;i++)
		{
		len += code_dim_line[i];

		}
		code_ptr-=len?len:0x20;*/
	      len= back_disasm (code_ptr, code_win_dim);
	      code_ptr-=len?len:0x20;
	      force_disasm = 1;
	      update_disasm = 1;


	    }

	}


      break;
    case KBPAGEDOWN:

		 
      if (fired)
	{
	  if (edit_mode)
	    {
	      update_cursor(cursor_X, cursor_Y+10);
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(!shift_key))
	    {
	      bufferbigdown();
	      displaybuffer();
	    }

	  if (((alt_key)&&(!ctrl_key)&&(!shift_key)))
	    {
	      data_ptr+=0x100;

	    }


	  if ((!alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      len=0;
	      for (i=0;i<code_win_dim;i++)
		{
		  len += code_dim_line[i];

		}
	      code_ptr+=len?len:0x20;
	      force_disasm = 1;
	      update_disasm = 1;
	    }

	}

      break;
                    
    case KBF12:

      break;

    case KBCURSORUP:             /* I move up in the hexadecimal display windows */

      if (fired)
	{
	  if (edit_mode)
	    {
	      update_cursor(cursor_X, cursor_Y-1);
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(!shift_key))
	    {
	      bufferup();
	      displaybuffer();
	      break;
	    }

	  if (((alt_key)&&(!ctrl_key)&&(!shift_key)))
	    {
	      data_ptr-=0x10;
	      break;

	    }


	  if ((!alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      /*
		code_ptr-=code_dim_line[0];
	      */
	      len= back_disasm (code_ptr, 1);
	      code_ptr-=len?len:3;
	      force_disasm = 1;
	      update_disasm = 1;
	      break;
					
	    }


	  if ((alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      screen_offset_y--;
	      deblin_console_on();
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(shift_key))
	    {
	      recall_oldcmd(-1);
	      Refresh_Display();
	      break;
	    }



	}
      break;
 
    case KBCURSORDOWN:           /* I move down in the hexadecimal display windows */

      if (fired)
	{

	  if (edit_mode)
	    {
	      update_cursor(cursor_X, cursor_Y+1);
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(!shift_key))
	    {
	      bufferdown();
	      displaybuffer();
	      break;
	    }

	  if (((alt_key)&&(!ctrl_key)&&(!shift_key)))
	    {
	      data_ptr+=0x10;
	      break;

	    }

	  if ((!alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      code_ptr+=code_dim_line[0];
	      force_disasm = 1;
	      update_disasm = 1;
	      break;
	    }


	  if ((alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      screen_offset_y++;
	      deblin_console_on();
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(shift_key))
	    {
	      recall_oldcmd(1);
	      Refresh_Display();
	      break;
	    }


	}
      break;

    case KBCURSORLEFT:
		      
      if (fired)
	{

	  if (edit_mode)
	    {
	      update_cursor(cursor_X-1, cursor_Y);
	      break;
	    }

	  if ((alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      screen_offset_x--;
	      deblin_console_on();
	      break;
	    }

	}
      break;

    case KBCURSORRIGHT:
		      
      if (fired)
	{

	  if (edit_mode)
	    {
	      update_cursor(cursor_X+1, cursor_Y);
	      break;
	    }

	  if ((alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      screen_offset_x++;
	      deblin_console_on();
	      break;
	    }

	}
      break;


    case KBESC:
      edit_mode = 0;
      erase_cursor();
      break;



    case KBLEFTALT:             /* Activo desactivo Trap Mode */

      break;

    case KBRIGHTSHIFT:

    case KBLEFTSHIFT:

      break;

    case KBUP:

      break;

    case 0:
      break;

    default:                
		 
      if (!fired)
	break;

      if (!edit_mode)
	{
	  InsertCommandKey(car);
	  break;
	}

      if (!((car>='0' &&  car <='9')||
	    (car>='a' &&  car <='f')||
	    (car>='A' &&  car <='F')))
	break;
      
      if ((car>='0' &&  car <='9'))
	car-='0';
	      
      if ((car>='a' &&  car <='f'))
	car=car-'a'+10;
	      
      if ((car>='A' &&  car <='F'))
	car=car-'A'+10;
	      
	      
      local_data_ptr = data_ptr + (cursor_Y-data_win_y)*0x10 + (cursor_X-14)/3;
      /*
	Sprintf(debug__, "ad: %.8X : %d", local_data_ptr, (cursor_X)%3);
	insertinbuffer(debug__);
	displaybuffer();
	Refresh_AW();
      */
	      
      //InsertCommandKey(car);
      if (page_exist(local_data_ptr ))
	{
	  local_data_byte = *((unsigned char*)local_data_ptr);
	  switch((cursor_X)%3)
	    {
	    case 0:
	      local_data_byte&=0xf;
	      local_data_byte+=car<<4;
	      break;
		      
	    case 1:
	      local_data_byte&=0xf0;
	      local_data_byte+=car;
	      break;
		      
		      
	    }
	  write_save_dirty( (unsigned char*)local_data_ptr, (unsigned char)local_data_byte);
		  
	}
	      
      update_cursor(cursor_X+1, cursor_Y);
      dump_data(current_reg_stack.reg_DS, (unsigned char*)data_ptr);
      /*
	Refresh_DW();
	Refresh_Display();
      */      
            
    };
           
}
