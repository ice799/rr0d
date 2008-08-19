/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id: breakpoint.c,v 1.57 2006-07-11 07:09:29 serpilliere Exp $
 *
 */

//extern unsigned int* page_gift;

#include "vars.h"
#include "nasm.h"
#include "disasm.h"

#include "breakpoint.h"

#include "buffering.h"
#include "utils.h"

#include "page.h"
#include "idt.h"
#include "var_globale.h"

Bp_struct list_breakpoints[NUM_BREAKPOINTS]={
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
};           

Bp_struct list_hw_breakpoints[NUM_HW_BREAKPOINTS]={
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
};           


int unmapped_bp_to_shoot=-1;







/*insertion d'un breakpoint:                      */
/* /!\ la page de memoire doit etre deja bonne    */
/*sauvegarde l'ancien octet ecrasé par le break   */
/*point, et ajoute dans la liste des bp.          */
unsigned int iop;
int insert_bp(unsigned int cs, unsigned int dir, unsigned int attrib)
{


  int x=0;
  unsigned int info_page_src;
  unsigned int level;
  unsigned int padresse;
  unsigned char *ptr = (unsigned char*)translate_logic_to_linear(cs, dir);

  if (!ptr)
    {
      log_line_in_buffer("All but this address man");
      return -1;
    }

  if (!page_exist((unsigned int) ptr))
    {
      log_line_in_buffer("page doesn't exist, man");
      return -1;
    }

  info_page_src = get_page_info((unsigned int)ptr, NULL, NULL, &level );
  padresse = (info_page_src & 0xfffff000) + ((unsigned int)ptr & 0xfff);


  for (x=0;x<NUM_BREAKPOINTS;x++)
    {
      if (list_breakpoints[x].address==0)
	break;
    }

  if (x<NUM_BREAKPOINTS)
    {
      list_breakpoints[x].address=dir;
      list_breakpoints[x].value=*ptr;
      list_breakpoints[x].paddress=padresse;
      list_breakpoints[x].attrib=attrib;

      write_save_dirty( ptr, 0xcc);
      return x;
    }
  else 
    {
      log_line_in_buffer("too many breakpoint man");
      return -1;
    }	
}

/*renvoie si l'adresse donnée est un break point  */
/*le numéro du breakpoint si oui                  */
/*-1 sinon                                        */
/*                                                */
int is_breakpoint(unsigned char *ptr)
{
  int i;
  unsigned int padresse;


  if (!ptr)
    return -1;

  for (i=0;i<NUM_BREAKPOINTS;i++)
    if (list_breakpoints[i].address==(unsigned long) ptr) break;
              
  if (i<NUM_BREAKPOINTS)
    return i;

	
  padresse = linear2physical((unsigned int)ptr);
	
  for (i=0;i<NUM_BREAKPOINTS;i++)
    {
      if (!list_breakpoints[i].address) continue;
      if (list_breakpoints[i].paddress==padresse) break;
    }
	
  if (i<NUM_BREAKPOINTS)
    return i;
	
  return -1;
}


/*enleve un break point:                          */
/*remet l'octet ecrasé par le CC, et enleve       */
/*le break point de la liste                      */
/* /!\ la page de l'adresse du bp doit etre en mem*/
int erase_breakpoint(unsigned int bp)
{
  unsigned char *pbp;
  unsigned int padresse;

  if ((bp>= HW_MAGIC_VALUE) && (bp < HW_MAGIC_VALUE + NUM_HW_BREAKPOINTS))
    {
      bp-=HW_MAGIC_VALUE;

      switch(bp)
	{
	case 0:
	  set_dr0_val(0); break;
	case 1:
	  set_dr1_val(0); break;
	case 2:
	  set_dr2_val(0); break;
	case 3:
	  set_dr3_val(0); break;
	default:
	  log_line_in_buffer("but man, what are u doin' there?");
	  break;
	}

      list_hw_breakpoints[bp].address=0;
      return bp;
    }

  if (bp>NUM_BREAKPOINTS)
    return -1;

  if (!list_breakpoints[bp].address)
    return -1;


  if (!page_exist(list_breakpoints[bp].address))
    {
      log_line_in_buffer("yup! bp is not in ram anymore!! system may be unstable");
      return -1;
    }

  padresse = linear2physical((unsigned int)list_breakpoints[bp].address);
	
  if (padresse !=list_breakpoints[bp].paddress)
    {
      if ((unmapped_bp_to_shoot!=bp))
	{
	  log_line_in_buffer("lin@ doesnt match phys@. fall back in int del");
	  unmapped_bp_to_shoot = bp;
	  return -1;
	}
    }
	
  pbp= (unsigned char *) list_breakpoints[bp].address;

  write_save_dirty( pbp, list_breakpoints[bp].value);
	
  list_breakpoints[bp].address=0;
  list_breakpoints[bp].value=0;  
  return bp;	
}



/*remet le bon octet ecrasé par le bp, sans       */
/*l'enlever de la liste des bp                    */
/* /!\ la page doit etre en mem                   */
/*                                                */
int pass_breakpoint(unsigned int bp)
{
  unsigned char *pbp;

  if (bp>NUM_BREAKPOINTS)
    return -1;

  if (list_breakpoints[bp].address)
    {
      pbp= (unsigned char *) list_breakpoints[bp].address;
      write_save_dirty( pbp, list_breakpoints[bp].value);
      return bp;
    }
  else
    {
      return -1;
    }

}

/*remet un CC sur l'adresse du bp spécifié        */
/* /!\ la page doit etre en mem                   */
/*                                                */
/*                                                */
int spawn_breakpoint(unsigned int bp)
{
  unsigned char *pbp;

  if (bp>NUM_BREAKPOINTS)
    return -1;

  if (list_breakpoints[bp].address)
    {
      pbp= (unsigned char *) list_breakpoints[bp].address;
      //write_save_dirty( pbp, list_breakpoints[bp].value);
      write_save_dirty( pbp, 0xCC);
      return bp;
    }
  else
    {
      return -1;
    }
}



/*affiche la liste des breakpoint                 */
/*(dans la fenetre de log)                        */
/*                                                */
/*                                                */
void display_breakpoints()
{
  unsigned int i=0;
	
  log_line_in_buffer("[_Break point List_]");

  for (i=0;i<NUM_BREAKPOINTS;i++)
    {
      if (list_breakpoints[i].address)
	{
	  Sprintf(debug__, "%.4X %.8X", i, list_breakpoints[i].address);
	  log_line_in_buffer(debug__);
	}
    }


  for (i=0;i<4;i++)
    {
      if (list_hw_breakpoints[i].address)
	{
	  Sprintf(debug__, "%.4X %.8X", i+HW_MAGIC_VALUE, list_hw_breakpoints[i].address);
	  log_line_in_buffer(debug__);
	}
    }
}



/*si l'octet de l'adresse specifie a été remplace */
/*par un bp, renvoie l'ancien octet               */
/*renvoie l'octet de l'adresse sinon              */
unsigned char true_byte_bp(unsigned char *ptr)
{
  int bp_num;

  if ((bp_num=is_breakpoint(ptr))!=-1)
    return list_breakpoints[bp_num].value;
	
  return *ptr;
}

int parse_inst(unsigned int selector, unsigned int ptr, insn* my_ins)
{
       
  unsigned char tmp[100];
  int len=0;
  unsigned char *adresse = (unsigned char*)translate_logic_to_linear(selector, ptr);
  
  if (!page_exist((unsigned int) adresse))
    return 0;
    
  len=disasm((unsigned char*)adresse, tmp,32,0,0, my_ins);
  return len;
}




unsigned int debug_reg_value;

#if defined(__GNUC__)
#define get_val(reg) _asm_( "\t" \
	"push %eax\n\t" \
	"movl %" reg ", %eax\n\t" \
	"movl %eax, debug_reg_value\n\t" \
	"popl %eax\n\t" \
	)
#elif defined(_MSC_VER)
#define get_val(reg)\
{\
 __asm{push eax}\
 __asm{mov eax, reg}\
 __asm{mov debug_reg_value, eax}\
 __asm{pop eax}\
}
#endif

#if defined(__GNUC__)
#define set_val(reg) _asm_( "\t" \
	"push %eax\n\t" \
	"movl debug_reg_value, %eax\n\t" \
	"movl %eax, %" reg "\n\t" \
	"popl %eax\n\t" \
	)
#elif defined(_MSC_VER)
#define set_val(reg) \
{\
__asm{push eax}\
__asm{mov eax, debug_reg_value}\
__asm{mov reg, eax}\
__asm{pop eax}\
}
#endif

unsigned int get_dr0_val()
{
#if defined(__GNUC__)
  get_val("dr0");
#elif defined(_MSC_VER)
  get_val(dr0);
#endif

  return debug_reg_value;
}

unsigned int get_dr1_val()
{
#if defined(__GNUC__)
  get_val("dr1");
#elif defined(_MSC_VER)
  get_val(dr1);
#endif

  return debug_reg_value;
}

unsigned int get_dr2_val()
{
#if defined(__GNUC__)
  get_val("dr2");
#elif defined(_MSC_VER)
  get_val(dr2);
#endif

  return debug_reg_value;
}

unsigned int get_dr3_val()
{
#if defined(__GNUC__)
  get_val("dr3");
#elif defined(_MSC_VER)
  get_val(dr3);
#endif

  return debug_reg_value;
}

unsigned int get_dr4_val()
{
  /*
#if defined(__GNUC__)
  get_val("dr4");
#elif defined(_MSC_VER)
  get_val(dr4);
#endif
  */
  return debug_reg_value;
}

unsigned int get_dr5_val()
{
  /*
#if defined(__GNUC__)
  get_val("dr5");
#elif defined(_MSC_VER)
  get_val(dr5);
#endif
  */
  return debug_reg_value;
}

unsigned int get_dr6_val()
{
#if defined(__GNUC__)
  get_val("dr6");
#elif defined(_MSC_VER)
  get_val(dr6);
#endif

  return debug_reg_value;
}

unsigned int get_dr7_val()
{
#if defined(__GNUC__)
  get_val("dr7");
#elif defined(_MSC_VER)
  get_val(dr7);
#endif

  return debug_reg_value;
}





void set_dr0_val(unsigned int value)
{
  debug_reg_value = value;


#if defined(__GNUC__)
  set_val("dr0");
#elif defined(_MSC_VER)
  set_val(dr0);
#endif

}


void set_dr1_val(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr1");
#elif defined(_MSC_VER)
  set_val(dr1);
#endif

}

void set_dr2_val(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr2");
#elif defined(_MSC_VER)
  set_val(dr2);
#endif

}

void set_dr3_val(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr3");
#elif defined(_MSC_VER)
  set_val(dr3);
#endif

}

void set_dr4_val(unsigned int value)
{
  debug_reg_value = value;
  /*
#if defined(__GNUC__)
  set_val("dr4");
#elif defined(_MSC_VER)
  set_val(dr4);
#endif
  */
}

void set_dr5_val(unsigned int value)
{
  debug_reg_value = value;
  /*
#if defined(__GNUC__)
  set_val("dr5");
#elif defined(_MSC_VER)
  set_val(dr5);
#endif
  */
}

void set_dr6_val(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr6");
#elif defined(_MSC_VER)
  set_val(dr6);
#endif

}

void set_dr7_val(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr7");
#elif defined(_MSC_VER)
  set_val(dr7);
#endif

}



unsigned int get_cr4_val()
{
#if defined(__GNUC__)
  //get_val("cr4");
#elif defined(_MSC_VER)
  //get_val(cr4);
#endif

  return debug_reg_value;
}

void set_cr4_val(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  //set_val("cr4");
#elif defined(_MSC_VER)
  //set_val(cr4);
#endif

}



void reset_hw_bp()
{
  int x;

  for (x=0;x<NUM_HW_BREAKPOINTS;x++)
    list_hw_breakpoints[x].address=0;

  set_dr6_val(0x0);
  set_dr7_val(0x0);

  set_dr0_val(0x0);
  set_dr1_val(0x0);
  set_dr2_val(0x0);
  set_dr3_val(0x0);


}


int is_hw_breakpoint(unsigned char* ad)
{
  unsigned int x;
  for (x=0;x<NUM_HW_BREAKPOINTS;x++)
    if(list_hw_breakpoints[x].address==(unsigned int )ad)
      return 1;

  return 0;

}

void set_hw_bp(unsigned int ad)
{
  unsigned int mask_ge_le = 0x300;
  int num_debug_reg;
  unsigned int mask_current_bp;
  unsigned int dr7_val;

  for (num_debug_reg=0;num_debug_reg<NUM_HW_BREAKPOINTS;num_debug_reg++)
    if (!list_hw_breakpoints[num_debug_reg].address)
      {
	break;
      }


  

  if ((num_debug_reg <0 )||(num_debug_reg >3))
    {
      log_line_in_buffer("Oao, only 4 hw bp on 86, man");
      return ;
    }

  mask_current_bp = 0x3<<(num_debug_reg*2);
  /*only exec*/
  mask_current_bp &= ~(((0x0<<(num_debug_reg*2)) << 16));

  /*TEST TEST*/


  dr7_val = get_dr7_val();


  dr7_val |= (mask_ge_le | mask_current_bp);


  set_dr7_val(dr7_val);
  switch(num_debug_reg)
    {
    case 0:
      set_dr0_val(ad); break;
    case 1:
      set_dr1_val(ad); break;
    case 2:
      set_dr2_val(ad); break;
    case 3:
      set_dr3_val(ad); break;
    default:
      log_line_in_buffer("but man, what are u doin' there?");
      break;
    }

  list_hw_breakpoints[num_debug_reg].address = ad;


}


int check_hw_bp()
{
  unsigned int dr6_val;

  dr6_val = get_dr6_val();
  return dr6_val & 0xf;
}



void enable_hw_bp(int i)
{
  unsigned int mask= 0x3 << (2*i);
  unsigned int dr7_val;

  dr7_val = get_dr7_val();
  dr7_val|= mask;
  set_dr7_val(dr7_val);

}

void disable_hw_bp(int i)
{
  unsigned int mask=~(0x3 << (2*i));
  unsigned int dr7_val;

  dr7_val = get_dr7_val();
  dr7_val&= mask;
  set_dr7_val(dr7_val);

}


void enable_id_hw_bp(int id_hw_bp)
{
  int i;

  for (i=0;i<4;i++)
    {
      if (id_hw_bp & (1<<i))
	enable_hw_bp(i);
    }
}


void disable_id_hw_bp(int id_hw_bp)
{
  int i;

  for (i=0;i<4;i++)
    {
      if (id_hw_bp & (1<<i))
	disable_hw_bp(i);
    }
}

