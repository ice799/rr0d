/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id: core_rr0d.c,v 1.27 2006-07-11 07:09:29 serpilliere Exp $
 *
 */

#include "vars.h"

#include "nasm.h"
#include "disasm.h"


#include "utils.h"
#include "video.h"
#include "fb_pci_agp.h"

#include "var_globale.h"

#include "keyboard.h"
#include "idt.h"
#include "page.h"
#include "font_256.h"
#include "buffering.h"
#include "command.h"
#include "breakpoint.h"
#include "import_symb.h"

#include "pci.h"


#include "core_rr0d.h"

unsigned int stackptr;

void *oldkbd;
void *old_int[MAX_INT];

void *tmpkbd;

void *tmp_int[MAX_INT];


unsigned long addrnewkbd;
unsigned long addrnewint[MAX_INT];


int is_int_hooked[MAX_INT];


char debug_[1024]={0};
unsigned char key_scan_code=0;
extern unsigned int ctrl_key;
extern unsigned int shift_key;
extern unsigned int alt_key;
int fired = 0;

unsigned int gdt_limit;

unsigned long idt_[2];
unsigned long gdt_[2];
unsigned long ldt_[1];

unsigned int data_ptr;
unsigned int old_data_ptr;
unsigned int update_disasm=1;
unsigned int force_disasm=0;
unsigned int code_ptr;
unsigned int old_code_ptr;

Reg_struct current_reg_stack;
Reg_struct *reg_stack;

unsigned int current_CS;
unsigned int current_DS;


unsigned int ring0_ES;
unsigned int ring0_FS;
unsigned int ring0_GS;


unsigned int in_hlt=0;







void print_reg(Reg_struct *regstack);
void printf_key_state (unsigned int o );
void dump_data(unsigned int selector, unsigned char * ptr);
void step_by_step (void);


Reg_struct old_reg_data = {0};
/*                                                */
/*affiche le contenu des registres du programme   */
/*qui a été interrompu                            */
/*                                                */
/*BUG: affichage des GS FS en 16bit               */
void print_reg(Reg_struct *regstack)
{

  Sprintf( debug_, "%.8X", regstack->reg_EAX);
  DisplayText(5 , 1, debug_);
  if (regstack->reg_EAX== old_reg_data.reg_EAX)
    rr0d_memcpy(&table_color[1][5], "        ", 8);
  else
    rr0d_memcpy(&table_color[1][5], "11111111", 8);
  
  Sprintf( debug_, "%.8X", regstack->reg_EBX);
  DisplayText(18, 1, debug_);
  if (regstack->reg_EBX== old_reg_data.reg_EBX)
    rr0d_memcpy(&table_color[1][18], "        ", 8);
  else
    rr0d_memcpy(&table_color[1][18], "11111111", 8);

  Sprintf( debug_, "%.8X",regstack->reg_ECX );
  DisplayText(31, 1, debug_);
  if (regstack->reg_ECX== old_reg_data.reg_ECX)
    rr0d_memcpy(&table_color[1][31], "        ", 8);
  else
    rr0d_memcpy(&table_color[1][31], "11111111", 8);

  Sprintf( debug_, "%.8X",regstack->reg_EDX );
  DisplayText(44, 1, debug_);
  if (regstack->reg_EDX== old_reg_data.reg_EDX)
    rr0d_memcpy(&table_color[1][44], "        ", 8);
  else
    rr0d_memcpy(&table_color[1][44], "11111111", 8);

  Sprintf( debug_, "%.8X",regstack->reg_EDI );
  DisplayText(57, 1, debug_);
  if (regstack->reg_EDI== old_reg_data.reg_EDI)
    rr0d_memcpy(&table_color[1][57], "        ", 8);
  else
    rr0d_memcpy(&table_color[1][57], "11111111", 8);


  Sprintf( debug_, "%.8X",regstack->reg_ESI );
  DisplayText(5 , 2, debug_);
  if (regstack->reg_ESI== old_reg_data.reg_ESI)
    rr0d_memcpy(&table_color[2][5], "        ", 8);
  else
    rr0d_memcpy(&table_color[2][5], "11111111", 8);

  Sprintf( debug_, "%.8X",regstack->reg_EBP );
  DisplayText(18, 2, debug_);
  if (regstack->reg_EBP== old_reg_data.reg_EBP)
    rr0d_memcpy(&table_color[2][18], "        ", 8);
  else
    rr0d_memcpy(&table_color[2][18], "11111111", 8);

  Sprintf( debug_, "%.8X",regstack->reg_ESP );
  DisplayText(31, 2, debug_);
  if (regstack->reg_ESP== old_reg_data.reg_ESP)
    rr0d_memcpy(&table_color[2][31], "        ", 8);
  else
    rr0d_memcpy(&table_color[2][31], "11111111", 8);

  Sprintf( debug_, "%.8X",regstack->reg_EIP );
  DisplayText(44, 2, debug_);
  if (regstack->reg_EIP== old_reg_data.reg_EIP)
    rr0d_memcpy(&table_color[2][44], "        ", 8);
  else
    rr0d_memcpy(&table_color[2][44], "11111111", 8);


  debug_[4]=0;
  Sprintf(debug_, "%.2X", regstack->reg_CS);
  DisplayText(5 , 3, debug_);
  if (regstack->reg_CS== old_reg_data.reg_CS)
    rr0d_memcpy(&table_color[3][5], "        ", 8);
  else
    rr0d_memcpy(&table_color[3][5], "11111111", 8);

  Sprintf(debug_, "%.2X", regstack->reg_DS);
  DisplayText(18, 3, debug_);
  if (regstack->reg_DS== old_reg_data.reg_DS)
    rr0d_memcpy(&table_color[3][18], "        ", 8);
  else
    rr0d_memcpy(&table_color[3][18], "11111111", 8);

  Sprintf(debug_, "%.2X", regstack->reg_ES);
  DisplayText(31, 3, debug_);
  if (regstack->reg_ES== old_reg_data.reg_ES)
    rr0d_memcpy(&table_color[3][31], "        ", 8);
  else
    rr0d_memcpy(&table_color[3][31], "11111111", 8);

  Sprintf(debug_, "%.2X", regstack->reg_SS);
  DisplayText(44, 3, debug_);
  if (regstack->reg_SS== old_reg_data.reg_SS)
    rr0d_memcpy(&table_color[3][44], "        ", 8);
  else
    rr0d_memcpy(&table_color[3][44], "11111111", 8);

  Sprintf(debug_, "%.2X", regstack->reg_FS);
  DisplayText(57, 3, debug_);
  if (regstack->reg_FS== old_reg_data.reg_FS)
    rr0d_memcpy(&table_color[3][57], "        ", 8);
  else
    rr0d_memcpy(&table_color[3][57], "11111111", 8);

  Sprintf(debug_, "%.2X", regstack->reg_GS);
  DisplayText(70, 3, debug_);
  if (regstack->reg_GS== old_reg_data.reg_GS)
    rr0d_memcpy(&table_color[3][70], "        ", 8);
  else
    rr0d_memcpy(&table_color[3][70], "11111111", 8);


  Sprintf(debug_, "%.8X", key_scan_code);
  DisplayText(43, cmd_win_y+1, debug_);


  DisplayText(53, 2, "FLG=");


  Sprintf( debug_, "%c%c%c%c%c%c%c%c%c",
	   regstack->reg_EFLAG&0x800?'O':'o',
	   regstack->reg_EFLAG&0x400?'D':'d',
	   regstack->reg_EFLAG&0x200?'I':'i',
	   regstack->reg_EFLAG&0x100?'T':'t',
	   regstack->reg_EFLAG&0x80 ?'S':'s',
	   regstack->reg_EFLAG&0x40 ?'Z':'z',
	   //20
	   regstack->reg_EFLAG&0x10 ?'A':'a',
	   //8
	   regstack->reg_EFLAG&0x4 ?'P':'p',
	   //2
	   regstack->reg_EFLAG&0x1 ?'C':'c' );
  DisplayText(57, 2, debug_);


  table_color[2][57]= (regstack->reg_EFLAG&0x800)==(old_reg_data.reg_EFLAG&0x800)?' ':'1';
  table_color[2][58]= (regstack->reg_EFLAG&0x400)==(old_reg_data.reg_EFLAG&0x400)?' ':'1';
  table_color[2][59]= (regstack->reg_EFLAG&0x200)==(old_reg_data.reg_EFLAG&0x200)?' ':'1';
  table_color[2][60]= (regstack->reg_EFLAG&0x100)==(old_reg_data.reg_EFLAG&0x100)?' ':'1';
  table_color[2][61]= (regstack->reg_EFLAG&0x80) ==(old_reg_data.reg_EFLAG&0x80)?' ':'1';
  table_color[2][62]= (regstack->reg_EFLAG&0x40) ==(old_reg_data.reg_EFLAG&0x40)?' ':'1';
  table_color[2][63]= (regstack->reg_EFLAG&0x10) ==(old_reg_data.reg_EFLAG&0x10)?' ':'1';
  table_color[2][64]= (regstack->reg_EFLAG&0x4)  ==(old_reg_data.reg_EFLAG&0x4)?' ':'1';
  table_color[2][65]= (regstack->reg_EFLAG&0x1)  ==(old_reg_data.reg_EFLAG&0x1)?' ':'1';

  if (fired)
    Sprintf(debug_, "%.8X", 0xDEADBEEF);
  else
    Sprintf(debug_, "%.8X", 0);

  DisplayText(43, cmd_win_y+2, debug_);

  Refresh_Display();

}


unsigned int uuu;
/*affiche les codes des touches appuyées          */
void printf_key_state (unsigned int oo)
{

  
  Sprintf(debug_, "Key: %.8X", oo);
  DisplayText(52, cmd_win_y+1, debug_);

  Sprintf(debug_, "shift: %.1X ctrl: %.1X alt: %.1X", 
	  shift_key, 
	  ctrl_key,
	  alt_key);
  DisplayText(52, cmd_win_y, debug_);
  Refresh_Display();
  
}


unsigned int asm_lines_adresse[100]={0};
unsigned int asm_lines_adresse_n = 0;


char asm_line[78] = "                                                                              ";

char tab_true_byte[500];

/*affiche le contenu de la fenetre de code        */
/*disasm le code a l'adresse specifie             */
/*si la page memoire de l'adresse specifiee       */
/*pas, affiche le disasm de ?????                 */
/*cache egalement les octet des brea points       */
/*BUG: lors de la remontee du code asm, mauvaise  */
/*adresse. (retro dasm...)                        */
/*                                                */
void printf_disasm(unsigned int selector, unsigned char* current_eip)
{
  unsigned int i, ii;
  unsigned int vic;
  unsigned int decal_printf=0;
  unsigned int x=0;
  unsigned int index_chaine=0;

  unsigned int virtual_eip;

  unsigned char * symbol_name;
  unsigned int j;
  char asm_out[80];
  insn my_ins;

  if (current_eip != 0)
    code_ptr = (unsigned int)current_eip;


  selector = selector&0xFFFF;
  virtual_eip = translate_logic_to_linear(selector, (unsigned int)current_eip);
  current_eip = (unsigned char *)virtual_eip;

  if ( (!page_exist(virtual_eip)) || (!page_exist(virtual_eip+0x70)) )
    {
      for (i=0;i<0x70;i++)
	tab_true_byte[i] = '?';
    }
  else
    {
      for (i=0;i<0x70;i++)
	tab_true_byte[i] = true_byte_bp(&current_eip[i]);
    }



  asm_lines_adresse_n = 0;		
  for (i=0;i+decal_printf<code_win_dim;i++,asm_lines_adresse_n++)
    {
      index_chaine=0;

      if ((is_breakpoint(&current_eip[x])!=-1)||
	  (is_hw_breakpoint(&current_eip[x])))
	{
	  for (ii=14;ii<78;ii++)
	    table_color[code_win_y+i][ii]='1';
	}else if ((unsigned int)&current_eip[x] == translate_logic_to_linear(selector, (unsigned int)reg_stack->reg_EIP))
	  {
	    for (ii=14;ii<78;ii++)
	      table_color[code_win_y+i][ii]='2';
	  }
      else
	{
	  for (ii=14;ii<78;ii++)
	    table_color[code_win_y+i][ii]=' ';
	}

      /*test and print symbol...*/
      //		printk("%X\n", &current_eip[x]);
      //#if 0
      if ((symbol_name = is_symbol_address((unsigned int)&current_eip[x]))!=NULL)
	{


	  for (j=1;j<15;j++)
	    {
	      put_XY_char(j, code_win_y+i+decal_printf, ' ');
	    }

	  j=0;
	  while(symbol_name[j])
	    {
	      put_XY_char(j+15, code_win_y+i+decal_printf, symbol_name[j]);
	      j++;
	    }

	  put_XY_char(j+15, code_win_y+i+decal_printf, ':');
		   
	  for (j++;j<78-15;j++)
	    {
	      put_XY_char(j+15, code_win_y+i+decal_printf, ' ');

	    }

	  decal_printf++;
	  //		    printf("symbol found:) %s", symbol_name);
	}
      //#endif

      if (!(i+decal_printf<code_win_dim))
	break;

      /*end symbol*/
      index_chaine+=Sprintf(asm_line, "%.4X:%.8X ", selector,&current_eip[x]);

      asm_lines_adresse[asm_lines_adresse_n++] = (unsigned int)&current_eip[x];
      vic=disasm(&tab_true_byte[x], asm_out,32,(long)&current_eip[x],0, &my_ins);
      if (!vic)
	{
	  Sprintf(asm_out, "0x%.2X", current_eip[x]);
	  vic=1;
	}

      code_dim_line[i] = vic;

      for (ii = 0 ; ii<vic;ii++)
	index_chaine+=Sprintf(asm_line+index_chaine, "%.2X", tab_true_byte[x+ii]&0xff);

      for (;ii<10;ii++)
	{
	  asm_line[index_chaine++] = ' ';
	  asm_line[index_chaine++] = ' ';
	}

      index_chaine+=Sprintf(asm_line+index_chaine, "%s", asm_out);

      for (ii=index_chaine;ii<78;ii++)
	asm_line[index_chaine++] = ' ';

      if ((unsigned int)&current_eip[x] == translate_logic_to_linear(selector, (unsigned int)reg_stack->reg_EIP))
	asm_line[77] ='<'; 
      print_CW(asm_line, i+decal_printf);

      x+=vic;
    }

  Refresh_CW();
  Refresh_Display();

}




unsigned char scratch[0x100]="\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
";
/*affiche la fenetre data. si les pages memoire   */
/*de l'adresse ne sont pas presente, affiche des  */
/*?????                                           */
void dump_data(unsigned int selector, unsigned char * ptr)
{
  unsigned int i, ii;
  unsigned char *ptr_data;
  unsigned int virtual_eip;
  unsigned char buff[1024];
  unsigned int index_chaine;

	
  if (ptr == 0)
    ptr = (unsigned char*)data_ptr;
  else
    data_ptr = (unsigned int)ptr;

  ptr_data = ptr;

  selector = selector&0xFFFF;
  virtual_eip = translate_logic_to_linear(selector, (unsigned int)ptr);
  ptr = (unsigned char *)virtual_eip;



  for (i=0;i<0x10 * data_win_dim;i++)
    {
      if ( (!page_exist(virtual_eip+i)) )
	tab_true_byte[i] = scratch[i];
      else
	tab_true_byte[i] = true_byte_bp(&ptr[i]);
    }



  /*print data en hexa*/

  for (i=0;i<data_win_dim;i++)
    {
      index_chaine=0;
      index_chaine+=Sprintf(buff, "%.4X:%.8X ", selector,&ptr[i*0x10]);

      for (ii=0;ii<0x10;ii++)
	index_chaine+=Sprintf(buff+index_chaine, "%.2X ", tab_true_byte[i*0x10+ii]&0xFF);
      for (ii=0;ii<0x10;ii++)
	index_chaine+=Sprintf(buff+index_chaine, "%c", tab_true_byte[i*0x10+ii]?tab_true_byte[i*0x10+ii]&0xff:'.');
      for (ii=0;ii<78;ii++)
	put_XY_char(ii+1, data_win_y+i, buff[ii]);
	 

    }


  Refresh_DW();
  Refresh_Display();

}


#if defined(__GNUC__)
#define DUMMY_ERR_CODE\
   _asm_("\t"\
	"pushl $0xBADCAFFE\n\t"\
);

/*TODO: BUG: ERR de prot gnrale a cause de la restauration de GS ?!? */


#define HOOK_INT_HEADER_ASM(old_hooker)\
  _asm_("\t"\
	"pushl $0xDEADBEEF\n\t"\
	"push %ds\n\t"\
	"push "\
	RING_HOOO_SEGMENT\
	"\n\t"\
	"pop %ds\n\t"\
	"push %gs\n\t"\
	"push %fs\n\t"\
	"push %es\n\t"\
        "mov ring0_ES, %es\n\t"\
        "mov ring0_FS, %fs\n\t"\
	"pusha\n\t"\
	"movl %esp, reg_stack\n\t"\
  );\
  reg_stack->reg_dummy_ret = (unsigned int)old_hooker;



#define HOOK_INT_END_NOERR_ASM(hooker)\
  _asm_("\t"\
	"popa\n\t"\
	"pop %es\n\t"\
	"pop %fs\n\t"\
	"pop %gs\n\t"\
	"cmp $0, fired\n\t"\
	"pop %ds\n\t"\
	"jz not_"hooker"\n\t"\
	"addl $4, %esp\n\t"\
	"addl $4, %esp\n\t"\
	"iret\n\t"\
"not_"hooker":\n\t"\
	"ret $0x4\n\t"\
	);


#define HOOK_INT_END_ERR_ASM(hooker)\
  _asm_("\t"\
	"popa\n\t"\
	"pop %es\n\t"\
	"pop %fs\n\t"\
	"pop %gs\n\t"\
	"cmp $0, fired\n\t"\
	"pop %ds\n\t"\
	"jz not_"hooker"\n\t"\
	"addl $4, %esp\n\t"\
	"addl $4, %esp\n\t"\
	"iret\n\t"\
"not_"hooker":\n\t"\
	"ret\n\t"\
	);

#elif defined(_MSC_VER)

#define DUMMY_ERR_CODE\
   _asm{push 0xBADCAFFE};

#define HOOK_INT_HEADER_ASM(old_hooker)\
{\
  __asm{	push 0xDEADBEEF}\
  __asm{	push ds}\
  __asm{	push RING_HOOO_SEGMENT}\
  __asm{	pop ds}\
  __asm{	push gs}\
  __asm{	push fs}\
  __asm{	push es}\
  __asm{	pushad}\
  __asm{	mov reg_stack, esp}\
  };\
  reg_stack->reg_dummy_ret = (unsigned int)old_hooker;


#define HOOK_INT_END_NOERR_ASM(hooker)\
{\
__asm{	popad}\
__asm{	pop es}\
__asm{	pop fs}\
__asm{	pop gs}\
__asm{	cmp fired, 0}\
__asm{	pop ds}\
__asm{	jz not_hook}\
__asm{	add esp, 4}\
__asm{	add esp, 4}\
__asm{	iretd}\
__asm{not_hook:}\
__asm{	ret 4}\
	};

#define HOOK_INT_END_ERR_ASM(hooker)\
{\
__asm{	popad}\
__asm{	pop es}\
__asm{	pop fs}\
__asm{	pop gs}\
__asm{	cmp fired, 0}\
__asm{	pop ds}\
__asm{	jz not_hook}\
__asm{	add esp, 4}\
__asm{	add esp, 4}\
__asm{	iretd}\
__asm{not_hook:}\
__asm{	ret}\
	};

#endif



 
int num_old_bp=-1;
int step_on=0;
int spawn_bp=0;
int spawn_hw_bp=0;
 
int id_hw_bp=0;
 
/*handler du clavier:                             */
/*fonction appelee par l'interruption declenchee  */
/*par l'appuie d'une touche. appele le            */
/*de touche (commande....)                        */
/*BUG: scancode extended, (double octet en        */
/*parametre )                                     */
 
 
void _kbdhandle (void)
{
  unsigned int oo;
  /* needed by "boucle de tempo pour ack du 8259" */
  int i;
     

  //oo = print_keystroke();
     
  oo = read_scancode();
  handle_scancode((unsigned char)oo);


  if (fired)
    {
      
      if (old_data_ptr!= data_ptr)
	{
	  dump_data (current_reg_stack.reg_DS, (unsigned char*)data_ptr);
	  old_data_ptr = data_ptr;
	}

      if (old_code_ptr!= code_ptr)
	{

	  printf_disasm (current_reg_stack.reg_CS, (unsigned char*)code_ptr);
	  old_code_ptr = code_ptr;
	}
      
      printf_key_state(oo);

    }

  printf_key_state(oo);

  /* boucle de tempo pour ack du 8259 */
  for(i=0;i<100000;i++);


}
 
void __declspec_naked kbdhandle(void)
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(oldkbd);
  print_keystroke();
  //_kbdhandle();
  //HOOK_INT_END_NOERR_ASM("kbh");


#if defined(__GNUC__)
 _asm_("\t"	
	"popa\n\t"
	"pop %es\n\t"
	"pop %fs\n\t"
	"pop %gs\n\t"
	"cmp $0, fired\n\t"
	"pop %ds\n\t"
	"jz not_hook\n\t"
	"pushl %eax\n\t"
	"mov $0x20, %al\n\t"
	"out %al, $0x20\n\t"
	"popl  %eax\n\t"
	"addl $4, %esp\n\t" /*sucre le ret*/
	"addl $4, %esp\n\t" /*sucre le dummy err*/
	"iret\n\t"
	"not_hook:\n\t"
	"ret $0x4\n\t"
	);
#elif defined(_MSC_VER)
__asm{	popad}
__asm{	pop es}
__asm{	pop fs}
__asm{	pop gs}
__asm{	cmp fired, 0}
__asm{	pop ds}
__asm{	jz not_hook}
__asm{	push eax}
__asm{	mov al, 20h}
__asm{	out 20h, al}
__asm{	pop     eax}
__asm{	add esp, 4}
__asm{	add esp, 4}
__asm{	iretd}
__asm{not_hook:}
__asm{	ret 4}
#endif 

}

unsigned int stack_ptr;

#define STEP_INTO	0x42
#define STEP_OVER	0x44
#define RUN		0xd8




unsigned int oo;


unsigned int saved_eflag;

/*fonction boucle appelee lors d'un pas a pas     */
/*scan les touche car on est deja dans une        */
/*interruption                                    */
/*                                                */
void step_by_step (void)
{
  int ret=0;
  insn my_ins;
  int eip_in_asm_win = 0;
  unsigned int i;

#if defined(__GNUC__)
  _asm_("push %eax;\n"
	"pushf;\n"
	"pop %eax;\n"
	"mov %eax, saved_eflag;\n"
	"pop %eax;\n"
	"cli;\n"
	);
#elif defined(_MSC_VER)
  __asm{
    push eax;
    pushf;
    pop eax;
    mov saved_eflag, eax;
    pop eax;
    cli;
  };
#endif


  dump_screen();

	
  if (fired)
    {
      //      ret =  parse_inst(current_reg_stack.reg_CS, current_reg_stack.reg_EIP, &my_ins);
      ret =  parse_inst(reg_stack->reg_CS, reg_stack->reg_EIP, &my_ins);


      debug_[0] = '0'+ret;	
      debug_[1] = 'Ä';

      switch(my_ins.opcode)
	{

	case I_CALL:
	  strcpy(&debug_[2], "CALL");
	  debug_[6] = 0;
	  break;
	case I_PUSHF:
	case I_PUSHFD:
	  strcpy(&debug_[2], "pushf/d");
	  debug_[6] = 0;
	  break;
	case I_POPF:
	case I_POPFD:
	  strcpy(&debug_[2], "popf/d");
	  debug_[6] = 0;
	  break;
	case I_CLI:
	  strcpy(&debug_[2], "cli");
	  debug_[6] = 0;
	  break;
	case I_STI:
	  strcpy(&debug_[2], "int XX");
	  debug_[6] = 0;
	  break;
	case I_INT:
	  break;
	default:
	  strcpy(debug_, "ÄÄÄÄÄÄÄÄÄÄÄÄ");
	  debug_[10]=0;
	  ret = 0;
	  break;
	}

      DisplayText(52, cmd_win_y-1, debug_);
	  
    }
	

  do
    {
      if (fired)
	{
	  key_scan_code = (unsigned char)read_scancode();
	  handle_scancode(key_scan_code);
			

	  //if (old_data_ptr!= data_ptr)
	    {
	      dump_data(current_reg_stack.reg_DS, (unsigned char*)data_ptr);
	      old_data_ptr = data_ptr;
	    }

	  eip_in_asm_win = 0;
	  for (i=0;i<asm_lines_adresse_n;i++)
	    if (translate_logic_to_linear(reg_stack->reg_CS, (unsigned int)reg_stack->reg_EIP) == asm_lines_adresse[i])
	      {
		eip_in_asm_win = 1;
		break;
	      }

	  if (!eip_in_asm_win && !force_disasm)
	    {
	      code_ptr = reg_stack->reg_EIP;
	      update_disasm=1;
	    }
			

	  if (update_disasm)
	    {
	      update_disasm = 0;
	      printf_disasm( current_reg_stack.reg_CS, (unsigned char*)/*code_ptr*/code_ptr);
	    }

	  print_reg(reg_stack);
			
	  
	  if (key_scan_code)
	    printf_key_state(key_scan_code);
	  	
			
	}


    }while( ((key_scan_code & 0xFF) != STEP_INTO) &&
	    ((key_scan_code & 0xFF) != STEP_OVER) &&
	    ((key_scan_code & 0xFF) != RUN)	&&
	    (fired));

  switch((key_scan_code & 0xFF))
    {
    case STEP_INTO:
      restore_screen(0);
      break;
    default:
      restore_screen(1);
      break;
    }



#if defined(__GNUC__)
  _asm_("push %eax;\n"
	"mov saved_eflag, %eax;\n"
	"push %eax;\n"
	"popf;\n"
	"pop %eax;\n"
	);

#elif defined(_MSC_VER)
  __asm{
    push eax;
    mov eax, saved_eflag;
    push eax;
    popf;
    pop eax;
  };
#endif



	
  if ((key_scan_code & 0xFF) == RUN)
    {
      reg_stack->reg_EFLAG= 0xfffffeff & reg_stack->reg_EFLAG;
      step_on = 0;
      //		fired=0;
    }

  if ( ((key_scan_code & 0xFF) == STEP_INTO) || (((key_scan_code & 0xFF) == STEP_OVER)&&(!ret)))
    {
      step_on = 1;
      //in case the sucker did popf with tf off ;)
      //humm, I love this smell
      reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
    }


  if (((key_scan_code & 0xFF) == STEP_OVER)&&(ret))
    {

      step_on = 0;
      reg_stack->reg_EFLAG= 0xfffffeff & reg_stack->reg_EFLAG;
      insert_bp(current_reg_stack.reg_CS, current_reg_stack.reg_EIP+ret, BP_ONE_SHOT);
    }


  old_reg_data = *reg_stack;

}


/*handler de la division par 0                    */
void  _int0_handle (void)
{

  if (!fired)
    {
      Refresh_ALL();
      Refresh_Display();
    }

  current_reg_stack = *reg_stack;
  force_disasm = 0;
  update_disasm = 1;
  /*
    dump_data(reg_stack->reg_DS, (unsigned char*)reg_stack->reg_EIP);
    printf_disasm(reg_stack->reg_CS, (unsigned char*)reg_stack->reg_EIP);
    print_reg(reg_stack);	
  */
  fired=1;
  
  if (fired)
    {	insertinbuffer("(_.-div 0  h00keD-._)");	}
  else
    {	insertinbuffer("(_.-div 0  ziiip -._)");	}
  
  displaybuffer();
  Refresh_AW();
  Refresh_Display();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  step_by_step();

}

void __declspec_naked int0_handle (void)
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[0]);
  _int0_handle();
  HOOK_INT_END_NOERR_ASM("int0");
};


unsigned int dr7_value=0;
unsigned int old_id_hw_bp=0;

/*handler de l'interruption 1                     */
/*(appelee quant le trap flag est a 1 (pas a pas) */
/*                                                */
/*                                                */
void  _int1_handle (void) 
{


  current_reg_stack = *reg_stack;
  fired = 1;
  force_disasm = 0;
  update_disasm = 1;

  set_dr7_val(dr7_value);  
  Sprintf(debug_, "int1: %X", reg_stack->reg_EIP);
  insertinbuffer(debug_);


  //printk("int1 enter dr7 val:%X\n", get_dr7_val());
  /*
    if (!is_hw_breakpoint(reg_stack->reg_EIP))
    {
    insertinbuffer("YuuuupPP tuch DRX");
  */
  //printk("ADD: %X\n", reg_stack->reg_EIP);
  /*
    dump_data(reg_stack->reg_DS, (unsigned char*)data_ptr);
    printf_disasm(reg_stack->reg_CS,  (unsigned char*)reg_stack->reg_EIP);
    print_reg(reg_stack);	
    step_on = 1;
    step_by_step();

      

    }
  */
  
  /*int ret;*/
  
  if (spawn_bp)
    {
      spawn_breakpoint(num_old_bp);
      spawn_bp = 0;
      if (step_on == 0)
	{
	  insertinbuffer("YuuuupPP1 ");
	  return;
	}
    }
	

  if (old_id_hw_bp)
    {
      enable_id_hw_bp(id_hw_bp);
      old_id_hw_bp = 0;
      if (step_on == 0)
	{
	  insertinbuffer("YuuuupPP2 ");
	  return;
	}
	    
    }


  //printk("int1-1 enter dr7 val:%X\n", get_dr7_val());
  /*
    dump_data(reg_stack->reg_DS, (unsigned char*)data_ptr);
    printf_disasm(reg_stack->reg_CS,  (unsigned char*)reg_stack->reg_EIP);
    print_reg(reg_stack);	
  */
  /*respawn hw bp*/

  //printk("int1-2 enter dr7 val:%X\n", get_dr7_val());	

  if (id_hw_bp)
    {
      enable_id_hw_bp(id_hw_bp);
    }
	
  //printk("int1-3 enter dr7 val:%X\n", get_dr7_val());

  /*if single step caused by hw bp*/

	
  if ((id_hw_bp=check_hw_bp()) != 0)
    {





      //	    set_dr7_val(get_dr7_val() | (3 << (id_hw_bp>>1)));
      /*
	switch(id_hw_bp)
	{
	case 1:
	set_dr7_val(get_dr7_val() | 0x3<<0);
	break;
	case 2:
	set_dr7_val(get_dr7_val() | 0x3<<2);
	break;
	case 4:
	set_dr7_val(get_dr7_val() | 0x3<<4);
	break;
	case 8:
	set_dr7_val(get_dr7_val() | 0x3<<6);
	break;
	default:
	insertinbuffer("YuuuupPP HW Str4ngE");
	break;


	}
      */

      //set_dr0_val(0);

      fired=1;
      insertinbuffer("(_.-HW BP SpAWN InT 1-._)");
	    
	    
      step_on=1;
      disable_id_hw_bp(id_hw_bp);


      old_id_hw_bp = id_hw_bp;

	    



    }
	

  //printk("int1-4 enter dr7 val:%X\n", get_dr7_val());

  if (step_on)
    {
      if (fired)
	{	insertinbuffer("(_.-int  1 h00keD-._)");	}
      else
	{	insertinbuffer("(_.-int  1 ziiip -._)");	}

      displaybuffer();
      Refresh_AW();
      Refresh_Display();
	


      step_by_step();

      if (id_hw_bp)
	reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
    }
  else
    {
      reg_stack->reg_EFLAG= 0xfffffeff & reg_stack->reg_EFLAG;
    }

  /*TEST TEST ice 386*/

  dr7_value = get_dr7_val();
  /*
  set_dr6_val( 0);
  set_dr6_val( get_dr6_val() | (1 << 13));
  */
  //printk("int ret dr7 val:%X\n", get_dr7_val());


}

void __declspec_naked int1_handle (void) 
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[1]);
  _int1_handle();
  HOOK_INT_END_NOERR_ASM("int1");


};


int bp_num;
int test_=0;

/*handler de l'interruption 3                     */
/*gere les break point software, met en mode pas a*/
/*pas, ...                                        */
void  _int3_handle (void) 
{

  if (!fired)
    {
      Refresh_ALL();
      Refresh_Display();
    }

  fired=1;
  current_reg_stack = *reg_stack;
  force_disasm = 0;
  update_disasm = 1;

  reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;

  
/*
  __asm("push %eax\n"
	"push %ds\n"
	"pop %eax\n"
	"movl %eax, test_\n"
	"pop %eax\n"
	);

    printk("ds: %p\n", test_);

  __asm("push %eax\n"
	"push %cs\n"
	"pop %eax\n"
	"movl %eax, test_\n"
	"pop %eax\n"
	);

    printk("cs: %p\n", test_);
*/
  //printk("int3 enter dr7 val:%X\n", get_dr7_val());

  if ( (bp_num=is_breakpoint((unsigned char*)reg_stack->reg_EIP-1))!=-1)
    {
      reg_stack->reg_EIP--;
      if (reg_stack->reg_EIP!=list_breakpoints[bp_num].address)
	{
	  insertinbuffer("breakpoint diff mapping...changing @");
	  list_breakpoints[bp_num].address = reg_stack->reg_EIP;
	}
		
      if ((bp_num == unmapped_bp_to_shoot) && (unmapped_bp_to_shoot!=-1))
	{
	  Sprintf(debug_, "rr0d is deleting a unwanted bp@ %.8X n %d", reg_stack->reg_EIP,bp_num);
	  insertinbuffer( debug_);	

	  list_breakpoints[bp_num].address = reg_stack->reg_EIP;
	  erase_breakpoint(bp_num);
	  unmapped_bp_to_shoot = -1;
	  return;
		    
	}
		
      pass_breakpoint(bp_num);

      num_old_bp = bp_num;
      spawn_bp = 1;
    }


  //printk("int3-1 enter dr7 val:%X\n", get_dr7_val());

  if ((bp_num!=-1)&&(list_breakpoints[bp_num].attrib==BP_ONE_SHOT))
      erase_breakpoint(bp_num);


  //printk("int3-2 enter dr7 val:%X\n", get_dr7_val());
  /*
    dump_data(reg_stack->reg_DS, (unsigned char*)reg_stack->reg_EIP);
    printf_disasm(reg_stack->reg_CS, (unsigned char*)reg_stack->reg_EIP);
    print_reg(reg_stack);	
  */
  if (fired)
    {	insertinbuffer("(_.-int  3 h00keD-._)");	}
  else
    {	insertinbuffer("(_.-int  3 ziiip -._)");	}

  displaybuffer();
  Refresh_AW();
  Refresh_Display();


  //printk("int3-3 enter dr7 val:%X\n", get_dr7_val());

  step_by_step();

  //printk("int3-4 enter dr7 val:%X\n", get_dr7_val());
  if ( spawn_bp)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
 
  //printk("int3 ret dr7 val:%X\n", get_dr7_val());


  dr7_value = get_dr7_val();
}

void __declspec_naked int3_handle (void) 
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[3]);
  _int3_handle();
  HOOK_INT_END_NOERR_ASM("int3");

};



/*handler de invalid opcode                       */
void  _int6_handle (void)
{

  if (!fired)
    {
      Refresh_ALL();
      Refresh_Display();
    }

  current_reg_stack = *reg_stack;
  force_disasm = 0;
  update_disasm = 1;

  fired=1;
  
  if (fired)
    {	insertinbuffer("(_.-invalid op  h00keD-._)");	}
  else
    {	insertinbuffer("(_.-invalid op  ziiip -._)");	}
  
  displaybuffer();
  Refresh_AW();
  Refresh_Display();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  step_by_step();

}

void __declspec_naked int6_handle (void)
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[6]);
  _int6_handle();
  HOOK_INT_END_NOERR_ASM("int6");
}


/*handler d' erreur de protection generale        */
/*ps: l'interruption de la mort                   */
/*superstitieux s'abstenir, ca porte malheur.     */
void  _int13_handle (void)
{

  if (!fired)
    {
      Refresh_ALL();
      Refresh_Display();
    }

  current_reg_stack = *reg_stack;
  force_disasm = 0;
  update_disasm = 1;

  fired=1;
  
  if (fired)
    {	insertinbuffer("(_.-general prot fault  h00keD-._)");	}
  else
    {	insertinbuffer("(_.-general prot fault  ziiip -._)");	}


  displaybuffer();
  Refresh_AW();
  Refresh_Display();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  step_by_step();

}

void __declspec_naked int13_handle (void)
{
  fake_naked;

  HOOK_INT_HEADER_ASM(old_int[13]);
  _int13_handle();
  HOOK_INT_END_ERR_ASM("int13");
}


/*handler de page fault                    */

unsigned int mycr2;
void  _int14_handle (void)
{
	

  current_reg_stack = *reg_stack;

  
  dump_data(reg_stack->reg_DS, (unsigned char*)reg_stack->reg_EIP);
  printf_disasm(reg_stack->reg_CS, (unsigned char*)reg_stack->reg_EIP);
  print_reg(reg_stack);	
  
  fired=0;
  
  if (fired)
    {	insertinbuffer("(_.-div 14  h00keD-._)");	}
  else
    {	insertinbuffer("(_.-div 14  ziiip -._)");	}
  
  
  //  if (reg_stack->reg_ERROR!=4 && reg_stack->reg_ERROR!=6 && reg_stack->reg_ERROR!=7)
  {

    debug_[8]=' ';
    //  insertinbuffer( debug_+9);	
#if defined(__GNUC__)
    _asm_("pushl %eax\n"
	  "movl %cr2, %eax\n"
	  "movl %eax, mycr2\n"
	  "popl %eax\n");
#endif
    /*
      hexatoascii(mycr2, debug_+9);
      debug_[18]=0;
      insertinbuffer( debug_);	
    */
  }
  
  displaybuffer();
  Refresh_AW();
  Refresh_Display();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  step_by_step();
  
}


void __declspec_naked int14_handle (void)
{
  fake_naked;

  HOOK_INT_HEADER_ASM(old_int[14]);
  _int14_handle();
  HOOK_INT_END_ERR_ASM("int14");
}




/*handler de l'interruption 80                    */
void  _int128_handle (void)
{
  current_reg_stack = *reg_stack;
  
  dump_data(reg_stack->reg_DS, (unsigned char*)reg_stack->reg_EIP);
  printf_disasm(reg_stack->reg_CS, (unsigned char*)reg_stack->reg_EIP);
  print_reg(reg_stack);	
  
  fired=0;
  
  if (fired)
    {	insertinbuffer("(_.-int128 h00keD-._)");	}
  else
    {	insertinbuffer("(_.-int128 ziiip -._)");	}
  
  displaybuffer();
  Refresh_AW();
  Refresh_Display();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  step_by_step();

}

void __declspec_naked int128_handle (void)
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[128]);
  _int128_handle();
  HOOK_INT_END_NOERR_ASM("int128");
}



volatile unsigned int ooooooo;
int init_rr0d(void)
{
  //  int error = 0;
  unsigned  int hw_video_addess = 0;


  int dwRetVal = 0;
  unsigned int idt;
  unsigned int i;
/*
  __asm("push %eax\n"
	"push %ds\n"
	"pop %eax\n"
	"movl %eax, test_\n"
	"pop %eax\n"
	);


    printk("INIT:ds: %p\n", test_);
    printk("INIT:fb hw %p\n", fb_hw_flying_page1);
*/

/*
    
    for (i=0;i< 0x500;i++)
      {
	//ii=;
	for (ii=0x00001;ii<1000;ii++)
	  {
	    poked1(0xF0000000+4*i+4*0xE00*ii , ii/5);
	    

	  }


	
      }
  */  
#ifdef VIDEO_FB
  screen_adresse = SCREEN_ADRESSE;
#endif /* VIDEO_FB */

  font_x = 8;
  font_y = 12;
  font_bpp = 8;


  data_ptr = 0;
  old_data_ptr = 1;
  code_ptr = 0;
  old_code_ptr = 1;

  data_win_y   = 5;//5;
  data_win_dim = 3;//10;

  code_win_y   = data_win_y+data_win_dim+1;//11;//16;
  code_win_dim = 8;//10;
		
  buff_win_y   = code_win_y+code_win_dim+1;//17;//27;
  buff_win_dim = 4;//8;

  cmd_win_y    = buff_win_y+buff_win_dim+1;//23;//37;
  cmd_win_dim  = 3;//3;

  prepare_screen();

  screen_offset_x = 0;
  screen_offset_y = 0;

  screen_bpp = 2;

  insertcmdindex = 0;
  cmdindex = 0;

  reg_stack = &current_reg_stack;
  current_reg_stack.reg_EIP = 0xC0000000;
  current_reg_stack.reg_CS = RING_HOOO_SEGMENT_VALUE;


  Out_Debug_String("load bmp...\n\r");

  //LoadBMP(NULL,  &font_x,&font_y,&font_bpp, font_data);
  
//__asm{int 3};  
  //font_x=font_x/256;
  
  Out_Debug_String("load bmp...END\n\r");

  pagedir = (unsigned int)basetp();
  Out_Debug_String("chop cr3...ok\n\r");
		
  idt = (unsigned int)getbaseidt();  
  Out_Debug_String("chop idt...\n\r");

  //hexatoascii(idt,debug__);

  /*sav les valeur de es fs gs*/
#if defined(__GNUC__)
  __asm(
	"push %eax\n"
	"xor %eax, %eax\n"
	"mov %es, %ax\n"
	"mov %eax, ring0_ES\n"
	"mov %fs, %ax\n"
	"mov %eax, ring0_FS\n"
	"mov %gs, %ax\n"
	"mov %eax, ring0_GS\n"
	"pop %eax"
	);
#elif defined(_MSC_VER)
  __asm{
	push eax
	xor eax, eax
	mov ax, es
	mov ring0_ES, eax
	mov ax, fs
	mov ring0_FS, eax
	mov ax, gs
	mov ring0_GS, eax
	pop eax
	};
#endif
		

  reset_hw_bp();

  for (i=0;i<MAX_INT;i++)
    {
      is_int_hooked[i] = 0;
      old_int[i] = 0;
      addrnewint[i] = 0;
    }

  is_int_hooked[0] = 1;
  is_int_hooked[1] = 1;
  is_int_hooked[3] = 1;
  is_int_hooked[6] = 0;

  is_int_hooked[13] = 0;




  addrnewkbd  = ((unsigned int) &kbdhandle);

  addrnewint[0] = ((unsigned int) &int0_handle);
  addrnewint[1] = ((unsigned int) &int1_handle);
  addrnewint[3] = ((unsigned int) &int3_handle);
  addrnewint[6] = ((unsigned int) &int6_handle);
  addrnewint[13] =((unsigned int) &int13_handle);
  addrnewint[14] =((unsigned int) &int14_handle);
  addrnewint[128]=((unsigned int) &int128_handle);


  Out_Debug_String("install hook...\n\r");


  clear_wp_bit();

#ifndef DONT_HOOK_KBD
  oldkbd =(void *) modifie_idt(addrnewkbd,(unsigned int*)idt+2*KEYB_INTER);
#endif /* DONT_HOOK_KBD */

  for (i=0;i<MAX_INT;i++)
    {
      if (is_int_hooked[i])
	old_int[i] =(void *) modifie_idt(addrnewint[i],(unsigned int*)idt+2*i);
    }
  restore_wp_bit();

  Out_Debug_String("install hook...ok\n\r");

  cleaningbuffer();
  Out_Debug_String("init fini.1\n\r");

  //__asm{int 3};
  deblin_console_on();
  Out_Debug_String("init fini.2\n\r");


  //Refresh_Display();

  Out_Debug_String("init fini.\n\r");

#if 0
  init_symb();
#endif
  Out_Debug_String("symbol oki.\n\r");


  list_controller();
  hw_video_addess = pci_detect_display();

  if (hw_video_addess !=0)
    {
      Sprintf(debug_, "Autodetect return address: 0x%.8X\n", hw_video_addess);
      Out_Debug_String(debug_);
#ifdef AUTO_HW_FB
      Out_Debug_String("Setting this video address\n");

      screen_adresse = hw_video_addess;
#endif
    }
  else
    {
      Out_Debug_String("PCI scan failed\n");
    }
		
  return(dwRetVal);

}

int SPY_Dynamic_Exit(void);


void cleanup_rr0d(void)
{
  int dwRetVal = 0;
  int i;

  /* Dispatch to cleanup proc */
  Out_Debug_String("SPY: Closing!\n\r");

  dwRetVal = 1;

  Out_Debug_String("Spy: test unloading\n\r");

  reset_hw_bp();
#ifndef DONT_HOOK_KBD
  tmpkbd =(void *) modifie_idt( (unsigned int)oldkbd,   (unsigned int*)idt_[0]+2*KEYB_INTER);
#endif /* DONT_HOOK_KBD */

  for (i=0;i<MAX_INT;i++)
    {
      if (is_int_hooked[i])
	tmp_int[i] =(void *) modifie_idt( (unsigned int)old_int[i],(unsigned int*)idt_[0]+2*i);
    }

		
  SPY_Dynamic_Exit();

}



int SPY_Dynamic_Exit(void)
{

  int i;


  tmpkbd = gd_offset(idt_[0]+2*KEYB_INTER);

  for (i=0;i<MAX_INT;i++)
    {
      tmp_int[i] = gd_offset(idt_[0]+2*i);
    }

  /*
   * BUG: faut del les break point avant de partir: pb de
   * rémanance des CC dans les pages memoire cache penser a
   * hooked les page fault pour savoir si la page a été giclée
   * de la ram sucre les breakpoint mais sert a rien.
   */
#if 0
  for (i=0;i<NUM_BREAKPOINTS;i++)
    erase_breakpoint(i);
#endif
  reset_hw_bp();

  if (&kbdhandle == tmpkbd)
    {
#ifndef DONT_HOOK_KBD
      Out_Debug_String("SPY: Trouble in Unhooking proc!! [keyb] re-unhook\n\r");
      tmpkbd =(void *) modifie_idt( (unsigned int)oldkbd,   (unsigned int*)idt_[0]+2*KEYB_INTER);
#endif /* DONT_HOOK_KBD */
    }

  Out_Debug_String("SPY: Dynamic Exit\n\r");

  return(1);
}



 
 
