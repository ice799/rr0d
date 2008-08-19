/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id $
 *
 */


#include "utils.h"






#include <stdarg.h>



/*TEST TEST*/

unsigned char rr0d_inb (unsigned int Port)
{
  unsigned char Value;
#if defined(__GNUC__)
  __asm__("inb %w1, %0;\n\t"
	  : "=a" (Value)
	  : "d" (Port));

#elif defined(_MSC_VER)
  __asm
    {
  push edx
  xor edx, edx
	mov edx, Port
	in al, dx
	mov Value, al
	pop edx
        }
#endif
  return(Value);
}


unsigned short rr0d_inw (unsigned int Port)
{
  unsigned short Value;
#if defined(__GNUC__)
  __asm__("inw %w1, %0;\n\t"
	  : "=a" (Value)
	  : "d" (Port));

#elif defined(_MSC_VER)
  __asm
    {
  push edx
  xor edx, edx
	mov edx, Port
	in ax, dx
	mov Value, ax
	pop edx
        }
#endif
  return(Value);
}


unsigned int rr0d_inl (unsigned int Port)
{
  unsigned int Value;
#if defined(__GNUC__)
  __asm__("inl %w1, %0;\n\t"
	  : "=a" (Value)
	  : "d" (Port));

#elif defined(_MSC_VER)
  __asm
    {
  push edx
  xor edx, edx
	mov edx, Port
	in eax, dx
	mov Value, eax
	pop edx
        }
#endif
  return(Value);
}







void rr0d_outb (unsigned char Value, unsigned int Port)
{
#if defined(__GNUC__)
  __asm__("outb %0, %w1\n\t"
	  : 
	  : "a" (Value),
	  "d" (Port));
#elif defined(_MSC_VER)
  __asm
    {
  push edx
  xor edx, edx
  mov edx, Port
	mov al, Value
	out dx,al
	pop edx
        }
#endif
}


void rr0d_outw (unsigned short Value, unsigned int Port)
{
#if defined(__GNUC__)
  __asm__("outw %0, %w1\n\t"
	  : 
	  : "a" (Value),
	  "d" (Port));
#elif defined(_MSC_VER)
  __asm
    {
  push edx
  xor edx, edx
  mov edx, Port
	mov ax, Value
	out dx,ax
	pop edx
        }
#endif
}


void rr0d_outl (unsigned int Value, unsigned int Port)
{
#if defined(__GNUC__)
  __asm__("outl %0, %w1\n\t"
	  : 
	  : "a" (Value),
	  "d" (Port));
#elif defined(_MSC_VER)
  __asm
    {
  push edx
  xor edx, edx
  mov edx, Port
	mov eax, Value
	out dx,eax
	pop edx
        }
#endif
}


static void bad ( char *fmt)
{
  //printf("bad fmt in Sprintf, starting with \"%s\"\n", fmt);
  //exit(1);
}

#define put(x) *outbuf++ = (char)x



unsigned char hexabuf_M[] = "0123456789ABCDEF";
unsigned char hexabuf_L[] = "0123456789abcdef";


int Sprintf( char *outbuf,   char *fmt, ...)
{
  char *ob0, *s;
  char buf[32];
  va_list ap;
  //long i/*, j*/;
  unsigned int i, j;
  int signed_entry;
  char* hexabuf = (unsigned char)0;
  int fixed_len;
  unsigned char c;

  va_start(ap, fmt);
  ob0 = outbuf;
  for(;;) {
    for(;;) {
      switch(i = *fmt++) {
      case 0:
	goto done;
      case '%':
	break;
      default:
	put(i);
	continue;
      }
      break;
    }
    fixed_len=0;
  special_char:
    switch(*fmt++) {

    case '.':
      for(i=0;i<8;i++)
	{
	  c = *fmt++;
	  if (c>='0' && c <='9')
	    {
	      fixed_len*=10;
	      fixed_len+=c-'0';
	    }	  
	  else
	    break;
	}
      fmt--;
      goto  special_char;
      continue;
    case 'c':
      i = va_arg(ap, int);
      put(i);
      continue;
    case 'l':
      if ((*fmt != 'd')&& (*fmt != 'x') && (*fmt != 'X'))
	bad(fmt);
      if ((*fmt == 'x') || (*fmt == 'X'))
	goto special_char;

      fmt++;
      //i = va_arg(ap, long);
      signed_entry = va_arg(ap, long);
      goto have_i;
    case 'd':
      //i = va_arg(ap, int);
      signed_entry = va_arg(ap, int);
    have_i:
      if (signed_entry < 0) {
	put('-');
	signed_entry = -signed_entry;
      }
      s = buf;
      i = signed_entry;
      do {
	j = i / 10;
	*s++ = i - 10*j + '0';
	fixed_len--;
      }
      while((i = j));
      while(fixed_len-->0)  put('0');
      do {
	i = *--s;
	put(i);
      }
      while(s > buf);
      continue;
    case 'x':
      hexabuf = hexabuf_L;
      goto have_hexa;
    case 'X':
      hexabuf = hexabuf_M;
    have_hexa:
      i = va_arg(ap, int);
      s = buf;
      do {
	j = i / 16;
	*s++ = hexabuf[i - 16*j];
	fixed_len--;
      }
      while((i = j));
      while(fixed_len-->0)  put('0');
      do {
	i = *--s;
	put(i);
      }
      while(s > buf);
      continue;


    case 's':
      s = va_arg(ap, char*);
      while((i = *s++))
	{ put(i); }
      continue;
    default:
      bad(fmt);
    }
  }
 done:
  *outbuf = 0;
  return outbuf - ob0;
}
/*
  #ifdef __cplusplus
  }
  #endif
*/





/*TRUE STDLIB*/
#ifndef NULL
#define NULL ((unsigned char*)0)
#endif
#define _U      0x00000001      /* Upper case */
#define _L      0x00000002      /* Lower case */
#define _N      0x00000004      /* Numeral (digit) */
#define _S      0x00000008      /* Spacing character */
#define _P      0x00000010      /* Punctuation */
#define _C      0x00000020      /* Control character */
#define _B      0x00000040      /* Blank */
#define _X      0x00000080      /* heXadecimal digit */



char    _ctype_[] = { 0,

		      /*       0              1        2       3       4       5       6       7  */

		      /* 0*/  _C,             _C,     _C,     _C,     _C,     _C,     _C,     _C,
		      /* 10*/ _C,             _S|_C,  _S|_C,  _S|_C,  _S|_C,  _S|_C,  _C,     _C,
		      /* 20*/ _C,             _C,     _C,     _C,     _C,     _C,     _C,     _C,
		      /* 30*/ _C,             _C,     _C,     _C,     _C,     _C,     _C,     _C,
		      /* 40*/ (char)(_S|_B),  _P,     _P,     _P,     _P,     _P,     _P,     _P,
		      /* 50*/ _P,             _P,     _P,     _P,     _P,     _P,     _P,     _P,
		      /* 60*/ _N|_X,          _N|_X,  _N|_X,  _N|_X,  _N|_X,  _N|_X,  _N|_X,  _N|_X,
		      /* 70*/ _N|_X,          _N|_X,  _P,     _P,     _P,     _P,     _P,     _P,
		      /*100*/ _P,             _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U,
		      /*110*/ _U,             _U,     _U,     _U,     _U,     _U,     _U,     _U,
		      /*120*/ _U,             _U,     _U,     _U,     _U,     _U,     _U,     _U,
		      /*130*/ _U,             _U,     _U,     _P,     _P,     _P,     _P,     _P,
		      /*140*/ _P,             _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L,
		      /*150*/ _L,             _L,     _L,     _L,     _L,     _L,     _L,     _L,
		      /*160*/ _L,             _L,     _L,     _L,     _L,     _L,     _L,     _L,
		      /*170*/ _L,             _L,     _L,     _P,     _P,     _P,     _P,     _C,
		      /*200*/  0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0,
		      0,             0,       0,      0,      0,      0,      0,      0
};

unsigned int    _mb_cur_max;


int rr0d_isalpha(register int c)  
{
  return((_ctype_+1)[c]&(_U|_L));
}

int rr0d_isupper(register int c)
{
  return((_ctype_+1)[c]&_U);
}

int rr0d_islower(register int c)  
{
  return((_ctype_+1)[c]&_L);
}

int rr0d_isdigit(register int c)  
{
  return((_ctype_+1)[c]&_N);
}

int rr0d_isxdigit(register int c) 
{
  return((_ctype_+1)[c]&_X);
}


int rr0d_isspace(register int c)  
{
  return((_ctype_+1)[c]&_S);
}


int rr0d_ispunct(register int c)  
{
  return((_ctype_+1)[c]&_P);
}


int rr0d_isalnum(register int c)  
{
  return((_ctype_+1)[c]&(_U|_L|_N));
}


int rr0d_isprint(register int c)  
{
  return((_ctype_+1)[c]&(_P|_U|_L|_N|_B));
}


int rr0d_isgraph(register int c)  
{
  return((_ctype_+1)[c]&(_P|_U|_L|_N));
}


int rr0d_iscntrl(register int c)  
{
  return((_ctype_+1)[c]&_C);
}

int rr0d_isascii(register int c)  
{
  return((unsigned)(c)<=0177);
}

int rr0d_toascii(register int c)  
{
  return((c)&0177);
}





int rr0d_atoi( char *p)
{
  int n;
  int c, neg = 0;
  unsigned char   *up = (unsigned char *)p;

  if (!rr0d_isdigit(c = *up)) {
    while (rr0d_isspace(c))
      c = *++up;
    switch (c) {
    case '-':
      neg++;
      /* FALLTHROUGH */
    case '+':
      c = *++up;
    }
    if (!rr0d_isdigit(c))
      return (0);
  }
  for (n = '0' - c; rr0d_isdigit(c = *++up); ) {
    n *= 10; /* two steps to avoid unnecessary overflow */
    n += '0' - c; /* accum neg to avoid surprises at MAX */
  }
  return (neg ? n : -n);
}

char * rr0d_strchr( char *sp, int c)
{
  do {
    if (*sp == (char)c)
      return ((char *)sp);
  } while (*sp++);
  return (NULL);
}
int rr0d_strcmp( char *s1,  char *s2)
{
  if (s1 == s2)
    return (0);
  while (*s1 == *s2++)
    if (*s1++ == '\0')
      return (0);
  return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}

unsigned int rr0d_strlen( char *s)
{
         char *s0 = s + 1;

        while (*s++ != '\0')
                ;
        return (s - s0);
}
int rr0d_strnicmp ( char *s1, char *s2,size_t n)
{
    char c1,c2;

    if(n==0) return 0;

    do {
        c1 = *s1++; if(rr0d_islower(c1)) c1=rr0d_toupper(c1);
        c2 = *s2++; if(rr0d_islower(c2)) c2=rr0d_toupper(c2);
    } while(c1==c2 && c1 && c2 && --n>0);

    if(n==0 || c1==c2) return 0;
    if(c1=='\0' || c1<c2) return 1;
    return -1;
}

int rr0d_stricmp (char *s, char *t)
{
  int d = 0;
  do {
    d = rr0d_toupper(*s) - rr0d_toupper(*t);
  } while (*s++ && *t++ && !d);
  return (d);
}


char * rr0d_strcpy(char *s1,  char *s2)
{
  char *os1 = s1;

  while ((*s1++ = *s2++))
    ;
  return (os1);
}

unsigned  rr0d_strcspn( char *string,  char *charset)
{
   char *p, *q;

  for (q = string; *q != '\0'; ++q) {
    for (p = charset; *p != '\0' && *p != *q; ++p)
      ;
    if (*p != '\0')
      break;
  }
  return (q - string);
}



char *rr0d_strcat(char *s1,  char *s2)
{
  char *os1 = s1;

  while (*s1++)
    ;
  --s1;
  while ((*s1++ = *s2++))
    ;
  return (os1);
}


char *rr0d_strncat(char *s1,  char *s2, unsigned int n)
{
  char *os1 = s1;

  n++;
  while (*s1++)
    ;
  --s1;
  while ((*s1++ = *s2++))
    if (--n == 0) {
      s1[-1] = '\0';
      break;
    }
  return (os1);
}


int rr0d_strncmp( char *s1,  char *s2, unsigned int n)
{
  n++;
  if (s1 == s2)
    return (0);
  while (--n != 0 && *s1 == *s2++)
    if (*s1++ == '\0')
      return (0);
  return (n == 0 ? 0 : *(unsigned char *)s1 - *(unsigned char *)--s2);
}


char * rr0d_strncpy(char *s1,  char *s2, unsigned int n)
{
  char *os1 = s1;

  n++;
  while ((--n != 0) && ((*s1++ = *s2++) != '\0'))
    ;
  if (n != 0)
    while (--n != 0)
      *s1++ = '\0';
  return (os1);
}


char * rr0d_strpbrk( char *string,  char *brkset)
{
   char *p;

  do {
    for (p = brkset; *p != '\0' && *p != *string; ++p)
      ;
    if (*p != '\0')
      return ((char *)string);
  } while (*string++);

  return (NULL);
}


char * rr0d_strrchr( char *sp, int c)
{
  char *r = NULL;

  do {
    if (*sp == (char)c)
      r = (char *)sp;
  } while (*sp++);

  return (r);
}


unsigned int rr0d_strspn( char *string,  char *charset)
{
   char *p, *q;

  for (q = string; *q != '\0'; ++q) {
    for (p = charset; *p != '\0' && *p != *q; ++p)
      ;
    if (*p == '\0')
      break;
  }
  return (q - string);
}


char * rr0d_strstr( char *as1,  char *as2)
{
   char *s1, *s2;
   char *tptr;
  char c;

  s1 = as1;
  s2 = as2;

  if (s2 == ( char*)NULL || *s2 == '\0')
    return ((char *)s1);
  c = *s2;

  while (*s1)
    if (*s1++ == c) {
      tptr = s1;
      while ((c = *++s2) == *s1++ && c)
	;
      if (c == 0)
	return ((char *)tptr - 1);
      s1 = tptr;
      s2 = as2;
      c = *s2;
    }

  return (NULL);
}


char * rr0d_strtok_r(char *string,  char *sepset, char **lasts)
{
  char    *q, *r;

  /* first or subsequent call */
  if (string == ( char*)NULL)
    string = *lasts;

  if (string == ( char*)NULL)             /* return if no tokens remaining */
    return (NULL);

  q = string + rr0d_strspn(string, sepset);    /* skip leading separators */

  if (*q == '\0')         /* return if no tokens remaining */
    return (NULL);

  if ((r = rr0d_strpbrk(q, sepset)) == (char*)NULL)   /* move past token */
    *lasts = NULL;  /* indicate this is last token */
  else {
    *r = '\0';
    *lasts = r + 1;
  }
  return (q);
}



void* rr0d_memset(void* s, int c, unsigned int  n)
{
  unsigned int i;
  char *ss = (char*)s;

  for (i=0;i<n;i++) ss[i] = (char)c;
  return s;
}

void* rr0d_memcpy(void* __dest,  void* __src, unsigned int __n)
{
  unsigned int i;
  char *d = (char *)__dest, *s = (char *)__src;

  for (i=0;i<__n;i++) d[i] = s[i];
  return __dest;
}


