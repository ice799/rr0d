/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id
 *
 */

#include "import_symb.h"
#include "utils.h"
#include "buffering.h"


Symbol tab_import_symb[MAX_SYMBOL];


void init_symb(void)
{
  int i;
  for (i=0;i<MAX_SYMBOL;i++)
    {
      tab_import_symb[i].address=0;
      tab_import_symb[i].name[0]=0;
    }

  /*TEST TEST */

  rr0d_strcpy(tab_import_symb[0].name, "pouet1 (_.-._)" );
  tab_import_symb[0].address = 0x8048373;
  
  rr0d_strcpy(tab_import_symb[1].name, "boubou2");
  tab_import_symb[1].address = 0x8048365;
  
  /*FIN TEST*/
}


char * is_symbol_address(unsigned int address)
{
  int i;
  for (i=0;i<MAX_SYMBOL;i++)
    {
      if (address == tab_import_symb[i].address)
	{
	  return tab_import_symb[i].name;
	}
    }

  return (char*)0;

}



int load_symbol(Symbol* address_symbol, unsigned int num_symb)
{
  unsigned int i=0;
  unsigned int ii;
#if 0
  char debug__[80];
#endif

  while((address_symbol[i].address != 0) && (i<num_symb) && (i<MAX_SYMBOL))
    {
      tab_import_symb[i].address = address_symbol[i].address;
      for (ii=0;ii<SYMBOL_NAME_LEN;ii++)
	tab_import_symb[i].name[ii] = address_symbol[i].name[ii];
      
      
      i++;
    }
  
  tab_import_symb[i].address = 0;
  return i;
  

}


void list_symbol()
{
  unsigned int i=0;
  char debug__[80];

  i = 0;
  while(tab_import_symb[i].address && (i<MAX_SYMBOL))
    {
      Sprintf(debug__, "%s %.8X", tab_import_symb[i].name, tab_import_symb[i].address);
      insertinbuffer(debug__);     
      i++;
    }
  
}
