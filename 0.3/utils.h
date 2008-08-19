/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id $
 *
 */


/*tout est dit (ou pas)                           */
/*                                                */

#define size_t unsigned int

unsigned char rr0d_inb (unsigned int Port);
unsigned short rr0d_inw (unsigned int Port);
unsigned int rr0d_inl (unsigned int Port);
void rr0d_outb (unsigned char Value , unsigned int Port);
void rr0d_outw (unsigned short Value, unsigned int Port);
void rr0d_outl (unsigned int Value  , unsigned int Port);



int Sprintf (char *outbuf,  char *fmt, ...);




int rr0d_isalpha(register int );
int rr0d_isupper(register int );
int rr0d_islower(register int ); 
int rr0d_isdigit(register int ); 
int rr0d_isxdigit(register int ); 
int rr0d_isspace(register int );
int rr0d_ispunct(register int ); 
int rr0d_isalnum(register int ); 
int rr0d_isprint(register int ); 
int rr0d_isgraph(register int ); 
int rr0d_iscntrl(register int ); 
int rr0d_isascii(register int ); 
int rr0d_toascii(register int ); 

#define rr0d_toupper(c)      ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))
#define rr0d_tolower(c)      ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))

int rr0d_atoi( char *);
char * rr0d_strchr( char *, int );
int rr0d_strcmp( char *,  char *);
//#if 0
size_t rr0d_strlen( char *);
int rr0d_strnicmp ( char *, char *,size_t );
//#endif
int rr0d_stricmp (char *, char *);
char * rr0d_strcpy(char *,  char *);
unsigned  rr0d_strcspn( char *,  char *);
char *rr0d_strcat(char *,  char *);
char *rr0d_strncat(char *,  char *, unsigned int );
int rr0d_strncmp( char *,  char *, unsigned int );
char * rr0d_strncpy(char *,  char *, unsigned int );
char * rr0d_strpbrk( char *,  char *);
char * rr0d_strrchr( char *, int );
unsigned int rr0d_strspn( char *,  char *);
char * rr0d_strstr( char *,  char *);
char * rr0d_strtok_r(char *,  char *, char **);



 void* rr0d_memset(void* , int , unsigned int  );
 void* rr0d_memcpy(void* ,  void* , unsigned int );

