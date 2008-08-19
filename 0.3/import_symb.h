/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 *
 * $Id
 *
 */

#ifndef IMPORT_SYMB_H
#define IMPORT_SYMB_H



#define MAX_SYMBOL 2000
#define SYMBOL_NAME_LEN 16


typedef struct symb_struct {
  unsigned int address;
  char name[SYMBOL_NAME_LEN];
} Symbol;

extern Symbol tab_import_symb[MAX_SYMBOL];


void init_symb(void);
char * is_symbol_address(unsigned int);
int load_symbol(Symbol* address_symbol, unsigned int num_symb);

void list_symbol(void);

#endif //IMPORT_SYMB_H
