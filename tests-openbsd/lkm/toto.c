/*
 * ATTENTION : tel quel, le module crashe le kernel, c'est normal
 *
 * $Id: toto.c,v 1.9 2004-10-03 18:04:51 lalet Exp $
 */

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <sys/exec.h>
#include <sys/conf.h>
#include <sys/lkm.h>

MOD_MISC( "toto" )

unsigned int tarace;
unsigned char *ptr;

static int handler (struct lkm_table *lkmtp, int cmd) {
  int s;
  int error = 0;
  int i;

  switch (cmd) {
  case LKM_E_LOAD :

#if 0
  __asm__ __volatile__ ( "\t"
	"call pouetpouet\n"
	"pouetpouet:\n\t"
	"pop %eax\n\t"
	"mov %eax, tarace\n\t"
	);
  printf("tarace : 0x%.8X\n", tarace);
  ptr=tarace;
#endif

  ptr = (unsigned int) 0xCD000000;
  while (1) {
    printf("0x%.8X : 0x%X\n", ptr, ptr[0]);
    ptr-=0x01000000;
  }
  /* la, on est morts, de toute facon */
  
    break;
  case LKM_E_UNLOAD:
    break;
  default :
    error = EINVAL;
    break;
  }

  return error;
}

toto(struct lkm_table *lkmtp, int cmd, int ver) {
	DISPATCH(lkmtp, cmd, ver, handler, handler, lkm_nofunc)
}
