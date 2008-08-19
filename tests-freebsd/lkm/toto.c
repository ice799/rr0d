#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/sysproto.h>

typedef uint64_t pd_entry_t;
extern pd_entry_t PTD[];

extern pd_entry_t PTmap[];

unsigned int * my_ptd;
unsigned int my_cr3;

int tototiti;

//extern u_int KERNELBASEPDE;


unsigned int gdt[2];
unsigned int * ad_gdt;
unsigned int cs_val=0;
unsigned int ds_val=0;
unsigned int fs_val=0;
unsigned int gs_val=0;
unsigned int es_val=0;

unsigned int ss_val=0;
static int handler (struct module *module, int cmd, void *arg) {
  int s;
  int error = 0;
  int i;

  switch (cmd) {
  case MOD_LOAD :
    //    printf("%.8X\n", KERNELBASEPDE);

    printf("PTmap: %.8X\n", PTmap);
    printf("PTD: 0x%.8x\n", PTD);
    __asm__("mov %cr3, %eax");
    __asm__("mov %eax, my_cr3");
    printf("CR3: 0x%.8x\n", my_cr3);

    my_ptd = my_cr3;//(unsigned int *) PTD;

    my_ptd+=(0xc00>>2);
    printf("ptd: %.8X\n", my_ptd);


    printf("page info 0 %.8X\n", my_ptd[0]);

    /*
    for (i=0;i<1024;i++)
      {
	if (my_ptd[i]&1)
	  printf("info :%.8X\n", my_ptd[i]);
      }
    */

    //    printf("KERN_BASE: %.8X\n", PTD-my_cr3);
    __asm__("mov %cs, %ax\n mov %eax, cs_val");
    __asm__("mov %ds, %ax\n mov %eax, ds_val");
    __asm__("mov %gs, %ax\n mov %eax, fs_val");
    __asm__("mov %fs, %ax\n mov %eax, gs_val");
    __asm__("mov %es, %ax\n mov %eax, es_val");
    __asm__("mov %ss, %ax\n mov %eax, ss_val");
    printf("cs: 0x%x\n", cs_val);
    printf("ds: 0x%x\n", ds_val);
    printf("fs: 0x%x\n", fs_val);
    printf("gs: 0x%x\n", gs_val);
    printf("es: 0x%x\n", es_val);
    printf("ss: 0x%x\n", ss_val);



    break;
  case MOD_UNLOAD:
    break;
  default :
    error = EINVAL;
    break;
  }

  return error;
}

static moduledata_t toto_mod = {
  "toto",
  handler,
  NULL
};

DECLARE_MODULE(toto, toto_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
