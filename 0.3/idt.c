/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id: idt.c,v 1.21 2006-07-11 07:09:29 serpilliere Exp $
 *
 */

#include "vars.h"
#include "idt.h"
#include "var_globale.h"
#include "buffering.h"
#include "utils.h"
#include <types.h>

/**
 *  get_idt: returns the idt base address for x86
 *
 *  @returns idt base address
 */
u32 *
idt_base(void) {
  IDTR idtr;
#if defined(__GNUC__)
  _asm_("sidt idtr");
#elif defined(_MSC_VER)
  __asm{ sidt idtr };
#endif

  return (u32 *)idtr.base;
}

/**
 * gdt_base: returns the gdt base address for x86
 *
 * @returns gdt base address
 */
u32 *
gdt_base(void) {
  GDTR gdtr;
#if defined(__GNUC__)
  _asm_("sgdt gdtr");
#elif defined(_MSC_VER)
  __asm{ sgdt gdtr};
#endif

  return (u32 *)gdtr.base;
}

/**
 * ldt_base: returns the ldt base address for x86
 *
 * @returns the ldt base address
 */
u32 *
ldt_base(void) {
  LDTR ldtr;
#if defined(__GNUC__)
  _asm_("sldt ldtr");
#elif defined(_MSC_VER)
  __asm{ sldt ldtr };
#endif

  return (u32 *)ldtr.base;
}

unsigned int translate_logic_to_linear(unsigned int segment, unsigned int adresse)
{
  unsigned int index;
  unsigned int *  dt_base;
  unsigned int tmp_ldt;
  unsigned int data1, data2;
  unsigned int adresse_base;

  if (segment & 0x4)
    {
      tmp_ldt = getbaseldt();
      dt_base = getbasegdt();
      tmp_ldt = tmp_ldt>>3;

      data1 = dt_base[tmp_ldt*2];
      data2 = dt_base[tmp_ldt*2+1];

      adresse_base= (data1 & 0xffff0000)>>16;
      adresse_base+=(data2 & 0x000000ff)<<16;
      adresse_base+=(data2 & 0xff000000);
      dt_base = (unsigned int*)adresse_base;
      

    }
  else
    {
      dt_base = getbasegdt();
    }

  index = segment >>3;
  data1 = dt_base[index*2];
  data2 = dt_base[index*2+1];

  adresse_base= (data1 & 0xffff0000)>>16;
  adresse_base+=(data2 & 0x000000ff)<<16;
  adresse_base+=(data2 & 0xff000000);
  //if (segment & 4)
  //  printk("local! %X \n", adresse_base);
  return  adresse+adresse_base;


}


/*affiche les adresse de 'lidt                    */
/* l'IDT_ doit avoir était affecté avant /!\      */
void  visualise_idt(unsigned long *adresse_idt)
{
  unsigned int offset;
  unsigned int selecteur;
  int x;
  char tmp[2048];

  insertinbuffer("[_    idt list    _]");
  insertinbuffer("--------------------");

  for(x=0;x<=0x80;)
    {
      offset=((unsigned long) (*adresse_idt) & 0xffff);
      selecteur=((unsigned long) (*adresse_idt) & 0xffff0000)>>16;
      adresse_idt++;
      offset=offset+((unsigned long) (*adresse_idt) & 0xffff0000);
      adresse_idt++;
      Sprintf(debug__, "%.2X %.8X %.8X", x, selecteur, offset);
      x++;

      offset=((unsigned long) (*adresse_idt) & 0xffff);
      selecteur=((unsigned long) (*adresse_idt) & 0xffff0000)>>16;
      adresse_idt++;
      offset=offset+((unsigned long) (*adresse_idt) & 0xffff0000);
      adresse_idt++;
      Sprintf(tmp, "%s ³ %.2X %.8X %.8X", debug__, x, selecteur, offset);
      x++;

      offset=((unsigned long) (*adresse_idt) & 0xffff);
      selecteur=((unsigned long) (*adresse_idt) & 0xffff0000)>>16;
      adresse_idt++;
      offset=offset+((unsigned long) (*adresse_idt) & 0xffff0000);
      adresse_idt++;
      Sprintf(debug__, "%s ³ %.2X %.8X %.8X", tmp, x, selecteur, offset);
      x++;

      insertinbuffer(debug__);

    }
}

/*modifie l'entrée de l'idt par l'arg de la       */
/*fonction passée en parametre                    */ 
/*renvoie l'ancien pointeur de l'entrée           */
unsigned long modifie_idt(unsigned int newhandle,unsigned int *idtentry)
{
  unsigned long aux;


  aux=((unsigned long) (*idtentry) & 0xffff);
  *idtentry=(((unsigned long) (*idtentry) & 0xffff0000)|(newhandle & 0xffff));
  idtentry++;
  aux=aux+((unsigned long) (*idtentry) & 0xffff0000);
  *idtentry=(((unsigned long) (*idtentry) & 0xffff)|(newhandle & 0xffff0000)); 
  return aux;

}


unsigned long read_idt_entry(unsigned int *idtentry)
{
  unsigned long aux;

  aux=((unsigned long) (*idtentry) & 0xffff);
  idtentry++;
  aux=aux+((unsigned long) (*idtentry) & 0xffff0000);
  return aux;
}
