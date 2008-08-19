/* disasm.h   header file for disasm.c
 *
 * The Netwide Assembler is copyright (C) 1996 Simon Tatham and
 * Julian Hall. All rights reserved. The software is
 * redistributable under the licence given in the file "Licence"
 * distributed in the NASM archive.
 */

#ifndef NASM_DISASM_H
#define NASM_DISASM_H

#define INSN_MAX 32	/* one instruction can't be longer than this */

//long disasm (unsigned char *, char *, int , long , int );
long disasm (unsigned char *, char *, int , long,int, insn *);
long eatbyte (unsigned char *, char *);

#endif
