/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id: page.c,v 1.45 2006-07-11 07:09:30 serpilliere Exp $
 *
 */

#include "vars.h"

#include "page.h"

#include "utils.h"
#include "buffering.h"


/*TESTTEST*/



void page_info(void);

unsigned int pagedir;

#define PAG4M    2
#define EXISTE   1
#define NOEXISTE 0

int init_p=0;

#ifdef FREEBSD

#include <vm/vm.h>
#include <vm/pmap.h>
/*
  typedef uint64_t pd_entry_t;
  extern pd_entry_t PTD[];
  extern pd_entry_t PTDpde[];
  extern pd_entry_t PTmap[];
*/
#endif



/*renvoie l'adresse du repertoire des pages       */
/* /!\ dépendant de l'os pour l'adresse base du   */
/*kernel: sous win c2690000 sous nux c0000000     */
unsigned int *basetp(void)
{

#if defined(__GNUC__)
  _asm_("\t"
	"movl %cr3, %eax\n\t"
	"movl %eax, pagedir\n"
	);
#elif defined(_MSC_VER)
  __asm{
        mov eax, cr3
	mov pagedir, eax
      };
#endif


  /* /!\ BUG KERNEL BASE */
  pagedir += KERNEL_B;
  return (unsigned int*)pagedir; 
}



/*affiche (plus) les info des tables              */
/*                                                */
/*                                                */
void page_info(void)
{
  unsigned int *page_tab;
  int i;
  unsigned int page_info;
  unsigned int adr;
  unsigned int info;

  page_tab = basetp();
  for (i=0;i<1024;i++)
    {
      page_info = page_tab[i];		
      /* KERNEL_B */
      adr = page_info & 0xFFFFF000;
      info =page_info & 0x00000FFF;

      if (!(info & 1))
	continue;
#if 0
      printf("%.8X %.8X %.3X %.8X ", page_info, adr, info, i*0x00400000);
      printf("p:%d ", (info&1)?1:0); /* p */
      printf("r:%d ", (info&2)?1:0); /* r */
      printf("a:%d ", (info&0x20)?1:0); /* access */
      printf("mode:%d ", (info&0x4)?1:0); /* mode */


      printf("\n");
#endif

    }

}

#ifdef WIN32
unsigned int * PTD = (unsigned int*)       0xC0300000;
unsigned int * PTmap = (unsigned int*)     0xC0000000;
#endif

/*renvoie l'attribut de la page et affecte le niveau:*/
/*(si level!=NULL) 0: merdouille; 1: 1er niveau; 2: 2 eme niveau*/
unsigned int get_page_info(unsigned int adresse, unsigned int *tab_page, unsigned int *tab_page_entry, unsigned int *level )
{
 

#if defined FREEBSD || defined WIN32
  /*TODO: non quand meme, faudrait faire qq chose pour les *bsd*/

  if (level)
    *level=0;

  if (tab_page)
     *tab_page = (unsigned int)PTD;

  if (!(PTD[adresse >> 22] & 1))
      return 0;

  if (PTD[adresse >> 22]&0x80)
    {
      if (level)
	      *level=1;
      return PTD[adresse >> 22];
    }

  if (level)
    *level=2;

  if (!(PTmap[adresse >> 12] & 1))
    return 0;

  if (tab_page_entry)
    *tab_page_entry = (unsigned int)PTmap;


  return PTmap[adresse >> 12];
	
  //return 1;		  
#else
  unsigned int *page_tab;
  unsigned int *sub_page_tab;
  int page_dir;
  int page;
  unsigned int page_info;
  unsigned int sub_page_info;
  unsigned int tmp;


  if (level)
    *level=0;

  page_tab = basetp();
  page_dir = (adresse>>22) & 0x3FF; 
  page_info = page_tab[page_dir];

  if (tab_page)
    {
      tmp = (unsigned int)(page_info & 0xFFFFF000)+KERNEL_B;
      *tab_page = (unsigned int)tmp;
    }

  if (!(page_info&1))
    return 0;

  if (page_info&0x80)
    {
      if (level)
	*level=1;
      return page_info;
    }

  if (level)
    *level=2;


  tmp = (page_info & 0xFFFFF000)+KERNEL_B;
  sub_page_tab = (unsigned int*)tmp;

  if (tab_page_entry)
    *tab_page_entry = (unsigned int)sub_page_tab;

  page= (adresse>>12) & 0x3FF; 

  sub_page_info = sub_page_tab[page];

  return sub_page_info;
#endif
}

/*renvoie si une page est présente en memoire     */
/*ou pas.                                         */
/*                                                */
int page_exist(unsigned int adresse)
{
  return get_page_info(adresse, NULL, NULL, NULL) & 1;
}


unsigned int set_page_info(unsigned int adresse, unsigned int data, unsigned int level)
{
  unsigned int *page_tab;
  unsigned int *sub_page_tab;
  unsigned int page_info;

  page_info = get_page_info(adresse,
			    (unsigned int*)&page_tab,
			    (unsigned int*)&sub_page_tab,
			    NULL);

  if (!page_info)
    return 0;

  if (level==0)
    return 0;

  if (level==1)
    {
      page_tab[(adresse>>22) & 0x3FF]=data;
      return 0x1337;
    }

  if (level==2)
  {
#ifdef WIN32
    sub_page_tab[(adresse>>12) & 0x3FFFFF]=data;
#else
    sub_page_tab[(adresse>>12) & 0x3FF]=data;
#endif
  }
  return 0x1337;
}


unsigned int linear2physical(unsigned int ptr)
{
  unsigned int info_page_src;
  info_page_src = get_page_info(ptr, NULL, NULL, NULL );
  return (info_page_src & 0xfffff000) + (ptr & 0xfff);
}



void display_page(unsigned int ptr)
{
  unsigned char debug[80];
  unsigned int page_ad;


  page_ad = get_page_info(ptr, NULL, NULL, NULL );

  if (!(page_ad & 0x1))
    {
      log_line_in_buffer("Not present");
      return;
    }
  Sprintf(debug, "%.8X %.8X %s %s %s %s %s %s", 
	  ptr, 
	  page_ad, 
	  (page_ad & 0x2)?"R/W":"R",
	  (page_ad & 0x4)?"U":"S",
	  (page_ad & 0x20)?"a":"!a",
	  (page_ad & 0x40)?"d":"!d",
	  (page_ad & 0x80)?"4M":"4K",
	  (page_ad & 0x100)?"G":"L");
  log_line_in_buffer(debug);

}






unsigned int saved_wp_bit=0;

void clear_wp_bit(void)
{
#if defined(__GNUC__)
  _asm_(
	"push %eax\n\t"
	"movl %cr0,%eax\n\t" 
	"movl %eax, saved_wp_bit\n\t"
	"andl $0xFFFEFFFF,%eax\n\t"   
	"movl %eax,%cr0\n\t"
	"pop  %eax\n\t"
	);
#elif defined(_MSC_VER)
  _asm{
      push eax
      mov  eax, cr0
      mov  saved_wp_bit, eax
      and   eax, 0xFFFEFFFF
      mov  cr0, eax
      pop  eax
      };
#endif


  saved_wp_bit = saved_wp_bit & 0x00010000;
}

void restore_wp_bit(void)
{

#if defined(__GNUC__)
  _asm_(
	"push %eax\n\t"
	"movl %cr0,%eax\n\t" 
	"andl $0xFFFEFFFF, %eax\n\t"   
	"orl  saved_wp_bit, %eax\n\t"
	"movl %eax,%cr0\n\t"
	"pop  %eax\n\t"
	);
#elif defined(_MSC_VER)
  _asm{
    push eax
      mov  eax,cr0
      and  eax, 0xFFFEFFFF
      or   eax, saved_wp_bit
      mov  cr0,eax
      pop  eax
      };
#endif


}



int write_save_dirty(unsigned char* ptr, unsigned char o)
{

  unsigned int info_page_src;
  unsigned int level;

  info_page_src = get_page_info((unsigned int)ptr, NULL, NULL, &level );  
  clear_wp_bit();
  *ptr  = o;  

  set_page_info((unsigned int)ptr, info_page_src, level);
  restore_wp_bit();
  return 0;
}




unsigned int get_page_phys(unsigned int ad)
{
  unsigned char debug[80];
  unsigned int i;
  unsigned int info_page_src;
  unsigned int level;

  ad &= 0xFFFFF000;
  for (i=0;i<0xFFFF0000;i+=0x1000)
    {
      info_page_src = get_page_info((unsigned int)i, NULL, NULL, &level )& 0xFFFFF000;  
      if (info_page_src == ad)
	{
	  Sprintf(debug, "%.8X", i);
	  log_line_in_buffer(debug);

	}


    }

  return 0;
}


/*
unsigned char padding_var1[0x1000];
unsigned char  flying_page1[0x1000];
unsigned char padding_var2[0x1000];
*/
unsigned char  flying_page1[0x4000] = {'o'};



void poked1(unsigned int ptr, unsigned int v)
{
  unsigned int info_page_src;
  unsigned int level;
  unsigned char* ptr_flying = (unsigned char*)(((unsigned int) &flying_page1[0x1000]) & 0xFFFFF000);
  unsigned int ptr_hi = ptr & 0xFFFFF000;
  unsigned int ptr_lo = ptr & 0x00000FFF;
  //unsigned char debug[80];
  //unsigned int toto;
  ptr_lo &= 0xFFFFFFFC;




#if defined(__GNUC__)
  _asm_("\t"
	"pushl %eax\n\t"
	"movl %cr3, %eax\n\t"
	"movl %eax, %cr3\n"
	"popl %eax\n\t"
	);
#elif defined(_MSC_VER)
  __asm{
        push eax
        mov eax, cr3
	      mov cr3, eax
	      pop eax
      };
#endif




  info_page_src = get_page_info((unsigned int)ptr_flying, NULL, NULL, &level );  



  if (info_page_src & 0x80)
    {
      Out_Debug_String("bad\n");
      return;      
    }

  //printk("%X %X %X %X\n", ptr_flying, ptr_hi + (info_page_src & 0x00000FFF), &ptr_flying[ptr_lo], level);
  clear_wp_bit();

  set_page_info((unsigned int)ptr_flying, ptr_hi + (info_page_src & 0x00000EFF), level);

#if defined(__GNUC__)
  _asm_("\t"
	"pushl %eax\n\t"
	"movl %cr3, %eax\n\t"
	"movl %eax, %cr3\n"
	"popl %eax\n\t"
	);
#elif defined(_MSC_VER)
  __asm{
        push eax
        mov eax, cr3
	      mov cr3, eax
	      pop eax
      };
#endif



  //v = *((unsigned int*)(&ptr_flying[ptr_lo]));
  //v ^=0xFFFFFFFF;
  *((unsigned int*)(&ptr_flying[ptr_lo])) =v;
  //ptr_flying[ptr_lo] = v;

  set_page_info((unsigned int)ptr_flying, info_page_src, level);

#if defined(__GNUC__)
  _asm_("\t"
	"pushl %eax\n\t"
	"movl %cr3, %eax\n\t"
	"movl %eax, %cr3\n"
	"popl %eax\n\t"
	);
#elif defined(_MSC_VER)
  __asm{
        push eax
        mov eax, cr3
	      mov cr3, eax
	      pop eax
      };
#endif



  restore_wp_bit();





}
